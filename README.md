![Water Tank Diagram](/images/WaterTankDiagram.png)

# Watermonitor Controller

> Well water pumphouse microcontroller monitoring projects for monitoring the depth of a water tank and reporting to the [io.adafruit.com](https://io.adafruit.com/).

> [**esp8266_monitor**](https://github.com/kokuda/watermonitor_controller/blob/master/esp8266_monitor/README.md) is a microcontroller project that runs on an esp8266 nodemcu 1.0 for measuring the water depth of a watertank and posting it to a REST API server.

> **esp8266_config** is a microcontroller project that stores configuration information in an esp2866 flash file to configure the monitor app. This project was experimental and eventually not required for the monitor to work.

## Configuration

Open the project with your favourite Arduino IDE. I use Visual Studio Code, but it should work from the default Arduino IDE

Create a file called **configData.h** and include two macros, WIFI_SSID and WIFI_PASSPHRASE like this

```C++
#define WIFI_SSID "MyWifiName"
#define WIFI_PASSPHRASE "MyWifiPassword"
```

Create an account at io.adafruit.com.
Create a file called **configAdafruitMqtt.h** and define the following macros with the appropriate values.

```C++
#define AIO_SERVER      "io.adafruit.com"
#define AIO_USERNAME    "Your Adafruit account name"
#define AIO_KEY         "Your Adafruit key"
#define AIO_SERVERPORT  1883
```

## Wiring

A design and components are available on [EasyEDA](https://easyeda.com/kokuda/watermonitor_copy). You can also order PCBs directly from them from them with this design. Note that the pin usage is slightly different than the code to make the PCB design a bit simpler. The code will be updated with the changes.

![Circuit Diagram](/images/Circuit.png)
![PCB Layout](/images/PCB.png)

[**Download and order the PCB**](https://easyeda.com/kokuda/watermonitor_copy)

## Output

When the data is regularly published to io.adafruit.com, you can create a dashboard of various stats, including water depth, distance from sensor to water, temperature, and humidity.

Here is a 1 week duration depth chart showing the changes in water depth as the tank is filled and drained.
![Example io.adafruit.com Water Depth Chart](/images/Adafruit.png)

## See Also

The original server project [watermonitor_server](https://github.com/kokuda/watermonitor-server). The intention was for the monitor to post to this server, but using the MQTT server at io.adafruit.com was easier. The server may find a dual purpose as a trigger notification from io.adafruit.com to trigger emails when exceptional events occur.
