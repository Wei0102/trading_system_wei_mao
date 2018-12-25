/**
 * market_data_service.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_MARKET_DATA_SERVICE_HPP
#define TRADING_SYSTEM_MARKET_DATA_SERVICE_HPP

#include <string>
#include <vector>
#include "soa.hpp"

using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };


/**
 * A market data order with price, quantity, and side.
 */
class Order{
private:
    double price;
    long quantity;
    PricingSide side;

public:
    // ctors
    Order();
    Order(double _price, long _quantity, PricingSide _side);

    // getters
    double GetPrice() const;

    long GetQuantity() const;

    PricingSide GetSide() const;

};


/**
 * Class representing a bid and offer order
 */
class BidOffer{
private:
    Order bidOrder;
    Order offerOrder;

public:
    // ctors
    BidOffer(const Order &_bidOrder, const Order &_offerOrder);

    // getters
    const Order& GetBidOrder() const;

    const Order& GetOfferOrder() const;

};


/**
 * Order book with a bid and offer stack.
 * Type T is the product type.
 */
template<typename T>
class OrderBook{
private:
    T product;
    vector<Order> bidStack;
    vector<Order> offerStack;

public:
    // ctor for the order book
    OrderBook();
    OrderBook(const T &_product, const vector<Order> &_bidStack, 
              const vector<Order> &_offerStack);

    // Get the product
    const T& GetProduct() const;

    const vector<Order>& GetBidStack() const;

    const vector<Order>& GetOfferStack() const;
};


/**
 * Market Data Service which distributes market data
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class MarketDataService : public Service<string,OrderBook <T> >{
private:
    map<string, OrderBook<T>> market_data;
    vector<ServiceListener<OrderBook<T>> *> service_listeners;
    MarketDataService();

public:
    static MarketDataService<T>* GenerateInstance(){
        static MarketDataService<T> instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    OrderBook<T>& GetData(string key) override;

    void OnMessage(OrderBook<T> &data) override;

    void AddListener(ServiceListener<OrderBook<T>> *listener) override;

    const vector<ServiceListener<OrderBook<T>>* >& GetListeners() const override;

    // Get the best bid/offer order
    const BidOffer& GetBestBidOffer(const string &productId);

    // Aggregate the order book
    const OrderBook<T>& AggregateDepth(const string &productId);

};


/**
 * MarketDataServiceConnector subscribing from input
 * Type T is the product type.
 */
