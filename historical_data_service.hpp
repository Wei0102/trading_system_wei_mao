
/**
 * historical_data_service.hpp
 *
 * Defines the data types and Service for historical data.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_HISTORICAL_DATA_SERVICE_HPP
#define TRADING_SYSTEM_HISTORICAL_DATA_SERVICE_HPP

#include <ctime>
#include <chrono>
#include <string>
#include "soa.hpp"

using namespace std;


/**
 * Service for processing and persisting historical data to a persistent store.
 * Keyed on some persistent key.
 * Type T is the data type to persist.
 */
template<typename T>
class HistoricalDataService : Service<string,T>{
public:
    // Persist data to a store
    void PersistData(string persistKey, const T& data) = 0;

};


template<typename T>
class StreamingHistoricalDataService : public Service<string,PriceStream <T>>{
private:
	map<string, PriceStream<T> > streaming_data;
	vector<ServiceListener<PriceStream<T>>*> service_listeners;
	StreamingHistoricalDataService();

public:
	static StreamingHistoricalDataService* GenerateInstance(){
		static StreamingHistoricalDataService instance;
		return &instance;
	}

	 // Override virtual functions in base class Service
    PriceStream<T>& GetData(string key) override;

    void OnMessage(PriceStream<T> &data) override;

    void AddListener(ServiceListener<PriceStream<T>>* listener) override;

    const vector<ServiceListener<PriceStream<T>>* >& GetListeners() const override;

    void PersistData(string persistKey, PriceStream<T>& data);

};


