#include "Forecast.h"

bool Forecast::GetCityInfo(size_t ind, const std::string& kApi) {
    json info = JsonParse();
    if (ind >= info["cities-names"].size()) {
        std::cerr << "Config is invalid!" << std::endl;
        return false;
    }

    cpr::Response r = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
                                cpr::Parameters{{"name", info["cities-names"][ind]}},
                                cpr::Header{{"X-Api-key", kApi}});
    if (!CheckResponse(r.status_code)) return false;
    json res = json::parse(r.text);
    if (CheckEmpty(res)) return false;
    config_.push_back(City{info["cities-names"][ind], res[0]["latitude"], res[0]["longitude"]});
    return true;
}

bool Forecast::GetForecast(size_t count, size_t ind, const std::string& kApi) {
    if (ind >= forecast_.size()) {
        if (!GetCityInfo(ind, kApi)) return false;
    }
    cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                                cpr::Parameters{{"latitude", std::to_string(config_[ind].GetLatitude())}, 
                                                {"longitude", std::to_string(config_[ind].GetLongitude())},
                                                {"hourly", request},
                                                {"timezone", "auto"},
                                                {"forecast_days", std::to_string(count)}}); 
    if (!CheckResponse(r.status_code)) return false;
    json info = json::parse(r.text);
    if (CheckEmpty(info)) return false;

    int16_t hour = GetHour();

    std::vector<std::string> str_temp;
    HandleTemperature(std::vector<float>(info["hourly"]["apparent_temperature"]), 
                        std::vector<float>(info["hourly"]["temperature_2m"]), 
                        str_temp);
    float current_app_temp = info["hourly"]["apparent_temperature"][hour];
    float current_temp = info["hourly"]["temperature_2m"][hour];

    std::vector<std::string> str_prec;
    HandlePrecipitation(std::vector<float>(info["hourly"]["precipitation"]), str_prec);
    float current_prec = info["hourly"]["precipitation"][hour];
    
    std::vector<std::string> str_hum;
    HandleHumidity(std::vector<int16_t>(info["hourly"]["relative_humidity_2m"]), str_hum);
    int16_t current_hum = info["hourly"]["relative_humidity_2m"][hour];
    
    std::vector<std::string> str_wind;
    HandleWindSpeed(std::vector<float>(info["hourly"]["wind_speed_10m"]), str_wind);
    float current_wind = info["hourly"]["wind_speed_10m"][hour];
    
    std::vector<int16_t> weather_code;
    HandleWeatherCode(std::vector<int16_t>(info["hourly"]["weather_code"]), weather_code);
    int16_t current_code = info["hourly"]["weather_code"][hour];
    
    std::vector<std::string> date;
    HandleDateStrings(std::vector<std::string>(info["hourly"]["time"]), date);
    HandleCurrent(current_temp, current_app_temp, current_prec, current_hum, current_wind, current_code, ind);

    if (ind == forecast_.size()) {
        forecast_.push_back(Weather{str_temp, str_prec, str_hum, str_wind, date, weather_code});
    } else {
        forecast_[ind] = Weather{str_temp, str_prec, str_hum, str_wind, date, weather_code};
    }
    return true;
}

std::string Forecast::GetWeatherKind(size_t ind) {
    switch (ind) {
    case 0:
        return "Clear sky";
    case 1:
        return "Mainly clear";
    case 2:
        return "Partly cloudy";
    case 3:
        return "Cloudy";
    case 45:
        return "Fog";
    case 48:
        return "Depositing rime fog";
    case 51:
        return "Light drizzle";
    case 53:
        return "Drizzle";
    case 55:
        return "Dense drizzle";
    case 56:
        return "Light freezing drizzle";
    case 57:
        return "Dense freezing drizzle";
    case 61:
        return "Light rain";
    case 63:
        return "Rain";
    case 65:
        return "Heavy rain";
    case 66:
        return "Light freezing rain";
    case 67:
        return "Heavy freezing rain";
    case 71:
        return "Light snow";
    case 73:
        return "Snow";
    case 75:
        return "Heavy snow";
    case 77:
        return "Snow grains";
    case 80:
        return "Light rain showers";
    case 81:
        return "Shower";
    case 82:
        return "Violent showers";
    case 85:
        return "Light snowfall";
    case 86:
        return "Heavy snowfall";
    case 95:
        return "Thunderstorm";
    case 96:
        return "Thunderstorm with small hail";
    case 99:
        return "Thunderstorm with heavy hail";   
    default:
        return "Clear sky";     
    }
}