template<typename T>
class MarketDataServiceConnector : public Connector<OrderBook<T> > {
private:
    MarketDataService<T>* market_data_service;
    MarketDataServiceConnector();

public:
    static MarketDataServiceConnector<T>* GenerateInstance(){
        static MarketDataServiceConnector<T> instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(OrderBook<T>& data) override;

    void Subscribe() override;

    MarketDataService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of Order class
Order::Order(){
    price = 0;
    quantity = 0;
    side = OFFER;
}


Order::Order(double _price, long _quantity, PricingSide _side){
    price = _price;
    quantity = _quantity;
    side = _side;
}

double Order::GetPrice() const{
    return price;
}

long Order::GetQuantity() const{
    return quantity;
}

PricingSide Order::GetSide() const{
    return side;
}


//
// Implementation of BidOffer class
BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
        bidOrder(_bidOrder), offerOrder(_offerOrder){
}

const Order& BidOffer::GetBidOrder() const{
    return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const{
    return offerOrder;
}


//
// Implementation of OrderBook class
template<typename T>
OrderBook<T>::OrderBook() : product(T()){

}

template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack,
                        const vector<Order> &_offerStack) : product(_product),
                        bidStack(_bidStack), offerStack(_offerStack){

}

template<typename T>
const T& OrderBook<T>::GetProduct() const{
    return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const{
    return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const{
    return offerStack;
}



//
// Implementation of MarketDataService class
template <typename T>
MarketDataService<T>::MarketDataService() {
}

template <typename T>
OrderBook<T>& MarketDataService<T>::GetData(string key) {
    return market_data[key];
}

template <typename T>
void MarketDataService<T>::OnMessage(OrderBook<T> &data) {
    const string product_id = data.GetProduct().GetProductId();
    if (market_data.find(product_id) == market_data.end()){
        market_data.insert(make_pair(product_id, data));
    }
    else{
        market_data[product_id] = data;
    }
    for(auto listener : service_listeners) {
        listener->ProcessAdd(data);
    }
}

template <typename T>
void MarketDataService<T>::AddListener(ServiceListener<OrderBook<T>> *listener) {
    service_listeners.push_back(listener);
}

template <typename T>
const vector<ServiceListener<OrderBook<T>>* >& MarketDataService<T>::GetListeners() const {
    return service_listeners;
}

template <typename T>
const BidOffer& MarketDataService<T>::GetBestBidOffer(const string &productId) {
    OrderBook<T> book = market_data[productId];
    return BidOffer(*book.GetBidStack().begin(), *book.GetOfferStack().begin());
}

template <typename T>
const OrderBook<T>& MarketDataService<T>::AggregateDepth(const string &productId) {
    OrderBook<T> book = market_data[productId];
    auto AggregateOrderStack = [](vector<Order> &order_stack){
        vector<Order> aggregate_order_stack;
        PricingSide side = order_stack.begin()->GetSide();
        long running_aggregate_quantity = order_stack.begin()->GetQuantity();
        double running_price = order_stack.begin()->GetPrice();
        for(int i = 1; i <= order_stack.size(); ++i){
            if (order_stack[i].GetPrice() == order_stack[i-1].GetPrice()){
                running_aggregate_quantity += order_stack[i].GetQuantity();
            }
            else{
                aggregate_order_stack.push_back(Order(running_price,
                    running_aggregate_quantity, side));
                running_price = order_stack[i].GetPrice();
                running_aggregate_quantity = order_stack[i].GetQuantity();
            }
        }
        return aggregate_order_stack;        
    };
    return OrderBook<T>(book.GetProduct(), 
                        AggregateOrderStack(book.GetBidStack()),
                        AggregateOrderStack(book.GetOfferStack()));

}


//
// Implementation of MarketDataServiceConnector class
template<typename T>
MarketDataServiceConnector<T>::MarketDataServiceConnector() {
    market_data_service = MarketDataService<T>::GenerateInstance();
}

template<typename T>
void MarketDataServiceConnector<T>::Publish(OrderBook<T>& data) {
}

template<typename T>
void MarketDataServiceConnector<T>::Subscribe() {
    auto SplitLine = [](string line, char delimiter){
        vector<string> tokens;
        string token;
        istringstream tokenStream(line);
        while (getline(tokenStream, token, delimiter)){
            tokens.push_back(token);
        }
        return tokens;
    };
    auto String2Price = [&](string s){
        vector<string> split = SplitLine(s, '-');
        double price = stoi(split[0]);
        if(split[1][2] == '+')
            split[1][2] = '4';
        price += stoi(split[1].substr(0,2)) / 32.0;
        price += stoi(split[1].substr(2,3)) / 256.0;
        return price;
    };
    auto FindMaturity = [](string cusip){
        vector<string> cusip_codes = {"9128285Q9", "9128285R7", "9128285P1",
                                      "9128285N6", "9128285M8", "912810SE9"};
        vector<date> dates = {date(2020, 11, 30), date(2021, 12, 15),
                              date(2023, 11, 30), date(2025, 11, 30),
                              date(2028, 12, 15), date(2048, 11, 15)};
        for(int i = 0; i < 6; ++i){
            if(cusip == cusip_codes[i])
                return dates[i];
        }
    };
    ifstream data;
    data.open("../input/marketdata.txt", ios::in);
    string line;
    getline(data, line);
    while(getline(data, line)){
        vector<string> line_fragments = SplitLine(line, ',');
        // Construction of Bond
        string cusip = line_fragments[0];
        string ticker = "NoTicker";
        float coupon = 0.0;
        date maturity_date = FindMaturity(cusip);
        Bond bond(cusip, CUSIP, ticker, coupon, maturity_date);
        // Construction of OrderBook<Bond>
        vector<Order> bid_stack;
        vector<Order> ask_stack;
        for (auto i = 0; i < 5; ++i) {
            double bid_price = String2Price(line_fragments[1+i*4]);
            double ask_price = String2Price(line_fragments[3+i*4]);
            long bid_quantity = stol(line_fragments[2+i*4]);
            long ask_quantity = stol(line_fragments[4+i*4]);
            bid_stack.push_back(Order(bid_price, bid_quantity, BID));
            ask_stack.push_back(Order(ask_price, ask_quantity, OFFER));
        }
        OrderBook<Bond> bond_order_book(bond, bid_stack, ask_stack);
        market_data_service->OnMessage(bond_order_book);
    }
}

template<typename T>
MarketDataService<T>* MarketDataServiceConnector<T>::GetService() {
    return market_data_service;
}




#endif //TRADING_SYSTEM_MARKET_DATA_SERVICE_HPP
