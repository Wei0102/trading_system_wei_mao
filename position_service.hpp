/**
 * position_service.hpp
 * Defines the data types and Service for positions.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */
#ifndef TRADING_SYSTEM_POSITION_SERVICE_HPP
#define TRADING_SYSTEM_POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "products.hpp"
#include "trade_booking_service.hpp"

using namespace std;


/**
 * Position class for a particular product across multiple books.
 * Keyed on book name
 * Type T is the product type.
 */
template<typename T>
class Position{
private:
    T product;
    map<string, long> positions;

public:
    // ctors
    Position();
    Position(const T &_product);

    // getters
    const T& GetProduct() const;
    long GetPosition(string &book);
    long GetAggregatePosition();

    // modifiers
    void UpdatePosition(const Trade<T> &trade);

};


/**
 * Position Service to manage positions across multiple books and securities.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string, Position<T> >{
private:
    map<string, Position<T>> position_data;
    vector<ServiceListener<Position<T>> *> service_listeners;
    PositionService();

public:
    static PositionService* GenerateInstance(){
        static PositionService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    Position<T>& GetData(string key) override;

    void OnMessage(Position<T> &data) override;

    void AddListener(ServiceListener<Position<T>>* listener) override;

    const vector<ServiceListener<Position<T>>*>& GetListeners() const override;

    void AddTrade(const Trade<T> &trade);

};


/** PositionServiceListener listen to TradeBookingService
* Type T is the product type.
*/
template<typename T>
class PositionServiceListener : public ServiceListener<Trade<T>>{
private:
    PositionService<T>* position_service;
    PositionServiceListener();

public:
    static PositionServiceListener* GenerateInstance(){
        static PositionServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(Trade<T> & data) override;

    void ProcessRemove(Trade<T> &data) override;

    void ProcessUpdate(Trade<T> &data) override;

    PositionService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of Position class
template<typename T>
Position<T>::Position() : product(T()){
}

template<typename T>
Position<T>::Position(const T &_product) : product(_product){
}

template<typename T>
const T& Position<T>::GetProduct() const{
    return product;
}

template<typename T>
long Position<T>::GetPosition(string &book){
    return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition(){
    long aggregate_position = 0;
    for (auto position : positions) {
        aggregate_position += position.second;
    }
    return aggregate_position;
}

template<typename T>
void Position<T>::UpdatePosition(const Trade<T> &trade) {
    // If update with different id, do nothing
    if(trade.GetProduct().GetProductId() != product.GetProductId()){
        return;
    }
    // Determine if there is any existing position
    auto pos = positions.find(trade.GetBook());
    long quantity = trade.GetSide() == BUY ? trade.GetQuantity() : -trade.GetQuantity();
    if (pos != positions.end()) {
        pos->second += quantity;
    }
    else {
        positions.insert(make_pair(trade.GetBook(), quantity));
    }
}


//
// Implementation of PositionService class
template<typename T>
PositionService<T>::PositionService(){
}

template<typename T>
Position<T>& PositionService<T>::GetData(string key){
    return position_data[key];
}

template<typename T>
void PositionService<T>::OnMessage(Position<T> &data){
    
}

template<typename T>
void PositionService<T>::AddListener(ServiceListener<Position<T>>* listener){
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Position<T>>*>& PositionService<T>::GetListeners() const{
    return service_listeners;
}

template<typename T>
void PositionService<T>::AddTrade(const Trade<T> &trade){
    const string product_id = trade.GetProduct().GetProductId();
    if (position_data.find(product_id) == position_data.end()) {
        position_data.insert(make_pair(product_id,
                                       Position<T>(trade.GetProduct())));
    }
    position_data[product_id].UpdatePosition(trade);
    for (auto& listener : service_listeners) {
        listener->ProcessAdd(position_data[product_id]);
    }
}


//
// Implementation of PositionServiceListener class
template<typename T>
PositionServiceListener<T>::PositionServiceListener(){
    position_service = PositionService<T>::GenerateInstance();
}

template<typename T>
void PositionServiceListener<T>::ProcessAdd(Trade<T> &data){
    position_service->AddTrade(data);
}

template<typename T>
void PositionServiceListener<T>::ProcessRemove(Trade<T> &data){

}

template<typename T>
void PositionServiceListener<T>::ProcessUpdate(Trade<T> &data){

}

template<typename T>
PositionService<T>* PositionServiceListener<T>::GetService(){
    return position_service;
}


#endif //TRADING_SYSTEM_POSITION_SERVICE_HPP