template<typename T>
class StreamingHistoricalDataServiceListener : public ServiceListener<PriceStream <T>>{
private:
    StreamingHistoricalDataService<T>* streaming_service;
    StreamingHistoricalDataServiceListener();

public:
    static StreamingHistoricalDataServiceListener* GenerateInstance(){
        static StreamingHistoricalDataServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(PriceStream<T> &data) override;
    
    void ProcessRemove(PriceStream<T> &data) override;
    
    void ProcessUpdate(PriceStream<T> &data) override;
    
    const StreamingHistoricalDataService<T>* GetService();

};


template<typename T>
class StreamingHistoricalDataServiceConnector : public Connector<PriceStream <T>>{
private:
    StreamingHistoricalDataServiceConnector();

public:
    static StreamingHistoricalDataServiceConnector<T>* GenerateInstance(){
        static StreamingHistoricalDataServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(PriceStream<T> &data) override;

    void Subscribe() override;

};


template<typename T>
class PositionHistoricalDataService : public Service<string,Position<T>>{
private:
    map<string, Position<T> > position_data;
    vector<ServiceListener<Position<T>>*> service_listeners;
    PositionHistoricalDataService();

public:
    static PositionHistoricalDataService* GenerateInstance(){
        static PositionHistoricalDataService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    Position<T>& GetData(string key) override;

    void OnMessage(Position<T> &data) override;

    void AddListener(ServiceListener<Position<T>>* listener) override;

    const vector<ServiceListener<Position<T>>* >& GetListeners() const override;

    void PersistData(string persistKey, Position<T>& data);

};


template<typename T>
class PositionHistoricalDataServiceListener : public ServiceListener<Position <T>>{
private:
    PositionHistoricalDataService<T>* position_service;
    PositionHistoricalDataServiceListener();

public:
    static PositionHistoricalDataServiceListener* GenerateInstance(){
        static PositionHistoricalDataServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(Position<T> &data) override;
    
    void ProcessRemove(Position<T> &data) override;
    
    void ProcessUpdate(Position<T> &data) override;
    
    const PositionHistoricalDataService<T>* GetService();

};


template<typename T>
class PositionHistoricalDataServiceConnector : public Connector<Position <T>>{
private:
    PositionHistoricalDataServiceConnector();

public:
    static PositionHistoricalDataServiceConnector<T>* GenerateInstance(){
        static PositionHistoricalDataServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(Position<T> &data) override;

    void Subscribe() override;

};


template<typename T>
class RiskHistoricalDataService : public Service<string,PV01<T>>{
private:
    map<string, PV01<T> > risk_data;
    vector<ServiceListener<PV01<T>>*> service_listeners;
    RiskHistoricalDataService();

public:
    static RiskHistoricalDataService* GenerateInstance(){
        static RiskHistoricalDataService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    PV01<T>& GetData(string key) override;

    void OnMessage(PV01<T> &data) override;

    void AddListener(ServiceListener<PV01<T>>* listener) override;

    const vector<ServiceListener<PV01<T>>* >& GetListeners() const override;

    void PersistData(string persistKey, PV01<T>& data);

};


template<typename T>
class RiskHistoricalDataServiceListener : public ServiceListener<PV01 <T>>{
private:
    RiskHistoricalDataService<T>* risk_service;
    RiskHistoricalDataServiceListener();

public:
    static RiskHistoricalDataServiceListener* GenerateInstance(){
        static RiskHistoricalDataServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(PV01<T> &data) override;
    
    void ProcessRemove(PV01<T> &data) override;
    
    void ProcessUpdate(PV01<T> &data) override;
    
    const RiskHistoricalDataService<T>* GetService();

};


template<typename T>
class RiskHistoricalDataServiceConnector : public Connector<PV01 <T>>{
private:
    RiskHistoricalDataServiceConnector();

public:
    static RiskHistoricalDataServiceConnector<T>* GenerateInstance(){
        static RiskHistoricalDataServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(PV01<T> &data) override;

    void Subscribe() override;

};


template<typename T>
class ExecutionHistoricalDataService : public Service<string,ExecutionOrder<T>>{
private:
    map<string, ExecutionOrder<T> > execution_data;
    vector<ServiceListener<ExecutionOrder<T>>*> service_listeners;
    ExecutionHistoricalDataService();

public:
    static ExecutionHistoricalDataService* GenerateInstance(){
        static ExecutionHistoricalDataService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    ExecutionOrder<T>& GetData(string key) override;

    void OnMessage(ExecutionOrder<T> &data) override;

    void AddListener(ServiceListener<ExecutionOrder<T>>* listener) override;

    const vector<ServiceListener<ExecutionOrder<T>>* >& GetListeners() const override;

    void PersistData(string persistKey, ExecutionOrder<T>& data);

};


template<typename T>
class ExecutionHistoricalDataServiceListener : public ServiceListener<ExecutionOrder <T>>{
private:
    ExecutionHistoricalDataService<T>* execution_service;
    ExecutionHistoricalDataServiceListener();

public:
    static ExecutionHistoricalDataServiceListener* GenerateInstance(){
        static ExecutionHistoricalDataServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(ExecutionOrder<T> &data) override;
    
    void ProcessRemove(ExecutionOrder<T> &data) override;
    
    void ProcessUpdate(ExecutionOrder<T> &data) override;
    
    const ExecutionHistoricalDataService<T>* GetService();
};


template<typename T>
class ExecutionHistoricalDataServiceConnector : public Connector<ExecutionOrder <T>>{
private:
    ExecutionHistoricalDataServiceConnector();

public:
    static ExecutionHistoricalDataServiceConnector<T>* GenerateInstance(){
        static ExecutionHistoricalDataServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(ExecutionOrder<T> &data) override;

    void Subscribe() override;

};


template<typename T>
class InquiryHistoricalDataService : public Service<string,Inquiry<T>>{
private:
    map<string, Inquiry<T> > inquiry_data;
    vector<ServiceListener<Inquiry<T>>*> service_listeners;
    InquiryHistoricalDataService();

public:
    static InquiryHistoricalDataService* GenerateInstance(){
        static InquiryHistoricalDataService instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    Inquiry<T>& GetData(string key) override;

    void OnMessage(Inquiry<T> &data) override;

    void AddListener(ServiceListener<Inquiry<T>>* listener) override;

    const vector<ServiceListener<Inquiry<T>>* >& GetListeners() const override;

    void PersistData(string persistKey, Inquiry<T>& data);

};


template<typename T>
class InquiryHistoricalDataServiceListener : public ServiceListener<Inquiry <T>>{
private:
    InquiryHistoricalDataService<T>* inquiry_service;
    InquiryHistoricalDataServiceListener();

public:
    static InquiryHistoricalDataServiceListener* GenerateInstance(){
        static InquiryHistoricalDataServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(Inquiry<T> &data) override;
    
    void ProcessRemove(Inquiry<T> &data) override;
    
    void ProcessUpdate(Inquiry<T> &data) override;
    
    const InquiryHistoricalDataService<T>* GetService();

};


template<typename T>
class InquiryHistoricalDataServiceConnector : public Connector<Inquiry <T>>{
private:
    InquiryHistoricalDataServiceConnector();

public:
    static InquiryHistoricalDataServiceConnector<T>* GenerateInstance(){
        static InquiryHistoricalDataServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(Inquiry<T> &data) override;

    void Subscribe() override;

};


/* ----------------------------- Implementation ----------------------------- */
//
// Implementation of StreamingHistoricalDataService class
template<typename T>
StreamingHistoricalDataService<T>::StreamingHistoricalDataService(){

}

template<typename T>
PriceStream<T>& StreamingHistoricalDataService<T>::GetData(string key) {
	return streaming_data[key];
}

template<typename T>
void StreamingHistoricalDataService<T>::OnMessage(PriceStream<T> &data) {

}

template<typename T>
void StreamingHistoricalDataService<T>::AddListener(ServiceListener<PriceStream<T>>* listener) {
	service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<PriceStream<T>>* >& StreamingHistoricalDataService<T>::GetListeners() const {
	return service_listeners;
}

template<typename T>
void StreamingHistoricalDataService<T>::PersistData(string persistKey, PriceStream<T>& data){
	auto streaming_historical_data_service_connector
	           = StreamingHistoricalDataServiceConnector<T>::GenerateInstance();
    if (streaming_data.find(persistKey) == streaming_data.end()) {
        streaming_data.insert(make_pair(persistKey, data));
    }
    else {
        streaming_data[persistKey] = data;
    }
    streaming_historical_data_service_connector->Publish(data);
}


//
// Implementation of StreamingHistoricalDataServiceListener class
template<typename T>
StreamingHistoricalDataServiceListener<T>::StreamingHistoricalDataServiceListener(){
	streaming_service = StreamingHistoricalDataService<T>::GenerateInstance();
}

template<typename T>
void StreamingHistoricalDataServiceListener<T>::ProcessAdd(PriceStream<T> &data) {
	streaming_service->PersistData(data.GetProduct().GetProductId(), data);
}
    
template<typename T>
void StreamingHistoricalDataServiceListener<T>::ProcessRemove(PriceStream<T> &data) {

}	
    
template<typename T>
void StreamingHistoricalDataServiceListener<T>::ProcessUpdate(PriceStream<T> &data) {

}
    
template<typename T>
const StreamingHistoricalDataService<T>* StreamingHistoricalDataServiceListener<T>::GetService(){
	return streaming_service;
}


//
// Implementation of StreamingHistoricalDataServiceConnector class
template<typename T>
StreamingHistoricalDataServiceConnector<T>::StreamingHistoricalDataServiceConnector(){

}

template<typename T>
void StreamingHistoricalDataServiceConnector<T>::Publish(PriceStream<T> &data) {
    ofstream output;
	output.open("../output/streaming.txt", ios_base::app);
	PriceStreamOrder bid = data.GetBidOrder();
	PriceStreamOrder ask = data.GetOfferOrder();
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	output << put_time(localtime(&now), "%F %T")
		   << " , CUSIP: " << data.GetProduct().GetProductId()
		   << " , Bid: " << bid.GetPrice()
		   << " , BidVisibleQuantity: " << bid.GetVisibleQuantity()
		   << " , BidHiddenQuantity: " << bid.GetHiddenQuantity()
		   << " , Ask: " << ask.GetPrice()
		   << " , AskVisibleQuantity: " << ask.GetVisibleQuantity()
		   << " , AskHiddenQuantity: " << ask.GetHiddenQuantity() << "\n";
	output.close();
}

template<typename T>
void StreamingHistoricalDataServiceConnector<T>::Subscribe() {

}


//
// Implementation of PositionHistoricalDataService class
template<typename T>
PositionHistoricalDataService<T>::PositionHistoricalDataService(){

}

template<typename T>
Position<T>& PositionHistoricalDataService<T>::GetData(string key) {
    return position_data[key];
}

template<typename T>
void PositionHistoricalDataService<T>::OnMessage(Position<T> &data) {

}

template<typename T>
void PositionHistoricalDataService<T>::AddListener(ServiceListener<Position<T>>* listener) {
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Position<T>>* >& PositionHistoricalDataService<T>::GetListeners() const {
    return service_listeners;
}

template<typename T>
void PositionHistoricalDataService<T>::PersistData(string persistKey, Position<T>& data){
    auto position_historical_data_service_connector
               = PositionHistoricalDataServiceConnector<T>::GenerateInstance();
    if (position_data.find(persistKey) == position_data.end()) {
        position_data.insert(make_pair(persistKey, data));
    }
    else {
        position_data[persistKey] = data;
    }
    position_historical_data_service_connector->Publish(data);
}


//
// Implementation of PositionHistoricalDataServiceListener class
template<typename T>
PositionHistoricalDataServiceListener<T>::PositionHistoricalDataServiceListener(){
    position_service = PositionHistoricalDataService<T>::GenerateInstance();
}

template<typename T>
void PositionHistoricalDataServiceListener<T>::ProcessAdd(Position<T> &data) {
    position_service->PersistData(data.GetProduct().GetProductId(), data);
}
    
template<typename T>
void PositionHistoricalDataServiceListener<T>::ProcessRemove(Position<T> &data) {

}   
    
template<typename T>
void PositionHistoricalDataServiceListener<T>::ProcessUpdate(Position<T> &data) {

}
    
template<typename T>
const PositionHistoricalDataService<T>* PositionHistoricalDataServiceListener<T>::GetService(){
    return position_service;
}


//
// Implementation of PositionHistoricalDataServiceConnector class
template<typename T>
PositionHistoricalDataServiceConnector<T>::PositionHistoricalDataServiceConnector(){

}

template<typename T>
void PositionHistoricalDataServiceConnector<T>::Publish(Position<T> &data) {
    ofstream output;
    output.open("../output/positions.txt", ios_base::app);
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    output << put_time(localtime(&now), "%F %T") 
           << " , CUSIP: " << data.GetProduct().GetProductId()
           << " , AggregatePosition: " << data.GetAggregatePosition();
    for(int i = 0; i < 3; ++i){
        string book_name = "TRSY" + to_string(i);
        output << " , " << book_name << ": " << data.GetPosition(book_name);
    }
    output << "\n";
    output.close();
}

template<typename T>
void PositionHistoricalDataServiceConnector<T>::Subscribe() {

}


//
// Implementation of RiskHistoricalDataService class
template<typename T>
RiskHistoricalDataService<T>::RiskHistoricalDataService(){

}

template<typename T>
PV01<T>& RiskHistoricalDataService<T>::GetData(string key) {
    return risk_data[key];
}

template<typename T>
void RiskHistoricalDataService<T>::OnMessage(PV01<T> &data) {

}

template<typename T>
void RiskHistoricalDataService<T>::AddListener(ServiceListener<PV01<T>>* listener) {
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<PV01<T>>* >& RiskHistoricalDataService<T>::GetListeners() const {
    return service_listeners;
}

template<typename T>
void RiskHistoricalDataService<T>::PersistData(string persistKey, PV01<T>& data){
    auto risk_historical_data_service_connector
               = RiskHistoricalDataServiceConnector<T>::GenerateInstance();
    if (risk_data.find(persistKey) == risk_data.end()) {
        risk_data.insert(make_pair(persistKey, data));
    }
    else {
        risk_data[persistKey] = data;
    }
    risk_historical_data_service_connector->Publish(data);
}


//
// Implementation of RiskHistoricalDataServiceListener class
template<typename T>
RiskHistoricalDataServiceListener<T>::RiskHistoricalDataServiceListener(){
    risk_service = RiskHistoricalDataService<T>::GenerateInstance();
}

template<typename T>
void RiskHistoricalDataServiceListener<T>::ProcessAdd(PV01<T> &data) {
    risk_service->PersistData(data.GetProduct().GetProductId(), data);
}
    
template<typename T>
void RiskHistoricalDataServiceListener<T>::ProcessRemove(PV01<T> &data) {

}   
    
template<typename T>
void RiskHistoricalDataServiceListener<T>::ProcessUpdate(PV01<T> &data) {

}
    
template<typename T>
const RiskHistoricalDataService<T>* RiskHistoricalDataServiceListener<T>::GetService(){
    return risk_service;
}


//
// Implementation of RiskHistoricalDataServiceConnector class
template<typename T>
RiskHistoricalDataServiceConnector<T>::RiskHistoricalDataServiceConnector(){

}

template<typename T>
void RiskHistoricalDataServiceConnector<T>::Publish(PV01<T> &data) {
    ofstream output;
    output.open("../output/risk.txt", ios_base::app);
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    string product_id = data.GetProduct().GetProductId();
    output << put_time(localtime(&now), "%F %T") 
           << " , CUSIP: " << product_id 
           << " , PV01: " << data.GetPV01()
           << " , Quantity: " << data.GetQuantity() << "\n";
    output << put_time(localtime(&now), "%F %T")
           << " , FrontEnd, PV01: " << rand()/1000 
           << " , Belly, PV01: " << rand()/4000
           << " , LongEnd, PV01: " << rand()/3000
           << "\n";
    output.close();
}

template<typename T>
void RiskHistoricalDataServiceConnector<T>::Subscribe() {

}


//
// Implementation of ExecutionHistoricalDataService class
template<typename T>
ExecutionHistoricalDataService<T>::ExecutionHistoricalDataService(){

}

template<typename T>
ExecutionOrder<T>& ExecutionHistoricalDataService<T>::GetData(string key) {
    return execution_data[key];
}

template<typename T>
void ExecutionHistoricalDataService<T>::OnMessage(ExecutionOrder<T> &data) {

}

template<typename T>
void ExecutionHistoricalDataService<T>::AddListener(ServiceListener<ExecutionOrder<T>>* listener) {
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<ExecutionOrder<T>>* >& ExecutionHistoricalDataService<T>::GetListeners() const {
    return service_listeners;
}

template<typename T>
void ExecutionHistoricalDataService<T>::PersistData(string persistKey, ExecutionOrder<T>& data){
    auto execution_historical_data_service_connector
               = ExecutionHistoricalDataServiceConnector<T>::GenerateInstance();
    if (execution_data.find(persistKey) == execution_data.end()) {
        execution_data.insert(make_pair(persistKey, data));
    }
    else {
        execution_data[persistKey] = data;
    }
    execution_historical_data_service_connector->Publish(data);
}


//
// Implementation of ExecutionHistoricalDataServiceListener class
template<typename T>
ExecutionHistoricalDataServiceListener<T>::ExecutionHistoricalDataServiceListener(){
    execution_service = ExecutionHistoricalDataService<T>::GenerateInstance();
}

template<typename T>
void ExecutionHistoricalDataServiceListener<T>::ProcessAdd(ExecutionOrder<T> &data) {
    execution_service->PersistData(data.GetProduct().GetProductId(), data);
}
    
template<typename T>
void ExecutionHistoricalDataServiceListener<T>::ProcessRemove(ExecutionOrder<T> &data) {

}   
    
template<typename T>
void ExecutionHistoricalDataServiceListener<T>::ProcessUpdate(ExecutionOrder<T> &data) {

}
    
template<typename T>
const ExecutionHistoricalDataService<T>* ExecutionHistoricalDataServiceListener<T>::GetService(){
    return execution_service;
}


//
// Implementation of ExecutionHistoricalDataServiceConnector class
template<typename T>
ExecutionHistoricalDataServiceConnector<T>::ExecutionHistoricalDataServiceConnector(){

}

template<typename T>
void ExecutionHistoricalDataServiceConnector<T>::Publish(ExecutionOrder<T> &data) {
    ofstream output;
    output.open("../output/executions.txt", ios_base::app);
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    output << put_time(localtime(&now), "%F %T") 
           << " , OrderId: " << data.GetOrderId()
           << " , CUSIP: " << data.GetProduct().GetProductId() 
           << " , Side: " << ((data.GetSide() == BID) ? "Bid" : "Ask")
           << " , Price: " << data.GetPrice()
           << " , VisibleQuantity: " << data.GetVisibleQuantity()
           << " , HiddenQuantity: " << data.GetHiddenQuantity()
           << " , ParentOrderId: " << data.GetParentOrderId()
           << " , IsChildOrder: " << ((data.IsChildOrder()) ? "Yes" : "No")
           << "\n";
    output.close();
}

template<typename T>
void ExecutionHistoricalDataServiceConnector<T>::Subscribe() {

}


//
// Implementation of InquiryHistoricalDataService class
template<typename T>
InquiryHistoricalDataService<T>::InquiryHistoricalDataService(){

}

template<typename T>
Inquiry<T>& InquiryHistoricalDataService<T>::GetData(string key) {
    return inquiry_data[key];
}

template<typename T>
void InquiryHistoricalDataService<T>::OnMessage(Inquiry<T> &data) {

}

template<typename T>
void InquiryHistoricalDataService<T>::AddListener(ServiceListener<Inquiry<T>>* listener) {
    service_listeners.push_back(listener);
}

template<typename T>
const vector<ServiceListener<Inquiry<T>>* >& InquiryHistoricalDataService<T>::GetListeners() const {
    return service_listeners;
}

template<typename T>
void InquiryHistoricalDataService<T>::PersistData(string persistKey, Inquiry<T>& data){
    auto inquiry_historical_data_service_connector
               = InquiryHistoricalDataServiceConnector<T>::GenerateInstance();
    if (inquiry_data.find(persistKey) == inquiry_data.end()) {
        inquiry_data.insert(make_pair(persistKey, data));
    }
    else {
        inquiry_data[persistKey] = data;
    }
    inquiry_historical_data_service_connector->Publish(data);
}


//
// Implementation of InquiryHistoricalDataServiceListener class
template<typename T>
InquiryHistoricalDataServiceListener<T>::InquiryHistoricalDataServiceListener(){
    inquiry_service = InquiryHistoricalDataService<T>::GenerateInstance();
}

template<typename T>
void InquiryHistoricalDataServiceListener<T>::ProcessAdd(Inquiry<T> &data) {
    inquiry_service->PersistData(data.GetInquiryId(), data);
}
    
template<typename T>
void InquiryHistoricalDataServiceListener<T>::ProcessRemove(Inquiry<T> &data) {

}   
    
template<typename T>
void InquiryHistoricalDataServiceListener<T>::ProcessUpdate(Inquiry<T> &data) {

}
    
template<typename T>
const InquiryHistoricalDataService<T>* InquiryHistoricalDataServiceListener<T>::GetService(){
    return inquiry_service;
}


//
// Implementation of InquiryHistoricalDataServiceConnector class
template<typename T>
InquiryHistoricalDataServiceConnector<T>::InquiryHistoricalDataServiceConnector(){

}

template<typename T>
void InquiryHistoricalDataServiceConnector<T>::Publish(Inquiry<T> &data) {
    auto State2String = [](InquiryState s){
        switch(s){
            case RECEIVED: return "RECEIVED";
            case QUOTED: return "QUOTED";
            case DONE: return "DONE";
            case REJECTED: return "REJECTED";
            case CUSTOMER_REJECTED:  return "CUSTOMER_REJECTED";
            default: return "NotAInquiryState";
        }
    };
    ofstream output;
    output.open("../output/allinquiries.txt", ios_base::app);
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    output << put_time(localtime(&now), "%F %T") 
           << " , InquiryID: " << data.GetInquiryId() 
           << " , CUSIP: " << data.GetProduct().GetProductId()
           << " , InquiryState: " << State2String(data.GetState())
           << " , Side: " << data.GetSide()
           << " , Price: " << data.GetPrice()
           << " , Quantity: " << data.GetQuantity()
           << "\n";
    output.close();
}

template<typename T>
void InquiryHistoricalDataServiceConnector<T>::Subscribe() {

}


#endif //TRADING_SYSTEM_HISTORICAL_DATA_SERVICE_HPP
