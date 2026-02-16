#include <iostream>
#include "GetWeather.h"
#include "Db.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <linux/dvb/frontend.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

string getCurl(string url)
{
  CURL *curl;
  CURLcode res;
  string readBuffer;

  curl = curl_easy_init();
  if (curl)
  {
    long httpCode = 0;
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      cout << "curl operation failed: " << curl_easy_strerror(res) << ", NOTHING WRITTEN TO DB!" << endl;
      readBuffer = "";
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode >= 400)
    {
      cout << "HTTP error: " << httpCode << ", is CURL string correct? NOTHING WRITTEN TO DB!" << endl;
      readBuffer = "";
    }
    curl_easy_cleanup(curl);
  }
  return readBuffer;
}

bool getSignalStrength(int fd, dtv_fe_stats &out)
{
  dtv_property prop{};
  dtv_properties props{};

  prop.cmd = DTV_STAT_SIGNAL_STRENGTH;
  props.num = 1;
  props.props = &prop;

  if (ioctl(fd, FE_GET_PROPERTY, &props) == -1)
    return false;

  out = prop.u.st;
  return true;
}

bool getSignalCNR(int fd, dtv_fe_stats &out)
{
  dtv_property prop{};
  dtv_properties props{};

  prop.cmd = DTV_STAT_CNR;
  props.num = 1;
  props.props = &prop;

  if (ioctl(fd, FE_GET_PROPERTY, &props) == -1)
    return false;

  out = prop.u.st;
  return true;
}

bool getStatus(int fd, fe_status_t &status)
{
  if (ioctl(fd, FE_READ_STATUS, &status) == -1)
    return false;
  return true;
}

bool getDvbValues(string adapterString, float &strength_dbm, float &cnr_db, fe_status_t &status)
{
  dtv_fe_stats strength_stats, cnr_stats;

  int fd = open(adapterString.c_str(), O_RDONLY);
  if (fd < 0)
  {
    return false;
  }

  if (!getStatus(fd, status))
  {
    close(fd);
    return false;
  }

  if (!getSignalStrength(fd, strength_stats))
    return false;

  if (!getSignalCNR(fd, cnr_stats))
    return false;

  strength_dbm = strength_stats.stat[0].svalue / 1000.0;
  cnr_db = cnr_stats.stat[0].svalue / 1000.0;

  return true;
}

bool argHandler(int argc, char *argv[], string &targetAdapter, string& targetFrontend, string &outputName, double &targetLatitude, double &targetLongitude, chrono::seconds &log_rate, bool &testMode)
{
  bool success = true;
  for (int i = 1; i < argc; ++i)
  {
    string arg = argv[i];
    if (arg == "--help" && i + 1 <= argc)
    {
      cout << "Usage: " << argv[0] << " [options]" << endl;
      cout << "Options:" << endl;
      cout << "  --adapter <value>        Path to DVB adapter (default: 0)" << endl;
      cout << "  --frontend <value>       Path to DVB frontend (default: 0)" << endl;
      cout << "  --output <name>         Output database name (default: weatherdata)" << endl;
      cout << "  --latitude <value>      Target latitude (default: 63.06)" << endl;
      cout << "  --longitude <value>     Target longitude (default: 21.37)" << endl;
      cout << "  --lograte <value>       Logging rate in seconds (default: 900), 900s = 15min" << endl;
      cout << "  --test                  Run in test mode (no DVB reading, only weather data logging)" << endl;
      success = false;
    }
    else if (arg == "--adapter" && i + 1 <= argc)
    {
      targetAdapter = argv[++i];
    }
    else if (arg == "--frontend" && i + 1 <= argc)
    {
      targetFrontend = argv[++i];
    }
    else if (arg == "--output" && i + 1 <= argc)
    {
      outputName = argv[++i];
    }
    else if (arg == "--latitude" && i + 1 <= argc)
    {
      string tempString = argv[++i];
      for (auto i = tempString.begin(); i != tempString.end(); ++i)
      {
        if (*i == ',')
        {
          *i = '.';
        }
      }
      targetLatitude = stod(tempString);
    }
    else if (arg == "--longitude" && i + 1 <= argc)
    {
      string tempString = argv[++i];
      for (auto i = tempString.begin(); i != tempString.end(); ++i)
      {
        if (*i == ',')
        {
          *i = '.';
        }
      }
      targetLongitude = stod(tempString);
    }
    else if (arg == "--lograte" && i + 1 <= argc)
    {
      log_rate = chrono::seconds(stoi(argv[++i]));
    }
    else if (arg == "--test")
    {
      cout << "Running in test mode: No DVB reading, only weather data logging" << endl;
      testMode = true;
    }
    else
    {
      cout << "Unknown argument: " << arg << endl;
      cout << "For help: " << argv[0] << " --help" << endl;
      success = false;
    }
  }
  return success;
}

int main(int argc, char *argv[])
{
  fe_status_t status;
  float strength_dbm = 0.0;
  float cnr_db = 0.0;

  // Variables
  string targetAdapter = "0";
  string targetFrontend = "0";
  string outputName = "weatherdata";
  chrono::seconds log_rate = 60s * 15;
  double targetLatitude = 63.06;
  double targetLongitude = 21.37;
  bool testMode = false;
  string adapterString;
  string weather_api_url = "";

  // Handle command line arguments
  if (!argHandler(argc, argv, targetAdapter, targetFrontend, outputName, targetLatitude, targetLongitude, log_rate, testMode))
  {
    return 1;
  }
  else
  {
    adapterString = "/dev/dvb/adapter" + targetAdapter + "/frontend" + targetFrontend;
    weather_api_url = "https://api.open-meteo.com/v1/forecast?latitude=" + to_string(targetLatitude) + "&longitude=" + to_string(targetLongitude) + "&current=temperature_2m,relative_humidity_2m,precipitation,cloud_cover&timeformat=unixtime";
    cout << "Fetching weather data from API: " << weather_api_url << endl;
    cout << "Using adapter: " << adapterString << endl;
    cout << "Output database name: " << outputName << endl;
    cout << "Target Latitude: " << targetLatitude << ", Target Longitude: " << targetLongitude << endl;
    cout << "Logging rate: " << log_rate.count() << " seconds" << endl;
  }
  Db signalHistory;
  signalHistory.connectDatabase(outputName);
  signalHistory.createTable();

  while (true)
  {
    if (!getDvbValues(adapterString, strength_dbm, cnr_db, status) && !testMode)
    {
      cout << "Failed to read DVB values or no device found" << endl;
    }
    else if (!(status & FE_HAS_LOCK) && !testMode)
    {
      cout << "DVB signal not locked" << endl;
    }
    else
    {
      string jsonString = getCurl(weather_api_url);
      if (jsonString == "")
      {
        cout << "Trying again in 5 seconds..." << endl;
        this_thread::sleep_for(5s);
        continue;
      }
      GetWeather weatherData(jsonString);
      signalHistory.insertData(weatherData, strength_dbm, cnr_db, adapterString);

      cout << "Inserted data at time: " << time(0) << endl;
      cout << "Signal Strength (dBm): " << NULL << ", CNR (dB): " << NULL << endl;
      weatherData.PrintWeather();
    }
    this_thread::sleep_for(log_rate);
  }
  return 0;
}
