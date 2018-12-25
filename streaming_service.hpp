/**
 * streaming_service.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */
#ifndef TRADING_SYSTEM_STREAMING_SERVICE_HPP
#define TRADING_SYSTEM_STREAMING_SERVICE_HPP

#include <map>
#include <vector>

#include "soa.hpp"
#include "products.hpp"
#include "pricing_service.hpp"
#include "market_data_service.hpp"


/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{
private:
    double price;
    long visibleQuantity;
    long hiddenQuantity;
    PricingSide side;

public:
    // ctors
    PriceStreamOrder();
    PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, 
                     PricingSide _side);
    
    // getters
    PricingSide GetSide() const;

    double GetPrice() const;
    
    long GetVisibleQuantity() const;
    
    long GetHiddenQuantity() const;

};


/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template<typename T>
class PriceStream
{
private:
    T product;
    PriceStreamOrder bidOrder;
    PriceStreamOrder offerOrder;

public:
    // ctors
    PriceStream();
    PriceStream(const T &_product, const PriceStreamOrder &_bidOrder,
                const PriceStreamOrder &_offerOrder);
    
    // getters
    const T& GetProduct() const;
    
    const PriceStreamOrder& GetBidOrder() const;
    
    const PriceStreamOrder& GetOfferOrder() const;

};


/**
 * AlgoStream with a reference to PriceStream
 * Type T is the product type.
 */
template<typename T>
class AlgoStream{
private:
    PriceStream<T> price_stream;

public:
    // ctors
    AlgoStream();
    AlgoStream(const Price<T>& new_price);

    // getters
    const PriceStream<T>& GetPriceStream() const;

    // modifiers
    void UpdateAlgoStream(const Price<T> &price);

};


