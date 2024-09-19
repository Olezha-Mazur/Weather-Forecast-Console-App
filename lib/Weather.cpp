#include "Weather.h"

City::City(std::string name, size_t lat, size_t lon) 
    : name_(name) 
    , latitude_(lat)
    , longitude_(lon)
{}

float City::GetLatitude() const { return latitude_;}

float City::GetLongitude() const { return longitude_;}

std::string City::GetName() const { return name_; }

void City::ShowInfo() const { std::cout << latitude_ << '\t' << longitude_ << '\n'; }

Weather::Weather(const std::vector<std::string>& temp, 
                 const std::vector<std::string>& prec, 
                 const std::vector<std::string>& hum,
                 const std::vector<std::string>& wind,
                 const std::vector<std::string>& date,
                 const std::vector<int16_t>& weather_code) 
: temp_(temp), prec_(prec), hum_(hum), wind_(wind), date_(date), weather_code_(weather_code) {}

bool CheckResponse(size_t answer) {
    return (answer != Constants::success_code) ? false: true;
}

bool CheckEmpty(nlohmann::json file) {
    if (file.empty()) {
        std::cerr << "Response is invalid!";
        return true;
    }
    return false;
}