Element Forecast::DrawPicture(size_t ind) {
    std::vector<Element> picture = {text("    .--.   "), 
                                    text(" .-(    ). "), 
                                    text("(___.__)__)")};
    if (ind == 71) {
        picture.push_back(text("*  *  *  *"));
    } else if (ind == 73 || ind == 85) {
        picture.push_back(vbox({text(" *  *  *  *"), 
                               text("*  *  *  *")}));
    } else if (ind == 75 || ind == 86) {
        picture.push_back(vbox({text("  *  *  *  "), 
                               text(" *  *  *  "), 
                              text("*  *  *  ")}));
    } else if (ind == 61 || ind == 51) {
        picture.push_back(text(",  ,  ,  ,") | color(Color::Blue));
    } else if (ind == 63 || ind == 53) {
        picture.push_back(vbox({text(" ,  ,  ,  ,") | color(Color::Blue), 
                               text(",  ,  ,  ,") | color(Color::Blue)}));
    } else if (ind == 65 || ind == 55) {
        picture.push_back(vbox({text("  ,  ,  ,  ") | color(Color::Blue), 
                               text(" ,  ,  ,  ") | color(Color::Blue), 
                              text(",  ,  ,  ") | color(Color::Blue)}));
    } else if (ind == 66 || ind == 56) {
        picture.push_back(vbox({text(",  ") | color(Color::Blue), 
                              text("*  "), text(",  ") | color(Color::Blue), text("* ")}));
    } else if (ind == 67 || ind == 57) {
        picture.push_back(vbox({text(",  ") | color(Color::Blue), 
                              text("*  "), text(",  ") | color(Color::Blue), text("* ")}));
        picture.push_back(vbox({text("*  "), text(",  ") | color(Color::Blue), 
                              text("*  "), text(",   ") | color(Color::Blue)}));
    } else if (ind == 45) {
        picture.clear();
        picture.push_back(vbox({text("--  __  --  __"), 
                                text(" __ --  __ -- "), 
                                text("--  ____ -- __")}));
    } else if (ind == 48) {
        picture.clear();
        picture.push_back(vbox({text("-*  __  *-  __"), 
                                text(" __ -*  __ -* "), 
                                text("*-  ____ -* __")}));
    } else if (ind == 0) {
        picture.clear();
        picture.push_back(vbox({text("   \\     /   ") | color(Color::Yellow), 
                                text("    . - .    ") | color(Color::Yellow), 
                                text("―― (     ) ――") | color(Color::Yellow),
                                text("    ` - ’    ") | color(Color::Yellow),
                                text("   /     \\ ") | color(Color::Yellow)}));
    } else if (ind == 2) {
        picture.clear();
        picture.push_back(vbox({hbox({text("  .-.  "), text("\\   /     ") | color(Color::Yellow)}), 
                                hbox({text(" (   )."), text("/'''") | color(Color::Yellow), text(".-.    ")}), 
                                hbox({text("(___(__)"), text("__") | color(Color::Yellow), text("(   ). ")}), 
                                hbox({text("       / ") | color(Color::Yellow), text("(___(__) ")})}));
    } else if (ind == 1) {
        picture.clear();
        picture.push_back(vbox({text("  \\  /     ") | color(Color::Yellow), 
                                hbox({text("_ /''") | color(Color::Yellow), text(".-.    ")}), 
                                hbox({text("  \\_") | color(Color::Yellow), text("(   ). ")}), 
                                hbox({text("  /") | color(Color::Yellow), text("(___(__) ")})}));
    } else if (ind == 77) {
        picture.push_back(vbox({text(" .  .  .  .") | color(Color::Blue), 
                               text(".  .  .  .") | color(Color::Blue)}));
    } else if (ind == 80) {
        picture.push_back(text("/  /  /  /") | color(Color::Blue));
    } else if (ind == 81) {
        picture.push_back(vbox({text(" /  /  /  /") | color(Color::Blue), 
                               text("/  /  /  /") | color(Color::Blue)}));
    } else if (ind == 82) {
        picture.push_back(vbox({text("  /  /  /  ") | color(Color::Blue), 
                               text(" /  /  /  ") | color(Color::Blue), 
                              text("/  /  /  ") | color(Color::Blue)}));
    } else if (ind == 95) {
        picture.push_back(vbox({text(" /   /   ") | color(Color::Yellow), 
                                text(" /   /   ") | color(Color::Yellow)}));
    } else if (ind == 96) {
        picture.push_back(vbox({hbox({text(" / ") | color(Color::Yellow), 
                                      text("."), text(" /   /  ") | color(Color::Yellow)}), 
                                hbox({text("."), text("/   / ") | color(Color::Yellow), 
                                      text("."), text(" /  ") | color(Color::Yellow)})}));
    } else if (ind == 99) {
        picture.push_back(vbox({hbox({text(" / ") | color(Color::Yellow), 
                                      text("*"), text(" /   /  ") | color(Color::Yellow)}), 
                                hbox({text("*"), text("/   / ") | color(Color::Yellow), 
                                      text("*"), text(" /  ") | color(Color::Yellow)})}));
    }
    return vbox(picture);
}

