/**
 * gui_service.hpp
 * Defines Service for GUI.
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */
#ifndef TRADING_SYSTEM_GUI_SERVICE_HPP
#define TRADING_SYSTEM_GUI_SERVICE_HPP

#include <map>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include "soa.hpp"
#include "products.hpp"
#include "pricing_service.hpp"

using namespace std;


/**
 * GUIServiceConnector publishing into output
 * Type T is the product type.
 */
template<typename T>
class GUIServiceConnector : public Connector<Price<T>>{
private:
    chrono::system_clock::time_point last_time;
    ofstream gui;
    GUIServiceConnector(){
        last_time = chrono::system_clock::now();
        gui.open("../output/gui.txt");
        gui << "Time, CUSIP, Mid, Spread\n";
        gui << fixed << setprecision(6);
    }
    ~GUIServiceConnector(){
        gui.close();
    }

public:
    static GUIServiceConnector<T>* GenerateInstance(){
        static GUIServiceConnector instance;
        return &instance;
    }

    // Override virtual functions in base class Service
    void Publish(Price<T> &data) override{
        auto curr_time = chrono::system_clock::now();
        while(chrono::duration<double, std::milli>(curr_time - last_time).count() < 300){
            curr_time = chrono::system_clock::now();
        }
        time_t now = chrono::system_clock::to_time_t(curr_time);
        gui << put_time(localtime(&now), "%F %T") << " , "
            << data.GetProduct().GetProductId() << " , "
            << data.GetMid() << " , " << data.GetBidOfferSpread() << "\n";
        last_time = curr_time;
    }

    void Subscribe() override{}

};


/**
 * GUI Service to manage throettling of price output
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class GUIService : public Service<string, Price<T>>{
private:
    int count;
    map<string, Price<T> > price_data;
    vector<ServiceListener<Price<T>> *> service_listeners;
    GUIService() : count(0) {}

public:
    static GUIService* GenerateInstance(){
        static GUIService instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    Price<T>& GetData(string key) override {
        return price_data[key];
    }
    
    void OnMessage(Price<T> &data) override {}
    
    void AddListener(ServiceListener<Price<T>>* listener) override {
        service_listeners.push_back(listener);
    }
    
    const vector<ServiceListener<Price<T>> *>& GetListeners() const override {
        return service_listeners;
    }

    void PrintPrice(Price<T> &price){
        auto gui_service_connector = GUIServiceConnector<T>::GenerateInstance();
        string product_id = price.GetProduct().GetProductId();
        price_data.insert(make_pair(product_id, price));
        if(count < 100){
            gui_service_connector->Publish(price);
            count++;
        }
    }

};


/** GUIServiceListener listen to PricingService
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class GUIServiceListener : public ServiceListener<Price<T>>{
private:
    GUIService<T>* gui_service;
    GUIServiceListener(){
        gui_service = GUIService<T>::GenerateInstance();
    }

public:
    static GUIServiceListener* GenerateInstance(){
        static GUIServiceListener instance;
        return &instance;
    }
    
    // Override virtual functions in base class Service
    void ProcessAdd(Price<T> &data) override {
        gui_service->PrintPrice(data);
    }
    
    void ProcessRemove(Price<T> &data) override {}
    
    void ProcessUpdate(Price<T> &data) override {}
    
    const GUIService<T>* GetService(){
        return gui_service;
    }
};

#endif //TRADING_SYSTEM_GUI_SERVICE_HPP
