
#include "data_generator.hpp"
#include "soa.hpp"
#include "products.hpp"

#include "pricing_service.hpp"
#include "streaming_service.hpp"
#include "gui_service.hpp"

#include "trade_booking_service.hpp"
#include "position_service.hpp"
#include "risk_service.hpp"

#include "market_data_service.hpp"
#include "execution_service.hpp"

#include "inquiry_service.hpp"

#include "historical_data_service.hpp"


using namespace std;

int main() {

    // generate data
    DataGenerator test;
    // Should be 1,000,000 prices for each bond
    test.GeneratePricesInput(1000000);
    // Should be 10 trades for each bond
    test.GenerateTradesInput(10);
    // Should be 1,000,000 order book updates for each bond
    test.GenerateMarketDataInput(1000000);
    // Should be 10 inquiries for each bond
    test.GenerateInquiriesInput(10);

    // price service
    auto pricing_service_connector =
            PricingServiceConnector<Bond>::GenerateInstance();
    auto pricing_service = pricing_service_connector->GetService();

    auto algo_streaming_service_listener =
            AlgoStreamingServiceListener<Bond>::GenerateInstance();
    pricing_service->AddListener(algo_streaming_service_listener);
    auto algo_streaming_service = algo_streaming_service_listener->GetService();

    auto streaming_service_listener =
            StreamingServiceListener<Bond>::GenerateInstance();
    algo_streaming_service->AddListener(streaming_service_listener);
    auto streaming_service = streaming_service_listener->GetService();
    auto streaming_historical_data_service_listener =
            StreamingHistoricalDataServiceListener<Bond>::GenerateInstance();
    streaming_service->AddListener(streaming_historical_data_service_listener);

   auto gui_service_listener = GUIServiceListener<Bond>::GenerateInstance();
   pricing_service->AddListener(gui_service_listener);

    // trade service
    auto trade_booking_service_connector =
            TradeBookingServiceConnector<Bond>::GenerateInstance();
    auto trade_booking_service = trade_booking_service_connector->GetService();

    auto position_service_listener =
            PositionServiceListener<Bond>::GenerateInstance();
    trade_booking_service->AddListener(position_service_listener);
    auto position_service = position_service_listener->GetService();
    auto position_historical_data_service_listener =
            PositionHistoricalDataServiceListener<Bond>::GenerateInstance();
    position_service->AddListener(position_historical_data_service_listener);

    auto risk_service_listener = RiskServiceListener<Bond>::GenerateInstance();
    position_service->AddListener(risk_service_listener);
    auto risk_service = risk_service_listener->GetService();
    auto risk_historical_data_service_listener =
            RiskHistoricalDataServiceListener<Bond>::GenerateInstance();
    risk_service->AddListener(risk_historical_data_service_listener);

    // market data service
    auto market_data_service_connector =
            MarketDataServiceConnector<Bond>::GenerateInstance();
    auto market_data_service = market_data_service_connector->GetService();

    auto algo_execution_service_listener =
            AlgoExecutionServiceListener<Bond>::GenerateInstance();
    market_data_service->AddListener(algo_execution_service_listener);
    auto algo_execution_service = algo_execution_service_listener->GetService();

    auto execution_service_listener = ExecutionServiceListener<Bond>::GenerateInstance();
    algo_execution_service->AddListener(execution_service_listener);
    auto execution_service = execution_service_listener->GetService();
    auto execution_historical_data_service_listener =
            ExecutionHistoricalDataServiceListener<Bond>::GenerateInstance();
    execution_service->AddListener(execution_historical_data_service_listener);

    auto trade_booking_service_listener = TradeBookingServiceListener<Bond>::GenerateInstance();
    execution_service->AddListener(trade_booking_service_listener);

    // inquiry service
    auto inquiry_service_connector = InquiryServiceConnector<Bond>::GenerateInstance();
    auto inquiry_service = inquiry_service_connector->GetService();
    auto inquiry_historical_data_service_listener =
            InquiryHistoricalDataServiceListener<Bond>::GenerateInstance();
    inquiry_service->AddListener(inquiry_historical_data_service_listener);


    // subscribe, start flow data into the system
    pricing_service_connector->Subscribe();
    trade_booking_service_connector->Subscribe();
    market_data_service_connector->Subscribe();
    inquiry_service_connector->Subscribe();


    return 0;
}