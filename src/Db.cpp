#include <iostream>
#include <sqlite3.h>
#include "Db.h"
using namespace std;
    
Db::Db()
{
    m_db = nullptr;
}

void Db::connectDatabase(string outputName)
{   
    string dbName = "./" + outputName + ".db";
    int rc = sqlite3_open(dbName.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        cout << "Failed to open database: " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "Database opened successfully." << endl;
    }
}

void Db::createTable()
{
    const char* createTableSQL = 
        "CREATE TABLE IF NOT EXISTS WeatherData ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "WeatherTimeStamp DATETIME, "
        "SignalTimeStamp DATETIME, "
        "Adapter TEXT, "
        "Temperature REAL, "
        "RelativeHumidity REAL, "
        "Precipitation REAL, "
        "CloudCover REAL, "
        "SignalStrength REAL, "
        "CNR REAL, "
        "SunElevation REAL, "
        "SunAzimuth REAL,"
        "Latitude REAL, "
        "Longitude REAL);";

    int rc = sqlite3_exec(m_db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Error creating table: " << sqlite3_errmsg(m_db) << endl;
    } else {
        cout << "Table created successfully." << endl;
    }
}

void Db::insertData(GetWeather& weatherData, float strength_dbm, float cnr_db, string adapterString)
{
    const char* insertSQL = 
        "INSERT INTO WeatherData ("
        "WeatherTimeStamp,"
        "SignalTimeStamp, "
        "Adapter, "
        "Temperature, "
        "RelativeHumidity, "
        "Precipitation, "
        "CloudCover, "
        "SignalStrength, "
        "CNR, "
        "SunElevation, "
        "SunAzimuth, "
        "Latitude, "
        "Longitude) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    

    int rc = sqlite3_prepare_v2(m_db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Error preparing statement: " << sqlite3_errmsg(m_db) << endl;
        return;
    }

    // Bind parameters
    sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(weatherData.getDateTime()));
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(time(0)));
    sqlite3_bind_text(stmt, 3, adapterString.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, static_cast<double>(weatherData.getTemperature()));
    sqlite3_bind_double(stmt, 5, static_cast<double>(weatherData.getRelativeHumidity()));
    sqlite3_bind_double(stmt, 6, static_cast<double>(weatherData.getPrecipitation()));
    sqlite3_bind_double(stmt, 7, static_cast<double>(weatherData.getCloudCover()));
    sqlite3_bind_double(stmt, 8, strength_dbm);
    sqlite3_bind_double(stmt, 9, cnr_db);
    sqlite3_bind_double(stmt, 10, static_cast<double>(weatherData.getSunElevation()));
    sqlite3_bind_double(stmt, 11, static_cast<double>(weatherData.getSunAzimuth()));
    sqlite3_bind_double(stmt, 12, static_cast<double>(weatherData.getLatitude()));
    sqlite3_bind_double(stmt, 13, static_cast<double>(weatherData.getLongitude()));
    


    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "Error inserting data: " << sqlite3_errmsg(m_db) << endl;
    }

    sqlite3_finalize(stmt);
}
       
void Db::disconnectDatabase()
{
    sqlite3_close(m_db);   
}