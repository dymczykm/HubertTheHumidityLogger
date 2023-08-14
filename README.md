# Hubert the Humidity Logger

![Photo of Hubert](/pictures/hubert_photo.jpg)

Huber the Humidity Logger is a project of an IoT device that measures the relative humidity, temperature and pressure and reports it back to a remote server using the built-in 4G modem. I created this project as I couldn't find a device off the shelf that could log humidity using the cellular network and would not cost an arm and a leg.

Hubert can use a HTTP POST action to submit data to virtually any server, either running in the cloud (I tested it with https://io.adafruit.com/) or maintained on your own (my Hubert is submitting data to a Raspberry Pi running InfluxDB).

Hubert is build around (Makerfabs Maduino Zero 4G LTE)[ttps://www.makerfabs.com/maduino-zero-4g-lte-sim7600.html] and loosely inspired by the [DIY Life 4G air quality weather station](https://www.the-diy-life.com/diy-4g-air-quality-and-environment-monitor-record-data-anywhere/).

For more details, please refer to the project page: https://dymczyk.com/hubert-the-humidity-logger


## Hardware

To build Hubert, you need the following components:
- Maduino Zero 4G/LTE board: https://www.makerfabs.com/maduino-zero-4g-lte-sim7600.html (includes an Arduino Zero compatible MCU + a Simcom 7600 modem)
- Hubert breakout board with a bunch of LEDs and a connector for the humidity sensor: KiCad files in `hardware/HumidityLoggerBreakout`
- BME280 sensor compatible with 3.3V supply [(Aliexpress item I used)](https://www.aliexpress.com/item/32862445164.html)
- A SIM card with some data allowance to post the humidity/temperature/pressure measurements

## Firmware

The firmware of Hubert is written in Arduino IDE and is located in `firmware/humidity_logger`.

The firmware does three main things:
1. Triggers the measurement process of BME280
2. POSTs the measurements to a remote server
3. Puts the modem and the MCU to sleep to save power

To adapt it to your application, you mostly need to put the credentials to access the server in `influxdb.h` file:
```
#define USER "xxx"
#define PASSWORD "xxx"
#define DB_URL "xxx"
```

## Remote server

Hubert uses a 4G connection to POST data to an InfluxDB database that resides on a remote server. In my case, it is a Raspberry Pi running continuously InfluxDB and Grafana to visualize the results in a nice dashboard.


## Case

I've designed a 3D printed case for Hubert, you can find the STL files for the top and the bottom in `hardware/case_stl`:

![Fusion 360 case design of Hubert](/pictures/hubert_fusion360.jpg)