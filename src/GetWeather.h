#ifndef GETWEATHER_H
#define GETWEATHER_H
#include <ctime>
#include <string>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
class GetWeather
{
private:
    time_t m_dateTime{};
    float m_temperature{};
    float m_relativeHumidity{};
    float m_precipitation{};
    float m_cloudCover{};
    double m_latitude{};
    double m_longitude{};
    double m_sunElevation{};
    double m_sunAzimuth{};
    string m_adapterString{};
    string m_jsonData{};
public:
    //Constructor
    GetWeather(string& jsonString);

    //functions
    void PrintWeather();

    //get functions
    time_t getDateTime() const { return m_dateTime; }
    float getTemperature() const { return m_temperature; }
    float getRelativeHumidity() const { return m_relativeHumidity; }
    float getPrecipitation() const { return m_precipitation; }
    float getCloudCover() const { return m_cloudCover; }
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    double getSunElevation() const { return m_sunElevation; }
    double getSunAzimuth() const { return m_sunAzimuth; }

    //destructor
    ~GetWeather() = default;

};



#endif