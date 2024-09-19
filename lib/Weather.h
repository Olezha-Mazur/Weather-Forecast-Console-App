#pragma once

#include <iostream>
#include <cstring>
#include <nlohmann/json.hpp>
#include <vector>
#include <cinttypes>

using namespace nlohmann;

struct Constants {
    static const int16_t success_code = 200;
    static const int16_t window_size = 160;
    static const int16_t parts_count = 4;
    static const int16_t max_days = 16;
    static const int16_t second = 1000;
    static const int16_t hours = 24;
    constexpr static const float km_m = 3.6;
    static const int16_t ten = 10;
};

class City {
public:
    City(std::string name, size_t lat, size_t lon);

    float GetLatitude() const;

    float GetLongitude() const;

    std::string GetName() const;

    void ShowInfo() const;

private:
    float latitude_;
    float longitude_;
    std::string name_;
};

struct Weather {
public:
    Weather(const std::vector<std::string>& temp, 
            const std::vector<std::string>& prec, 
            const std::vector<std::string>& hum,
            const std::vector<std::string>& wind,
            const std::vector<std::string>& date,
            const std::vector<int16_t>& weather_code);
    
    std::vector<std::string> temp_;
    std::vector<std::string> hum_;
    std::vector<std::string> prec_;
    std::vector<std::string> wind_;
    std::vector<std::string> date_;
    std::vector<int16_t> weather_code_;
};

bool CheckResponse(size_t answer);

bool CheckEmpty(json file);