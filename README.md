# dvb-logger

## About
dvb-logger is a program, with the goal to simply log the signal strength and noise ratio of a DVB signal. The program requires a signal lock to measure the strength and noise, in all of my testing I have used [minisatip](https://github.com/catalinii/minisatip). 
The program logs weather data from [Open Meteo](https://open-meteo.com/), which was chosen simply because this API has the best free tier of the APIs i found. dvb-logger is being developed as a part of a school project / degree project, with that being said, it may not be perfect.

## Dependencies:
In order to build dvb-logger, some dependencies are needed, so make sure to install the following:
```
libsqlite3-dev
libcurl4-openssl-dev
```

## Building and running
### Building
To build, simply make sure that all dependencies are installed, then run the command ``make``. If the build succeeds, there should be a file called dvb-logger in the root directory.

### Running dvb-logger
Following a successful build of the program, dvb-logger can simply be run with the command ``./dvb-logger``, the program will start logging the data to a database file, the default name is ``weatherdata.db``. The program will only log to the database if the DVB interface has a signal lock, and the API request is successful, it is important to ensure signal lock and internet connectivity.

### Arguments and use cases
List of valid arguments:
```
--help
--adapter
--output
--latitude
--longitude
--lograte
```
Example usage:
```
./dvb-logger --help
```
Returns:
```
Usage: ./dvb-logger [options]
Options:
  --adapter <value>       Path to DVB adapter (default: 0)
  --frontend <value>      Path to DVB frontend (default: 0)
  --output <name>         Output database name (default: weatherdata)
  --latitude <value>      Target latitude (default: 63.06)
  --longitude <value>     Target longitude (default: 21.37)
  --lograte <value>       Logging rate in seconds (default: 900), 900s = 15min
  --test                  Run in test mode (no DVB reading, only weather data logging)
```

#### Help
The ``--help`` argument generates a list of valid arguments, as seen in the example above. 

#### Adapter
The ``--adapter`` argument changes the value of the adapter path, this can be changed in order to log the signal from a specific DVB card or a specific frontend, this is also useful for comparing two devices.

#### Output
The ``--output`` argument changes the name of the log file, the file extension ``.db`` is automatically added. This feature is useful for keeping track of multiple logs.

#### Latitude and longitude
The ``--latitude`` and ``--longitude`` arguments let the user decide what location the weather data should correspond to. This argument is essential for getting any useful data from this program.

#### Lograte
The ``--lograte`` argument is quite simple, it decides how often the database should be updated. The default time here is 15min, this time is chosen because Open Meteos API only updates once per 15min. It is however possible to log faster than the API updates, this will still show changes in signal strength, noise and sun parameters. The database stores two timestamps, one that comes with the weather API, and one for the actual reading of the DVB signal.

#### Test
The ``--test`` argument makes the program run in `test mode`, this simply ignores the signal lock requirement of the program. This is useful if the user wishes to set up the program, without needing to establish a signal lock.

## Usage examples
### Example 1, the user wants to use the default adapter, and wants to log once every hour, from an antenna in a specific location, and a custom output.
Command: ``./dvb-logger --lograte 3600 --latitude 37.22 --longitude -122.4 --output "my_output"``

### Example 2, the user wants to log two separate signals to two different files.
In one terminal run: ``./dvb-logger --adapter "/dev/dvb/adapter0/frontend0" --output "adapter0"``
In the second terminal run: ``./dvb-logger --adapter "/dev/dvb/adapter1/frontend0" --output "adapter1"``

