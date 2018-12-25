/**
 * pricing_service.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_PRICING_SERVICE_HPP
#define TRADING_SYSTEM_PRICING_SERVICE_HPP

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "soa.hpp"
#include "products.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace boost::gregorian;


/**
 * A price object consisting of mid and bid/offer spread.
 * Type T is the product type.
 */
template<typename T>
class Price
{
private:
    T product;
    double mid;
    double bidOfferSpread;

public:
    // ctor for a price
    Price();
    Price(const T &_product, double _mid, double _bidOfferSpread);

    // Get the product
    const T& GetProduct() const;

    // Get the mid price
    double GetMid() const;

    // Get the bid/offer spread around the mid
    double GetBidOfferSpread() const;

};


/**
 * Pricing Service managing mid prices and bid/offers.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PricingService : public Service<string, Price<T>>
{
private:
    map<string, Price<T> > price_data;
    vector<ServiceListener<Price<T>>* > service_listeners;
    PricingService();

public:
    static PricingService* GenerateInstance(){
        static PricingService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    Price<T>& GetData(string key) override;

    void OnMessage(Price<T> &data) override;

    void AddListener(ServiceListener<Price<T>> *listener) override;

    const vector<ServiceListener<Price<T>>* >& GetListeners() const override;

};


/**
 * PricingServiceConnector subscribing from input
 * Type T is the product type.
 */
template<typename T>
class PricingServiceConnector : public Connector<Price<T>> {
private:
    PricingService<T>* pricing_service;
    PricingServiceConnector();

public:
    static PricingServiceConnector* GenerateInstance(){
        static PricingServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(Price<T> &data) override;

    void Subscribe() override;

    PricingService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of Price template class
template<typename T>
Price<T>::Price():product(T()){

}

template<typename T>
Price<T>::Price(const T &_product, double _mid, 
                double _bidOfferSpread) : product(_product){
    mid = _mid;
    bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const{
    return product;
}

template<typename T>
double Price<T>::GetMid() const{
    return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const{
    return bidOfferSpread;
}


//
// Implementation of PricingService template class
template<typename T>
PricingService<T>::PricingService() {

}

template<typename T>
Price<T>& PricingService<T>::GetData(string key) {
    return price_data[key];
}

template<typename T>
void PricingService<T>::OnMessage(Price<T> &data) {
    const string product_id = data.GetProduct().GetProductId();
    if (price_data.find(product_id) == price_data.end()){
        price_data.insert(make_pair(product_id, data));
    }
    else{
        price_data[product_id] = data;
    }
    for(auto listener : service_listeners){
        listener->ProcessAdd(data);
    }
}

template<typename T>
void PricingService<T>::AddListener(ServiceListener<Price<T>> *listener) {
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Price<T>>* >& PricingService<T>::GetListeners() const{
    return service_listeners;
}


//
// Implementation of PricingServiceConnector template class
template<typename T>
PricingServiceConnector<T>::PricingServiceConnector() {
    pricing_service = PricingService<T>::GenerateInstance();
}

template<typename T>
void PricingServiceConnector<T>::Publish(Price<T> &data){

}

template<typename T>
void PricingServiceConnector<T>::Subscribe() {
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
    data.open("../input/prices.txt", ios::in);
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
        // Construction of Price<Bond>
        double mid = String2Price(line_fragments[1]);
        double spread = String2Price(line_fragments[2]);
        Price<Bond> price_bond(bond, mid, spread);
        pricing_service->OnMessage(price_bond);
    }
    data.close();
}

template<typename T>
PricingService<T>* PricingServiceConnector<T>::GetService() {
    return pricing_service;
}

#endif //TRADING_SYSTEM_PRICING_SERVICE_HPP
