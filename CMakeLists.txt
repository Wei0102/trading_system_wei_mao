cmake_minimum_required(VERSION 3.13)
project(trading_system)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.65.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(/usr/local/include)

add_executable(trading_system
        main.cpp
        data_generator.hpp
        soa.hpp
        products.hpp
        pricing_service.hpp
        streaming_service.hpp
        gui_service.hpp
        trade_booking_service.hpp
        position_service.hpp
        risk_service.hpp
        market_data_service.hpp
        execution_service.hpp
        algo_execution_service.hpp
        inquiry_service.hpp
        historical_data_service.hpp
        )

target_link_libraries(trading_system ${Boost_LIBRARIES})