#ifndef DB_H
#define DB_H
#include "GetWeather.h"
#include "sqlite3.h"

class Db
{
private:
    sqlite3* m_db;
public:
    Db();

    void connectDatabase(string outputName);
    void createTable();
    void insertData(GetWeather& weatherData, float strength_dbm, float cnr_db, string adapterString);
    void disconnectDatabase();

};

#endif