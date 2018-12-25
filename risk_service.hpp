/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */
#ifndef TRADING_SYSTEM_RISK_SERVICE_HPP
#define TRADING_SYSTEM_RISK_SERVICE_HPP

#include "soa.hpp"
#include "position_service.hpp"

/**
 * PV01 risk.
 * Assume pv01 updates half as quantity for simplicity
 * Type T is the product type.
 */
template<typename T>
class PV01{
private:
    T product;
    double pv01;
    long quantity;

public:
    // ctors
    PV01();
    PV01(const T &_product, double _pv01, long _quantity);

    // getters
    const T& GetProduct() const;
    double GetPV01() const;
    long GetQuantity() const;

    // modifiers
    void UpdatePV01(double new_pv01);
    void UpdateQuantity(long new_quantity);

};


/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector{
private:
    vector<T> products;
    string name;

public:
    // ctors
    BucketedSector(const vector<T> &_products, string _name);

    // getters
    const vector<T>& GetProducts() const;
    const string& GetName() const;

};


/**
 * Risk Service to vend out risk for a particular security and across a risk
 * bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string,PV01 <T> >{
private:
    map<string, PV01<T>> pv01_data;
    vector<ServiceListener<PV01<T>> *> service_listeners;
    RiskService();

public:
    static RiskService* GenerateInstance(){
        static RiskService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    PV01<T>& GetData(string key) override;

    void OnMessage(PV01<T> &data) override;

    void AddListener(ServiceListener<PV01<T>>* listener) override;

    const vector<ServiceListener<PV01<T>>*>& GetListeners() const override;

    // Add a position that the service will risk
    void AddPosition(Position<T> &position);

    // Get the bucketed risk for the bucket sector
    const PV01<BucketedSector<T>>& GetBucketedRisk(const BucketedSector<T> &sector) const;

};


/** RiskServiceListener listen to PositionService
* Type T is the product type.
*/
template<typename T>
class RiskServiceListener : public ServiceListener<Position<T>>{
private:
    RiskService<T>* risk_service;
    RiskServiceListener();

public:
    static RiskServiceListener<T>* GenerateInstance(){
        static RiskServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(Position<T> & data) override;

    void ProcessRemove(Position<T> &data) override;

    void ProcessUpdate(Position<T> &data) override;

    RiskService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of PV01 class
template<typename T>
PV01<T>::PV01():product(T()){
    pv01 = 0.0;
    quantity = 0;
}

template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity):product(_product){
    pv01 = _pv01;
    quantity = _quantity;
}

template<typename T>
const T& PV01<T>::GetProduct() const{
    return product;
}

template<typename T>
double PV01<T>::GetPV01() const{
    return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const{
    return quantity;
}

template<typename T>
void PV01<T>::UpdatePV01(double new_pv01) {
    pv01 += new_pv01;
}


template<typename T>
void PV01<T>::UpdateQuantity(long new_quantity) {
    quantity += new_quantity;
}


//
// Implementation of BucketedSector class
template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
products(_products){
    name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const{
    return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const{
    return name;
}


//
// Implementation of RiskService class
template<typename T>
RiskService<T>::RiskService(){

}

template<typename T>
PV01<T>& RiskService<T>::GetData(string key){
    return pv01_data[key];
}

template<typename T>
void RiskService<T>::OnMessage(PV01<T> &data){

}

template<typename T>
void RiskService<T>::AddListener(ServiceListener<PV01<T>>* listener){
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<PV01<T>>*>& RiskService<T>::GetListeners() const {
    return service_listeners;
}

template<typename T>
void RiskService<T>::AddPosition(Position<T> &position){
    const string product_id = position.GetProduct().GetProductId();
    if (pv01_data.find(product_id) == pv01_data.end()){
        pv01_data.insert(make_pair(product_id, PV01<T>(
                         position.GetProduct(), 0, 0)));
    }
    pv01_data[product_id].UpdatePV01(0.000001 * position.GetAggregatePosition());
    pv01_data[product_id].UpdateQuantity(position.GetAggregatePosition());
    for (auto& listener : service_listeners) {
        listener->ProcessAdd(pv01_data[product_id]);
    }
}

template<typename T>
const PV01< BucketedSector<T> >& RiskService<T>::GetBucketedRisk(
        const BucketedSector<T> &sector) const{
    double bucketed_pv01 = 0.0;
    long bucketed_position = 0;
    PV01< BucketedSector<T> > bucketed_sector_pv01(sector,
            bucketed_pv01, bucketed_position);
    for(auto it : sector.GetProducts()){
        auto pos = pv01_data.find(it->GetProductId());
        if (pos != pv01_data.end()){
            bucketed_sector_pv01.UpdatePV01(pos->second.GetPV01());
            bucketed_sector_pv01.UpdateQuantity(pos->second.GetQuantity());
        }
    }
    return bucketed_sector_pv01;
}


//
// Implementation of RiskServiceListener class
template<typename T>
RiskServiceListener<T>::RiskServiceListener(){
    risk_service = RiskService<T>::GenerateInstance();
}

template<typename T>
void RiskServiceListener<T>::ProcessAdd(Position<T> & data){
    risk_service->AddPosition(data);
}

template<typename T>
void RiskServiceListener<T>::ProcessRemove(Position<T> &data){

}

template<typename T>
void RiskServiceListener<T>::ProcessUpdate(Position<T> &data){

}

template<typename T>
RiskService<T>* RiskServiceListener<T>::GetService(){
    return risk_service;
}

#endif //TRADING_SYSTEM_RISK_SERVICE_HPP
