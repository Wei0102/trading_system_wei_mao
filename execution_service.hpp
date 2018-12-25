/**
 * execution_service.hpp
 * Defines the data types and Service for executions.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_EXECUTION_SERVICE_HPP
#define TRADING_SYSTEM_EXECUTION_SERVICE_HPP


#include <string>
#include "soa.hpp"
#include "market_data_service.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };


/**
 * An execution order that can be placed on an exchange.
 * Type T is the product type.
 */
template<typename T>
class ExecutionOrder
{
private:
    T product;
    PricingSide side;
    string orderId;
    OrderType orderType;
    double price;
    double visibleQuantity;
    double hiddenQuantity;
    string parentOrderId;
    bool isChildOrder;

public:

    // ctor for an order
    ExecutionOrder();
    ExecutionOrder(const T &_product, PricingSide _side, string _orderId,
            OrderType _orderType, double _price, double _visibleQuantity,
            double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

    // Get the product
    const T& GetProduct() const;

    const PricingSide& GetSide() const;

    // Get the order ID
    const string& GetOrderId() const;

    // Get the order type on this order
    OrderType GetOrderType() const;

    // Get the price on this order
    double GetPrice() const;

    // Get the visible quantity on this order
    long GetVisibleQuantity() const;

    // Get the hidden quantity
    long GetHiddenQuantity() const;

    // Get the parent order ID
    const string& GetParentOrderId() const;

    // Is child order?
    bool IsChildOrder() const;

};


/* *
 * a class with a reference to an ExecutionOrder object.
 *
 */
template <typename T>
class AlgoExecution {
private:
    ExecutionOrder<T> execution_order;

public:
    // ctors
    AlgoExecution();
    AlgoExecution(const ExecutionOrder<T> &execution_order);
    