/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string, PriceStream <T> >{
private:
    map<string, PriceStream<T>> streaming_data;
    vector<ServiceListener<PriceStream<T>>*> service_listeners;
    StreamingService();

public:
    static StreamingService* GenerateInstance(){
        static StreamingService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    PriceStream<T>& GetData(string key) override;

    void OnMessage(PriceStream<T> &data) override;

    void AddListener(ServiceListener<PriceStream<T>>* listener) override;

    const vector<ServiceListener<PriceStream<T>>* >& GetListeners() const override;

    void PublishPrice(PriceStream<T>& price_stream);

};


/** AlgoStreamingService to publish two-way prices.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class AlgoStreamingService : public Service<string, AlgoStream<T>>{
private:
    map<string, AlgoStream<T>> algo_streaming_data;
    vector<ServiceListener<AlgoStream<T>>*> service_listeners;
    AlgoStreamingService();

public:
    static AlgoStreamingService* GenerateInstance(){
        static AlgoStreamingService instance;
        return &instance;
    }
    // Override virtual functions in base class Service
    AlgoStream<T>& GetData(string key) override;

    void OnMessage(AlgoStream<T> &data) override;

    void AddListener(ServiceListener<AlgoStream<T>>* listener) override;

    const vector<ServiceListener<AlgoStream<T>>*>& GetListeners()const override;

    void AddPrice(const Price<T>& price);

};


/** AlgoStreamingServiceListener listen to PricingService
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class AlgoStreamingServiceListener : public ServiceListener<Price<T> >{
private:
    AlgoStreamingService<T>* algo_streaming_service;
    AlgoStreamingServiceListener();

public:
    static AlgoStreamingServiceListener* GenerateInstance() {
        static AlgoStreamingServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(Price<T> & data) override;

    void ProcessRemove(Price<T> &data) override;

    void ProcessUpdate(Price<T> &data) override;
    
    AlgoStreamingService<T>* GetService();

};


/** StreamingServiceListener listen to AlgoStreamingService
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class StreamingServiceListener : public ServiceListener<AlgoStream<T> >{
private:
    StreamingService<T>* streaming_service;
    StreamingServiceListener();

public:
    static StreamingServiceListener* GenerateInstance() {
        static StreamingServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(AlgoStream<T> & data) override;

    void ProcessRemove(AlgoStream<T> &data) override;
    
    void ProcessUpdate(AlgoStream<T> &data) override;
    
    StreamingService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of PriceStreamOrder class
PriceStreamOrder::PriceStreamOrder(){
    price = 0.0;
    visibleQuantity = 0;
    hiddenQuantity = 0;
    side = BID;
}

PriceStreamOrder::PriceStreamOrder(double _price, long _visibleQuantity,
                                   long _hiddenQuantity, PricingSide _side){
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    side = _side;
}

double PriceStreamOrder::GetPrice() const{
    return price;
}

long PriceStreamOrder::GetVisibleQuantity() const{
    return visibleQuantity;
}

long PriceStreamOrder::GetHiddenQuantity() const{
    return hiddenQuantity;
}


//
// Implementation of PriceStream
template<typename T>
PriceStream<T>::PriceStream() :product(T()), bidOrder(PriceStreamOrder()),
                               offerOrder(PriceStreamOrder()){
}

template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder,
        const PriceStreamOrder &_offerOrder): product(_product),
        bidOrder(_bidOrder), offerOrder(_offerOrder){
}

template<typename T>
const T& PriceStream<T>::GetProduct() const{
    return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const{
    return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const{
    return offerOrder;
}


//
// Implementation of StreamingService
template<typename T>
StreamingService<T>::StreamingService(){

}

template<typename T>
PriceStream<T>& StreamingService<T>::GetData(string key){
    return streaming_data[key];
}

template<typename T>
void StreamingService<T>::OnMessage(PriceStream<T> &data){
    
}

template<typename T>
void StreamingService<T>::AddListener(
        ServiceListener<PriceStream<T>>* listener){
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<PriceStream<T>>* >&
StreamingService<T>::GetListeners() const {
    return service_listeners;
}


template<typename T>
void StreamingService<T>::PublishPrice(PriceStream<T>& price_stream){
    const string product_id = price_stream.GetProduct().GetProductId();
    if (streaming_data.find(product_id) == streaming_data.end()) {
        streaming_data.insert(make_pair(product_id, price_stream));
    }
    else {
        streaming_data[product_id] = price_stream;
    }
    for (auto& listener : service_listeners) {
        listener->ProcessAdd(price_stream);
    }
}


//
// Implementation of AlgoStream class
template<typename T>
AlgoStream<T>::AlgoStream() :  price_stream(PriceStream<T>()){

}

template<typename T>
AlgoStream<T>::AlgoStream(const Price<T>& price){
    T new_product = price.GetProduct();
    double new_mid = price.GetMid();
    double new_spread = price.GetBidOfferSpread();
    double new_bid = new_mid - 0.5 * new_spread;
    double new_ask = new_mid + 0.5 * new_spread;
    long new_visible_size = (1 + rand() % 2) * 1000000;
    long new_hidden_size = new_visible_size * 2;
    PriceStreamOrder new_bid_order(new_bid, new_visible_size,
                                   new_hidden_size, BID);
    PriceStreamOrder new_ask_order(new_ask, new_visible_size,
                                   new_hidden_size, OFFER);
    price_stream = PriceStream<T>(new_product, new_bid_order, new_ask_order);
}

template<typename T>
const PriceStream<T>& AlgoStream<T>::GetPriceStream() const{
    return price_stream;
}

template<typename T>
void AlgoStream<T>::UpdateAlgoStream(const Price<T> &price) {
    // If update with different id, do nothing
    if(price.GetProduct().GetProductId() !=
       price_stream.GetProduct().GetProductId()){
        return;
    }
    double new_mid = price.GetMid();
    double new_spread = price.GetBidOfferSpread();
    double new_bid = new_mid - 0.5 * new_spread;
    double new_ask = new_mid + 0.5 * new_spread;
    long new_visible_size = (1 + rand() % 2) * 1000000;
    long new_hidden_size = new_visible_size * 2;
    PriceStreamOrder new_bid_order(new_bid, new_visible_size,
                                   new_hidden_size, BID);
    PriceStreamOrder new_ask_order(new_ask, new_visible_size,
                                   new_hidden_size, OFFER);
    price_stream = PriceStream<T>(price.GetProduct(), new_bid_order,
                                    new_ask_order);
}


//
// Implementation of AlgoStreamingService class
template<typename T>
AlgoStreamingService<T>::AlgoStreamingService() {

}

template<typename T>
AlgoStream<T> & AlgoStreamingService<T>::GetData(string key){
    return algo_streaming_data[key];
}

template<typename T>
void AlgoStreamingService<T>::OnMessage(AlgoStream<T>& data){
}

template<typename T>
void AlgoStreamingService<T>::AddListener(ServiceListener<AlgoStream<T>> *listener) {
    service_listeners.push_back(listener);
}
template<typename T>
const vector<ServiceListener<AlgoStream<T>> *>& AlgoStreamingService<T>::GetListeners() const{
    return service_listeners;
}

template<typename T>
void AlgoStreamingService<T>::AddPrice(const Price<T>& price){
    const string product_id = price.GetProduct().GetProductId();
    if (algo_streaming_data.find(product_id) == algo_streaming_data.end()) {
        algo_streaming_data.insert(make_pair(product_id, AlgoStream<T>(price)));
    }
    else {
        algo_streaming_data[product_id].UpdateAlgoStream(price);
    }
    for (auto& listener : service_listeners) {
        listener->ProcessAdd(algo_streaming_data[product_id]);
    }
}


//
// Implementation of AlgoStreamingServiceListener class
template<typename T>
AlgoStreamingServiceListener<T>::AlgoStreamingServiceListener(){
    algo_streaming_service = AlgoStreamingService<T>::GenerateInstance();
}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessAdd(Price<T> & data){
    algo_streaming_service->AddPrice(data);
}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessRemove(Price<T> &data){

}

template<typename T>
void AlgoStreamingServiceListener<T>::ProcessUpdate(Price<T> &data){

}

template<typename T>
AlgoStreamingService<T>* AlgoStreamingServiceListener<T>::GetService(){
    return algo_streaming_service;
}


//
// Implementation of StreamingServiceListener class
template<typename T>
StreamingServiceListener<T>::StreamingServiceListener(){
    streaming_service = StreamingService<T>::GenerateInstance();
}

template<typename T>
void StreamingServiceListener<T>::ProcessAdd(AlgoStream<T> & data){
    auto price_stream = data.GetPriceStream();
    streaming_service->PublishPrice(price_stream);
}

template<typename T>
void StreamingServiceListener<T>::ProcessRemove(AlgoStream<T> &data){

}

template<typename T>
void StreamingServiceListener<T>::ProcessUpdate(AlgoStream<T> &data){

}

template<typename T>
StreamingService<T>*  StreamingServiceListener<T>::GetService(){
    return streaming_service;
}

#endif //TRADING_SYSTEM_STREAMING_SERVICE_HPP