void Forecast::BuildDescription(std::vector<Element>& arr, size_t day_ind, size_t ind, size_t city_ind) {
    int16_t count = Constants::parts_count;
    arr.push_back(DrawPicture(forecast_[city_ind].weather_code_[day_ind * count + ind]) | center | size(WIDTH, EQUAL, Constants::window_size / (count * 2)));
    std::vector<Element> ans;
    ans.push_back(text(GetWeatherKind(forecast_[city_ind].weather_code_[day_ind * count + ind])));
    ans.push_back(text(forecast_[city_ind].temp_[day_ind * count + ind]));
    ans.push_back(text(forecast_[city_ind].wind_[day_ind * count + ind]));
    ans.push_back(text(forecast_[city_ind].hum_[day_ind * count + ind] + " | " + forecast_[city_ind].prec_[day_ind * count + ind]));
    arr.push_back(vbox(ans) | center | size(WIDTH, EQUAL, Constants::window_size / (count * 2)));
}

void Forecast::BuildWeather(std::vector<Element>& arr, size_t day_ind, size_t city_ind) {
    std::string s;
    int16_t count = Constants::parts_count;
    for (size_t i = 0; i < 4; ++i) {
        std::vector<Element> arr1;
        BuildDescription(arr1, day_ind, i, city_ind);
        if (i == 0) {
            s = "Night";
        } else if (i == 1) {
            s = "Morning";
        } else if (i == 2) {
            s = "Afternoon";
        } else if (i == 3) {
            s = "Evening";
        }
        arr.push_back(vbox({text(s) | center, separator(), hbox(arr1) | flex | size(WIDTH, EQUAL, Constants::window_size / count)}) | flex);
        arr.push_back(separator());
    }
    arr.pop_back();
}

size_t Forecast::GetHour() {
    std::time_t result = std::time(nullptr);
    std::string s = std::ctime(&result);
    return ((s.substr(11, 2)[0] - '0') * 10 + (s.substr(11, 2)[1] - '0'));
}

void Forecast::HandleCurrent(float temp, float app_temp, float prec, int16_t hum, float wind, int16_t code, size_t ind) {
    std::vector<Element> ans;
    std::vector<std::string> buff;

    buff.push_back(GetWeatherKind(code));
    HandleTemperature({temp}, {app_temp}, buff);
    HandleWindSpeed({wind}, buff);
    HandleHumidity({hum}, buff);
    HandlePrecipitation({prec}, buff);

    std::string str_hum = buff.back();
    buff.pop_back();
    buff.back() += " | ";
    buff.back() += str_hum;

    for (size_t i = 0; i < buff.size(); ++i) {
        ans.push_back(text(buff[i]));
    }

    if (current_.size() == ind) {
        current_.push_back(border(hbox({DrawPicture(code) | center | size(WIDTH, EQUAL, Constants::window_size / (4 * 2)), 
            vbox(ans) | center | size(WIDTH, EQUAL, Constants::window_size / (4 * 2))})) | size(WIDTH, EQUAL, Constants::window_size / 4) | flex);
    } else {
        current_[ind] = border(hbox({DrawPicture(code) | center | size(WIDTH, EQUAL, Constants::window_size / (4 * 2)), 
            vbox(ans) | center | size(WIDTH, EQUAL, Constants::window_size / (4 * 2))})) | size(WIDTH, EQUAL, Constants::window_size / 4) | flex;
    }
}

