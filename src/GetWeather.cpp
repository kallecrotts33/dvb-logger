#include "GetWeather.h"
#include <iostream>
#include <ctime>
#include <string>
#include <curl/curl.h>
#include <cmath>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

// Constructor
GetWeather::GetWeather(string &jsonString)
    : m_jsonData(jsonString)
{
    json data = json::parse(jsonString);

    const json &current = data.at("current");

    m_dateTime = current.at("time").get<std::time_t>();
    m_temperature = current.at("temperature_2m").get<float>();
    m_relativeHumidity = current.at("relative_humidity_2m").get<float>();
    m_precipitation = current.at("precipitation").get<float>();
    m_cloudCover = current.at("cloud_cover").get<float>();
    m_latitude = data.at("latitude").get<double>();
    m_longitude = data.at("longitude").get<double>();

    // calculate sun position manually
    constexpr double DEG = M_PI / 180.0;
    constexpr double RAD = 180.0 / M_PI;
    // calculate current time in UTC
    time_t now = std::time(nullptr);
    tm utc{};
    gmtime_r(&now, &utc);  
    // calculate day of the year and time in hours
    int day = utc.tm_yday + 1;            
    double time = utc.tm_hour + utc.tm_min / 60.0;
    // calculate solar declination
    double decl = -23.44 * DEG * cos(2.0 * M_PI / 365.0 * (day + 10));
    // calculate solar hour angle
    double solarTime = time + m_longitude / 15.0;
    double H = (solarTime - 12.0) * 15.0 * DEG;
    double lat = m_latitude * DEG;
    // calculate solar elevation
    double elevation = asin(sin(lat) * sin(decl) + cos(lat) * cos(decl) * cos(H)) * RAD;
    // calculate solar azimuth
    double azimuth = atan2(sin(H), cos(H) * sin(lat) - tan(decl) * cos(lat)) * RAD + 180.0;
    azimuth = fmod(azimuth + 360.0, 360.0);

    m_sunElevation = elevation;
    m_sunAzimuth = azimuth;
}

void GetWeather::PrintWeather()
{
    char humanTime[50];
    strftime(humanTime, 50, "%a %b %e %H:%M:%S %Y", localtime(&m_dateTime));
    cout << "Coordinates: (" << m_latitude << ", " << m_longitude << ")" << endl;
    cout << "DateTime: " << m_dateTime << " : " << humanTime << endl;
    cout << "Temperature: " << m_temperature << " °C" << endl;
    cout << "Relative Humidity: " << m_relativeHumidity << " %" << endl;
    cout << "Precipitation: " << m_precipitation << " mm" << endl;
    cout << "Cloud Cover: " << m_cloudCover << " %" << endl;
    cout << "Sun Elevation: " << m_sunElevation << " °" << endl;
    cout << "Sun Azimuth: " << m_sunAzimuth << " °" << endl;
}
