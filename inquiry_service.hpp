/**
 * inquiry_service.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */
#ifndef TRADING_SYSTEM_INQUIRY_SERVICE_HPP
#define TRADING_SYSTEM_INQUIRY_SERVICE_HPP

#include <string>
#include "soa.hpp"
#include "trade_booking_service.hpp"

// Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{
private:
    string inquiryId;
    T product;
    Side side;
    long quantity;
    double price;
    InquiryState state;
public:

    // ctor for an inquiry
    Inquiry();
    Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity,
            double _price, InquiryState _state);

    // Get the inquiry ID
    const string& GetInquiryId() const;

    // Get the product
    const T& GetProduct() const;

    // Get the side on the inquiry
    Side GetSide() const;

    // Get the quantity that the client is inquiring for
    long GetQuantity() const;

    // Get the price that we have responded back with
    double GetPrice() const;

    // Get the current state on the inquiry
    InquiryState GetState() const;

    // Set the current price
    void SetPrice(double new_price);

    // Set the current price
    void SetState(InquiryState new_state);

};



/**
 * Service for customer inquiry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since
 * each inquiry must be unique).
 * Type T is the product type.
 */
template<typename T>
class InquiryService : public Service<string,Inquiry <T> >
{
private:
    map<string, Inquiry<T>> inquiry_data;
    vector<ServiceListener<Inquiry<T>> *> service_listeners;
    InquiryService();
public:
    static InquiryService<T>* GenerateInstance(){
        static InquiryService<T> instance;
        return &instance;
    }
    // Override virtual functions in base class Service
    Inquiry<T>& GetData(string key) override;
    void OnMessage(Inquiry<T> &data) override;
    void AddListener(ServiceListener<Inquiry<T>> *listener) override;
    const vector<ServiceListener<Inquiry<T>>* >& GetListeners() const override;
    // Send a quote back to the client
    void SendQuote(const string &inquiryId, double price);
    // Reject an inquiry from the client
    void RejectInquiry(const string &inquiryId);

};


/**
 * InquiryServiceConnector subscribing from input
 * Type T is the product type.
 */
template<typename T>
class InquiryServiceConnector : public Connector<Inquiry<T> > {
private:
    InquiryService<T>* inquiry_service;
    InquiryServiceConnector();
public:
    static InquiryServiceConnector<T>* GenerateInstance(){
        static InquiryServiceConnector<T> instance;
        return &instance;
    }
    void Publish(Inquiry<T>& data) override;
    void Subscribe() override;
    InquiryService<T>* GetService();
};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of Inquiry class
template<typename T>
Inquiry<T>::Inquiry() : product(T())
{
    inquiryId = "DefaultInquiryTest";
    side = BUY;
    quantity = 1;
    price = 0.0;
    state = RECEIVED;
}

template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side,
        long _quantity, double _price, InquiryState _state) : product(_product)
{
    inquiryId = _inquiryId;
    side = _side;
    quantity = _quantity;
    price = _price;
    state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
    return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
    return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
    return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
    return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
    return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
    return state;
}

template<typename T>
void Inquiry<T>::SetPrice(double new_price){
    price = new_price;
}

template<typename T>
void Inquiry<T>::SetState(InquiryState new_state){
    state = new_state;
}


//
// Implementation of InquiryService class
template <typename T>
InquiryService<T>::InquiryService() {

}

template <typename T>
Inquiry<T>& InquiryService<T>::GetData(string key) {
    return inquiry_data[key];
}

template <typename T>
void InquiryService<T>::OnMessage(Inquiry<T> &data) {
    if (data.GetState() == QUOTED){
        data.SetState(DONE);
    }
    const string inquiry_id = data.GetInquiryId();
    if (inquiry_data.find(inquiry_id) == inquiry_data.end()){
        inquiry_data.insert(make_pair(inquiry_id, data));
    }
    else{
        inquiry_data[inquiry_id] = data;
    }
    for(auto listener : service_listeners) {
        listener->ProcessAdd(data);
    }
    this->SendQuote(inquiry_id, 100);
}

template <typename T>
void InquiryService<T>::AddListener(ServiceListener<Inquiry<T>> *listener) {
    service_listeners.push_back(listener);
}

template <typename T>
const vector<ServiceListener<Inquiry<T>>* >& InquiryService<T>::GetListeners() const {
    return service_listeners;
}

template <typename T>
void InquiryService<T>::SendQuote(const string &inquiryId, double price){
    if (inquiry_data[inquiryId].GetState() == RECEIVED) {
        inquiry_data[inquiryId].SetPrice(price);
        auto inquiry_service_connector = InquiryServiceConnector<T>::GenerateInstance();
        inquiry_service_connector->Publish(inquiry_data[inquiryId]);
    }
}


template <typename T>
void InquiryService<T>::RejectInquiry(const string &inquiryId){

}

//
// Implementation of MarketDataServiceConnector class
template<typename T>
InquiryServiceConnector<T>::InquiryServiceConnector() {
    inquiry_service = InquiryService<T>::GenerateInstance();
}
template<typename T>
void InquiryServiceConnector<T>::Publish(Inquiry<T>& data) {
    data.SetState(QUOTED);
    inquiry_service->OnMessage(data);
}

template<typename T>
void InquiryServiceConnector<T>::Subscribe() {
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
    auto String2State = [](string s){
        vector<InquiryState> states{ RECEIVED, QUOTED, DONE,
                                     REJECTED, CUSTOMER_REJECTED };
        vector<string> strings{"RECEIVED", "QUOTED", "DONE",
                                "REJECTED", "CUSTOMER_REJECTED"};
        for(int i = 0; i < 5; ++i){
            if(s == strings[i]){
                return states[i];
            }
        }
    };
    ifstream data;
    data.open("../input/inquiries.txt", ios::in);
    string line;
    getline(data, line);
    while(getline(data, line)){
        vector<string> line_fragments = SplitLine(line, ',');
        string inquiry_id = line_fragments[0];
        // Construction of Bond
        string cusip = line_fragments[1];
        string ticker = "NoTicker";
        float coupon = 0.0;
        date maturity_date = FindMaturity(cusip);
        Bond bond(cusip, CUSIP, ticker, coupon, maturity_date);
        // Construction of Inquiry<Bond>
        long quantity = stol(line_fragments[2]);
        Side side = (line_fragments[3] == "BUY") ? BUY : SELL;
        double price = String2Price(line_fragments[4]);
        InquiryState inquiry_state = String2State(line_fragments[5]);
        Inquiry<Bond> bond_inquiry(inquiry_id, bond, side,
                                   quantity, price, inquiry_state);
        inquiry_service->OnMessage(bond_inquiry);
    }
}

template<typename T>
InquiryService<T>* InquiryServiceConnector<T>::GetService() {
    return inquiry_service;
}




#endif //TRADING_SYSTEM_INQUIRY_SERVICE_HPP