Element Forecast::GetElement(size_t index, bool is_connected) {
    Weather city = forecast_[index];
    std::vector<Element> answer;
    std::string mode = "";
    if (!is_connected) mode = "offline";
    for (size_t i = 0; i < city.date_.size(); ++i) {
        std::vector<Element> arr;
        BuildWeather(arr, i, index);
        std::string mode = "";
        if (!is_connected) mode = "offline";
        answer.push_back(window(text(city.date_[i]) | center, hbox(arr) | flex) | size(WIDTH, EQUAL, Constants::window_size));
    }
    
    return vbox(hbox({text(config_[index].GetName()) | flex}) | size(WIDTH, EQUAL, Constants::window_size), 
                      hbox({current_[index] | size(WIDTH, EQUAL, Constants::window_size / 2), 
                      text(mode) | color(Color::Red) | center | size(WIDTH, EQUAL, Constants::window_size / 2)}), answer);
}

void Forecast::ShowForecast(const std::string& kApi) {
    size_t freq = GetFreq();
    size_t ind = 0;
    size_t loop_count = 0;
    bool is_connected = true;

    auto screen = ScreenInteractive::FitComponent();
    auto renderer = Renderer([&] { return GetElement(ind, is_connected); });
    renderer |= CatchEvent([&](Event event) -> bool {
        if (event == Event::Special("\x1B")) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('+')) {
            if (forecast_[ind].date_.size() < (Constants::max_days)) {
                (!GetForecast(forecast_[ind].date_.size() + 1, ind, kApi)) ? is_connected = false: is_connected = true;
            }
            return true;
        }
        if (event == Event::Character('-')) {
            if (forecast_[ind].date_.size() > 1) {
                (!GetForecast(forecast_[ind].date_.size() - 1, ind, kApi)) ? is_connected = false: is_connected = true;
            }
            return true;
        }
        if (event == Event::Character('n')) {
            if (ind < GetLength() - 1) ++ind;
            if (ind == forecast_.size()) {
                if (!GetForecast(GetForecastDays(), ind, kApi)) {
                    --ind;
                    is_connected = false;
                } else {
                    is_connected = true;
                }
            }
            return true;
        }
        if (event == Event::Character('p')) {
            if (ind > 0) --ind;
            return true;
        }
        return false; });
    Loop loop(&screen, renderer);
    while (!loop.HasQuitted())
    {
        ++loop_count;
        if (loop_count % (Constants::second * freq) == (Constants::second * freq - 1)) {
            (!GetForecast(forecast_[ind].date_.size(), ind, kApi)) ? is_connected = false: is_connected = true;
            loop_count = 0;
        }
        loop.RunOnce();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Forecast::HandleDateStrings(const std::vector<std::string>& arr, std::vector<std::string>& date) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i % Constants::hours == 0) {
            date.push_back(arr[i].substr(0, 10));
        }
    }
}

void Forecast::HandleWeatherCode(const std::vector<int16_t>& arr, std::vector<int16_t>& weather_code) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (i % (Constants::hours / Constants::parts_count) == 0) {
            weather_code.push_back(arr[i]);
        }
    }
}

void Forecast::HandleWindSpeed(const std::vector<float>& arr, std::vector<std::string>& str) {
    std::vector<float> low;
    std::vector<float> upp;
    FindInterval(arr, low, upp);
    GetWindStrings(low, upp, str);
}

void Forecast::FindInterval(const std::vector<float>& arr, std::vector<float>& low, std::vector<float>& upp) {
    float mx = arr[0];
    float mn = arr[0];
    
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] > mx) { mx = arr[i]; }
        if (arr[i] < mn) { mn = arr[i]; }
        if (i % (Constants::hours / Constants::parts_count) == 0 && (i != 0)) {
            low.push_back(mn);
            upp.push_back(mx);
            mx = arr[i];
            mn = arr[i];
        }
    }
    low.push_back(mn);
    upp.push_back(mx);
}

