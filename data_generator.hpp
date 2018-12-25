/**
 * data_generator.hpp
 * Definition of data generator of input data
 *
 * @author Wei Mao
 * Decemeber 18th, 2018
 */

#ifndef TRADING_SYSTEM_DATA_GENERATOR_HPP
#define TRADING_SYSTEM_DATA_GENERATOR_HPP

#include <vector>
#include <string>
#include <fstream>
using namespace std;

class DataGenerator{
private:
    vector<string> cusip_codes;
    vector<string> trade_books;
    vector<string> trade_sides;
public:
    // Constructor
    DataGenerator(){
        cusip_codes = {"9128285Q9", "9128285R7", "9128285P1",
                       "9128285N6", "9128285M8", "912810SE9"};
        trade_books = {"TRSY1", "TRSY2", "TRSY3"};
        trade_sides = {"BUY", "SELL"};
    };
    // Destructor
    ~DataGenerator(){};
    // Member functions: generate inputs
    string GeneratePrice(int price);
    void GeneratePricesInput(int count);
    void GenerateTradesInput(int count);
    void GenerateMarketDataInput(int count);
    void GenerateInquiriesInput(int count);
};

string DataGenerator::GeneratePrice(int price){
    // price oscillate from 0-511 ticks
    string price_level = to_string(99 + price / 256);
    price = price % 256;
    string xy = (price/8<10?"0"+to_string(price/8):to_string(price/8));
    string z = (price % 8 == 4 ? "+" : to_string(price % 8));
    string price_string = price_level + "-" + xy + z;
    return price_string;
}

void DataGenerator::GeneratePricesInput(int count) {
    ofstream prices;
    prices.open("../input/prices.txt");
    prices << "CUSIP, Mid, Spread\n";
    for(int i = 0; i < count; ++i){
        for(int j = 0; j < 6; ++j){
            prices << cusip_codes[j] << "," << GeneratePrice(rand() % 510)
                   << ",0-00" + to_string(2 + rand() % 3) + '\n';
        }
    }
    prices.close();
}

void DataGenerator::GenerateTradesInput(int count){
    ofstream trades;
    trades.open("../input/trades.txt");
    trades << "CUSIP, Trade ID, Price, Quantity, Book, Side\n";
    for(int i = 0; i < count; ++i){
        for(int j = 0; j < 6; ++j){
            int trade_id = i * 6 + j + 1;
            string price = GeneratePrice(rand() % 512);
            int quantity = (1 + rand() % 5) * 1000000;
            trades << cusip_codes[j] << "," << trade_id << "," << price
                   << "," << quantity << "," << trade_books[rand()%3]
                   << "," << trade_sides[rand() % 2] << "\n";
        }
    }
    trades.close();
}

void DataGenerator::GenerateMarketDataInput(int count){
    ofstream market;
    market.open("../input/marketdata.txt");
    market << "CUSIP, Bid1, QB1, Ask1, QA1, "
           << "Bid2, QB2, Ask2, QA2, Bid3, QB3, Ask3, QA3, "
           << "Bid4, QB4, Ask4, QA4, Bid5, QB5, Ask5, QA5\n";
    vector<int> quantity_list = {1000000,2000000,3000000,4000000,5000000};
    function<string(int, int)> GenerateBookLine = [&](int top_bid, int top_ask){
        string OrderBookLine = "";
        for(int i = 0; i < 5; ++i){
            string bid = GeneratePrice(top_bid - i);
            string ask = GeneratePrice(top_ask + i);
            string qb = to_string(quantity_list[i]);
            string qa = to_string(quantity_list[i]);
            OrderBookLine += bid + "," + qb + "," + ask + "," + qa + ",";
        }
        OrderBookLine += "\n";
        return OrderBookLine;
    };
    vector<int> spread_cycle = {2,4,6,8,6,4};
    for(int i = 0; i < count; ++i){
        int spread = spread_cycle[i % 6];  // each bond has cyclic spread
        int max_spread = spread + 8;  // consider 5 ticks depth on each side
        for(int j = 0; j < 6; ++j){
            int bid1 = 4 + rand() % (512 - max_spread);
            int ask1 = bid1 + spread;
            market << cusip_codes[j] << "," << GenerateBookLine(bid1, ask1);
        }
    }
}

void DataGenerator::GenerateInquiriesInput(int count){
    ofstream inquiries;
    inquiries.open("../input/inquiries.txt");
    inquiries << "InquiryID, CUSIP, Quantity, Side, Price, InquiryState\n";
    for(int i = 0; i < count; ++i){
        for(int j = 0; j < 6; ++j){
            int inquiry_id = i * 6 + j + 1;
            int quantity = 1000000 * (1 + rand() % 6);
            inquiries << inquiry_id << "," << cusip_codes[j] << ","
                      << quantity << "," << trade_sides[rand() % 2]
                      << "," << GeneratePrice(rand() % 512) << ",RECEIVED\n";
        }
    }
    inquiries.close();
}

#endif //TRADING_SYSTEM_DATA_GENERATOR_HPP
