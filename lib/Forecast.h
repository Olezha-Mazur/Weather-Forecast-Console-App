#pragma once
#include <iostream>
#include <cstring>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <cinttypes>
#include <math.h>
#include <sstream>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ftxui/component/component.hpp" 
#include "ftxui/component/component_base.hpp" 
#include "ftxui/component/event.hpp"  
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/loop.hpp" 
#include "Weather.h"

using namespace nlohmann;
using namespace ftxui;

static std::string request = "temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,weather_code,wind_speed_10m";

class Forecast {
private:
    std::vector<Weather> forecast_;
    std::vector<Element> current_;

    void GetWeatherCodeArray(const std::vector<int16_t>& arr_in, std::vector<int16_t>& arr_out);

    void GetTempStrings(const std::vector<float>& first, const std::vector<float>& second, std::vector<std::string>& arr_out);

    void GetPrecStrings(const std::vector<float>& prec, std::vector<std::string>& arr_out);

    void GetHumStrings(const std::vector<int16_t>& prec, std::vector<std::string>& arr_out);

    std::string FloatToString(const float number);

    void HandlePrecipitation(const std::vector<float>& arr, std::vector<std::string>& str);

    void HandleTemperature(const std::vector<float>& app, const std::vector<float>& temp, std::vector<std::string>& str);

    void HandleHumidity(const std::vector<int16_t>& arr, std::vector<std::string>& str);

    void HandleWindSpeed(const std::vector<float>& arr, std::vector<std::string>& str);

    void FindInterval(const std::vector<float>& arr, std::vector<float>& low, std::vector<float>& upp);

    void GetWindStrings(const std::vector<float>& low, const std::vector<float>& upp, std::vector<std::string>& str);

    void HandleWeatherCode(const std::vector<int16_t>& arr, std::vector<int16_t>& weather_code);

    void HandleDateStrings(const std::vector<std::string>& arr, std::vector<std::string>& date);

    void BuildDescription(std::vector<Element>& arr, size_t day_ind, size_t ind, size_t city_ind);

    void BuildWeather(std::vector<Element>& arr, size_t day_ind, size_t city_ind);

    std::string GetWeatherKind(size_t ind);

    Element GetElement(size_t index, bool is_connected);

    size_t GetHour();

    void HandleCurrent(float temp, float app_temp, float prec, int16_t hum, float wind, int16_t code, size_t ind);

    bool GetCityInfo(size_t ind, const std::string& kApi);

    Element DrawPicture(size_t ind);

    size_t GetLength();

    size_t GetFreq();

    json JsonParse();

public:
    std::string file_name = "config.json";
    std::vector<City> config_;

    bool GetForecast(size_t count, size_t ind, const std::string& kApi);

    void ShowForecast(const std::string& kApi);

    size_t GetForecastDays();
};

template<class T>
void FindAverage(const std::vector<T>& arr_in, std::vector<T>& arr_out);