void Forecast::GetWindStrings(const std::vector<float>& low, const std::vector<float>& upp, std::vector<std::string>& str) {
    int16_t t = Constants::ten;
    float km = Constants::km_m;
    for (size_t i = 0; i < low.size(); ++i) {
        std::string elem;
        if (round(low[i] * t) / t != round(upp[i] * t) / t) {
            elem += FloatToString(round((low[i] / km) * t) / t);
            elem += '-';
            elem += FloatToString(round((upp[i] / km) * t) / t);
            elem += " m/s";
        } else {
            elem += FloatToString(round((low[i] / km) * t) / t);
            elem += " m/s";
        }
        str.push_back(elem);
    }
}

void Forecast::HandleHumidity(const std::vector<int16_t>& arr, std::vector<std::string>& str) {
    std::vector<int16_t> hum;
    FindAverage(arr, hum);
    GetHumStrings(hum, str);
}

void Forecast::HandlePrecipitation(const std::vector<float>& arr, std::vector<std::string>& str) {
    std::vector<float> prec; 
    FindAverage(arr, prec);
    GetPrecStrings(prec, str);
}

void Forecast::HandleTemperature(const std::vector<float>& app, const std::vector<float>& fact, std::vector<std::string>& str) {
    std::vector<float> app_temp;
    FindAverage(app, app_temp);
    std::vector<float> temp;
    FindAverage(fact, temp);
    GetTempStrings(temp, app_temp, str);
}

std::string Forecast::FloatToString(const float number) {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

void Forecast::GetHumStrings(const std::vector<int16_t>& prec, std::vector<std::string>& arr_out) {
    for (size_t i = 0; i < prec.size(); ++i) {
        std::string elem;
        elem += std::to_string(int(round(prec[i])));
        elem += "%";
        arr_out.push_back(elem);
    }
}

void Forecast::GetPrecStrings(const std::vector<float>& prec, std::vector<std::string>& arr_out) {
    for (size_t i = 0; i < prec.size(); ++i) {
        std::string elem;
        elem += FloatToString(round(prec[i] * Constants::ten) / Constants::ten);
        elem += " mm";
        arr_out.push_back(elem);
    }
}

void Forecast::GetTempStrings(const std::vector<float>& first, const std::vector<float>& second, std::vector<std::string>& arr_out) {
    int16_t t = Constants::ten;
    for(size_t i = 0; i < first.size(); ++i) {
        std::string elem;
        if ((round(first[i] * t) / t) > 0) {
            elem += '+';
        }
        elem += FloatToString(round(first[i] * t) / t);
        elem += '(';
        if ((round(second[i] * t) / t) > 0) {
            elem += '+';
        }
        elem += FloatToString(round(second[i] * t) / t);
        elem += ")°C";
        arr_out.push_back(elem);
    }
}

void Forecast::GetWeatherCodeArray(const std::vector<int16_t>& arr_in, std::vector<int16_t>& arr_out) {
    for (size_t i = 0; i < arr_in.size(); ++i) {
        if (i % (Constants::hours / Constants::parts_count) == 0) {
            arr_out.push_back(arr_in[i]);
        }
    }
}

template<class T>
void FindAverage(const std::vector<T>& arr_in, std::vector<T>& arr_out) {
    T sm = 0;
    int16_t part = (Constants::hours / Constants::parts_count);
    for (size_t i = 0; i < arr_in.size(); ++i) {
        if ((i % part == 0) && i != 0) {
            arr_out.push_back(sm / part);
            sm = 0;
        }
        sm += arr_in[i];
    }
    arr_out.push_back(sm / part);
}

size_t Forecast::GetForecastDays() {
    return JsonParse()["forecast-days"];
}

size_t Forecast::GetLength() {
    return JsonParse()["cities-names"].size();
}

size_t Forecast::GetFreq() {
    return JsonParse()["update-frequency"];
}

json Forecast::JsonParse() {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Directory is invalid. Please, check file!" << std::endl;
        return false;
    }
    return json::parse(file);
}