    // getters
    const ExecutionOrder<T>& GetExecutionOrder() const;

};


/**
 * Service for executing orders on an exchange.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class ExecutionService : public Service<string,ExecutionOrder <T> >
{
private:
    map<string, ExecutionOrder<T>> execution_data;
    vector<ServiceListener<ExecutionOrder<T>> *> service_listeners;
    ExecutionService();

public:
    static ExecutionService* GenerateInstance(){
        static ExecutionService instance;
        return &instance;
    }
 
    // Override virtual functions in base class Service
    ExecutionOrder<T>& GetData(string key) override;

    void OnMessage(ExecutionOrder<T> &data) override;

    void AddListener(ServiceListener<ExecutionOrder<T>>* listener) override;

    const vector<ServiceListener<ExecutionOrder<T>>*>& GetListeners() const override;

    // Execute an order on a market
    void ExecuteOrder(const ExecutionOrder<T>& order, Market market);

};


template <typename T>
class ExecutionServiceListener : public ServiceListener<AlgoExecution<T> > {
private:
    ExecutionService<T>* execution_service;
    ExecutionServiceListener();

public:
    static ExecutionServiceListener* GenerateInstance(){
        static ExecutionServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(AlgoExecution<T> & data) override;

    void ProcessRemove(AlgoExecution<T> &data) override;

    void ProcessUpdate(AlgoExecution<T> &data) override;

    ExecutionService<T>* GetService();

};


/**
 * Keyed on product identifier with value an AlgoExecution object.
 * Register a ServiceListener on the BondMarketDataService and aggress 
 * the top of the book
 *
 */
template <typename T>
class AlgoExecutionService : Service<string, AlgoExecution<T>> {
private:
    map<string, AlgoExecution<T>> algo_execution_data;
    vector<ServiceListener<AlgoExecution<T>> *> service_listeners;
    AlgoExecutionService();

public:
    static AlgoExecutionService* GenerateInstance(){
        static AlgoExecutionService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    AlgoExecution<T>& GetData(string key) override;

    void OnMessage(AlgoExecution<T> &data) override;

    void AddListener(ServiceListener<AlgoExecution<T>>* listener) override;

    const vector<ServiceListener<AlgoExecution<T>>*>& GetListeners() const override;

    // Execute on the entire size on the market data for the right side
    void ExecuteAlgo(const OrderBook<T> &order_book);
};


/** 
* AlgoExecutionServiceListener listen to TradeBookingService
* Type T is the product type.
*/
template <typename T>
class AlgoExecutionServiceListener : public ServiceListener<OrderBook<T> > {
private:
    AlgoExecutionService<T>* algo_execution_service;
    AlgoExecutionServiceListener();

public:
    static AlgoExecutionServiceListener* GenerateInstance(){
        static AlgoExecutionServiceListener instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void ProcessAdd(OrderBook<T> & data) override;

    void ProcessRemove(OrderBook<T> &data) override;

    void ProcessUpdate(OrderBook<T> &data) override;

    AlgoExecutionService<T>* GetService();

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of ExecutionOrder class
template<typename T>
ExecutionOrder<T>::ExecutionOrder() : product(T())
{
    side = OFFER;
    orderId = "0";
    orderType = FOK;
    price = 0;
    visibleQuantity = 0;
    hiddenQuantity = 0;
    parentOrderId = "0";
    isChildOrder = false;
}

template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side,
        string _orderId, OrderType _orderType, double _price,
        double _visibleQuantity, double _hiddenQuantity, string _parentOrderId,
        bool _isChildOrder) : product(_product)
{
    side = _side;
    orderId = _orderId;
    orderType = _orderType;
    price = _price;
    visibleQuantity = _visibleQuantity;
    hiddenQuantity = _hiddenQuantity;
    parentOrderId = _parentOrderId;
    isChildOrder = _isChildOrder;
}

template<typename T>
const PricingSide& ExecutionOrder<T>::GetSide() const {
    return side;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
    return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
    return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
    return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
    return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
    return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
    return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
    return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
    return isChildOrder;
}


//
// Implementation of ExecutionOrder class
template<typename T>
AlgoExecution<T>::AlgoExecution() : execution_order(ExecutionOrder<T>()) {

}

template<typename T>
AlgoExecution<T>::AlgoExecution(const ExecutionOrder<T> &execution_order) : execution_order(
        execution_order) {

}

template<typename T>
const ExecutionOrder<T>& AlgoExecution<T>::GetExecutionOrder() const{
    return execution_order;
}


//
// Implementation of ExecutionService class
template <typename T>
ExecutionService<T>::ExecutionService(){

}

template <typename T>
ExecutionOrder<T>& ExecutionService<T>::GetData(string key) {
    return execution_data[key];
}

template <typename T>
void ExecutionService<T>::OnMessage(ExecutionOrder<T> &data) {

}

template <typename T>
void ExecutionService<T>::AddListener(ServiceListener<ExecutionOrder<T>>* listener) {
    service_listeners.push_back(listener);
}

template <typename T>
const vector<ServiceListener<ExecutionOrder<T>>*>& ExecutionService<T>::GetListeners() const {
    return service_listeners;
}

template <typename T>
void ExecutionService<T>::ExecuteOrder(const ExecutionOrder<T>& order, Market market){
    const string product_id = order.GetProduct().GetProductId();
    if (execution_data.find(product_id) == execution_data.end()) {
        execution_data.insert(make_pair(product_id, order));
    }
    else {
        execution_data[product_id] = order;
    }
    for (auto& listener : service_listeners) {
        listener->ProcessAdd(execution_data[product_id]);
    }
}


//
// Implementation of ExecutionServiceListener class
template <typename T>
ExecutionServiceListener<T>::ExecutionServiceListener(){
    execution_service = ExecutionService<T>::GenerateInstance();
}

template <typename T>
void ExecutionServiceListener<T>::ProcessAdd(AlgoExecution<T> & data) {
    execution_service->ExecuteOrder(data.GetExecutionOrder(), BROKERTEC);
}

template <typename T>
void ExecutionServiceListener<T>::ProcessRemove(AlgoExecution<T> &data) {

}

template <typename T>
void ExecutionServiceListener<T>::ProcessUpdate(AlgoExecution<T> &data) {

}

template <typename T>
ExecutionService<T>* ExecutionServiceListener<T>::GetService(){
    return execution_service;
}


//
// Implementation of AlgoExecutionService class
template <typename T>
AlgoExecutionService<T>::AlgoExecutionService()
{
}

template <typename T>
AlgoExecution<T>& AlgoExecutionService<T>::GetData(string key)
{
    return algo_execution_data[key];
}

template <typename T>
void AlgoExecutionService<T>::OnMessage(AlgoExecution<T> &data)
{
}

template <typename T>
void AlgoExecutionService<T>::AddListener(ServiceListener<AlgoExecution<T>>* listener)
{
    service_listeners.push_back(listener);
}

template <typename T>
void AlgoExecutionService<T>::ExecuteAlgo(const OrderBook<T> &order_book)
{
    static int order_count = 0;
    T product = order_book.GetProduct();
    string product_id = product.GetProductId();
    double bid = order_book.GetBidStack()[0].GetPrice();
    double ask = order_book.GetOfferStack()[0].GetPrice();
    double spread = ask - bid;
    if (spread <= 2.0/128.0){
        order_count++;
        PricingSide order_side = order_count%2 == 1 ? BID : OFFER;
        string parent_order_id = to_string(order_count) + "-" +to_string(rand()%10);
        string order_id = parent_order_id + to_string(rand()%1000000);
        OrderType order_type = MARKET;
        double price = (order_side == OFFER) ? bid : ask;
        double visible_quantity =  1000000;
        double hidden_quantity = 1000000;
        bool is_child_order = false;
        AlgoExecution<T> algo_execution(ExecutionOrder<T>(product,
                order_side, order_id, order_type, price, visible_quantity,
                hidden_quantity, parent_order_id, is_child_order));
        for (auto listener : service_listeners) {
            listener->ProcessAdd(algo_execution);
        }
    }
}


//
// Implementation of AlgoExecutionService class
template <typename T>
const vector<ServiceListener<AlgoExecution<T>>*>&
AlgoExecutionService<T>::GetListeners() const
{
    return service_listeners;
}


template<typename T>
AlgoExecutionServiceListener<T>::AlgoExecutionServiceListener(){
    algo_execution_service = AlgoExecutionService<T>::GenerateInstance();
}

template<typename T>
void AlgoExecutionServiceListener<T>::ProcessAdd(OrderBook<T> & data){
    algo_execution_service->ExecuteAlgo(data);
}

template<typename T>
void AlgoExecutionServiceListener<T>::ProcessRemove(OrderBook<T> &data){

}

template<typename T>
void AlgoExecutionServiceListener<T>::ProcessUpdate(OrderBook<T> &data){

}

template<typename T>
AlgoExecutionService<T>* AlgoExecutionServiceListener<T>::GetService(){
    return algo_execution_service;
}

#endif //TRADING_SYSTEM_EXECUTION_SERVICE_HPP
