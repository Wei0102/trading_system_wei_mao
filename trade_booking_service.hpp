/**
 * trade_booking_service.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_TRADE_BOOKING_SERVICE_HPP
#define TRADING_SYSTEM_TRADE_BOOKING_SERVICE_HPP

#include <map>
#include <string>
#include <vector>
#include "soa.hpp"
#include "products.hpp"
#include "execution_service.hpp"

// Trade sides
enum Side { BUY, SELL };


/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade{
private:
    T product;
    string tradeId;
    double price;
    string book;
    long quantity;
    Side side;

public:
    // ctors
    Trade();
    Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side);
    
    // getters
    const T& GetProduct() const;
    
    const string& GetTradeId() const;
    
    double GetPrice() const;
    
    const string& GetBook() const;
    
    long GetQuantity() const;
    
    Side GetSide() const;

};


/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on trade id.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string,Trade <T> >
{
private:
    map<string, Trade<T>> trade_data;
    vector<ServiceListener<Trade<T>> *> service_listeners;
    TradeBookingService();

public:
    static TradeBookingService<T>* GenerateInstance(){
        static TradeBookingService<T> instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    Trade<T>& GetData(string key) override;

    void OnMessage(Trade<T> &data) override;

    void AddListener(ServiceListener<Trade<T>> *listener) override;

    const vector<ServiceListener<Trade<T>>* >& GetListeners() const override;

    void BookTrade(const ExecutionOrder<T> &execution_order);

};


/**
 * TradeBookingServiceConnector subscribing from input
 * Type T is the product type.
 */
template<typename T>
class TradeBookingServiceConnector : public Connector<Trade<T>>{
private:
    TradeBookingService<T>* trade_booking_service;
    TradeBookingServiceConnector();

public:
    static TradeBookingServiceConnector<T>* GenerateInstance(){
        static TradeBookingServiceConnector<T> instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(Trade<T>& data) override;
    
    void Subscribe() override;
    
    TradeBookingService<T>* GetService();

};


/** 
* TradeBookingServiceListener listen to ExecutionService
* Type T is the product type.
*/
template <typename T>
class TradeBookingServiceListener : public ServiceListener<ExecutionOrder<T> > {
private:
    TradeBookingService<T>* trade_booking_service;
    TradeBookingServiceListener();

public:
    static TradeBookingServiceListener* GenerateInstance(){
        static TradeBookingServiceListener instance;
        return &instance;
    }
 
    // Override virtual functions in base class Service
    void ProcessAdd(ExecutionOrder<T> & data) override;
 
    void ProcessRemove(ExecutionOrder<T> &data) override;
 
    void ProcessUpdate(ExecutionOrder<T> &data) override;
 
    TradeBookingService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of Trade class
template<typename T>
Trade<T>::Trade() : product(T()){
}

template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, double _price, string _book,
        long _quantity, Side _side) : product(_product){
    tradeId = _tradeId;
    price = _price;
    book = _book;
    quantity = _quantity;
    side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const{
    return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const{
    return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const{
    return price;
}

template<typename T>
const string& Trade<T>::GetBook() const{
    return book;
}

template<typename T>
long Trade<T>::GetQuantity() const{
    return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const{
    return side;
}


//
// Implementation of TradeBookingService class
template<typename T>
TradeBookingService<T>::TradeBookingService(){

}

template<typename T>
Trade<T>& TradeBookingService<T>::GetData(string key){
    return trade_data[key];
}

template<typename T>
void TradeBookingService<T>::OnMessage(Trade<T> &data){
    const string trade_id = data.GetTradeId();
    if (trade_data.find(trade_id) == trade_data.end()){
        trade_data.insert(make_pair(trade_id, data));
    }
    else{
        trade_data[trade_id] = data;
    }
    for(auto listener : service_listeners) {
        listener->ProcessAdd(data);
    }
}

template<typename T>
void TradeBookingService<T>::AddListener(ServiceListener<Trade<T>> *listener){
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Trade<T>>* >& TradeBookingService<T>::GetListeners() const{
    return service_listeners;
}

template<typename T>
void TradeBookingService<T>::BookTrade(const ExecutionOrder<T> &execution_order){
    static int order_count = 0;
    order_count ++;
    T product = execution_order.GetProduct();
    Side side = (execution_order.GetSide() == BID) ? BUY : SELL;
    long quantity = execution_order.GetVisibleQuantity() +
            execution_order.GetHiddenQuantity();
    double price = execution_order.GetPrice();
    string book = "TSY" + to_string(order_count % 3 + 1);
    string trade_id = "ETrade"+to_string(order_count);
    Trade<T> trade(product, trade_id, price, book, quantity, side);
    this->OnMessage(trade);
}


//
// Implementation of TradeBookingServiceConnector class
template<typename T>
TradeBookingServiceConnector<T>::TradeBookingServiceConnector() {
    trade_booking_service = TradeBookingService<T>::GenerateInstance();
}

template<typename T>
void TradeBookingServiceConnector<T>::Publish(Trade<T>& data){
}

template<typename T>
void TradeBookingServiceConnector<T>::Subscribe(){
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
    data.open("../input/trades.txt", ios::in);
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
        // Construction of Trade<Bond>
        string trade_id = line_fragments[1];
        double trade_price = String2Price(line_fragments[2]);
        long trade_quantity = stol(line_fragments[3]);
        string trade_book = line_fragments[4];
        Side trade_side = (line_fragments[5] == "SELL") ? SELL : BUY;
        Trade<Bond> bond_trade(bond, trade_id, trade_price,
                    trade_book, trade_quantity, trade_side);
        trade_booking_service->OnMessage(bond_trade);
    }
}

template<typename T>
TradeBookingService<T>* TradeBookingServiceConnector<T>::GetService(){
    return trade_booking_service;
}


//
// Implementation of TradeBookingServiceListener class
template<typename T>
TradeBookingServiceListener<T>::TradeBookingServiceListener(){
    trade_booking_service = TradeBookingService<T>::GenerateInstance();
}

template<typename T>
void TradeBookingServiceListener<T>::ProcessAdd(ExecutionOrder<T> & data){
    trade_booking_service->BookTrade(data);
}

template<typename T>
void TradeBookingServiceListener<T>::ProcessRemove(ExecutionOrder<T> &data){

}

template<typename T>
void TradeBookingServiceListener<T>::ProcessUpdate(ExecutionOrder<T> &data){

}

template<typename T>
TradeBookingService<T>* TradeBookingServiceListener<T>::GetService(){
    return trade_booking_service;
}


#endif //TRADING_SYSTEM_TRADE_BOOKING_SERVICE_HPP
