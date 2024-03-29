![Water Tank Diagram](/images/WaterTankDiagram.png)

# Watermonitor Controller

Well water pumphouse esp8266 nodemcu 1.0 microcontroller project for monitoring the depth of a water tank and room temperature, reporting to the [io.adafruit.com](https://io.adafruit.com/).

## Installation

Dependencies required to build this...

### Boards

[Configure Arduino IDE for esp8266](https://www.make-it.ca/setup-nodemcu-arduino-ide/)

Install the esp8266 boards through the Arduino IDE APP

    Tools->Board->Board Manager

Search for "esp8266"

### Libraries

    Adafruit Unified Sensor by Adafruit (Tested with 1.1.9)
    DHT sensor library by Adafruit (Tested with 1.4.4)
    ArduinoJson by Benoit Blanchon (Tested with 6.21.2)
    Adafruit MQTT Library by Adafruit (Tested with 2.5.4)

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

## Final Result

![Final Water Tank Monitor Device](images/FinalBox.jpg)

## See Also

The original server project [watermonitor_server](https://github.com/kokuda/watermonitor-server). The intention was for the monitor to post to this server, but using the MQTT server at io.adafruit.com was easier. The server may find a dual purpose as a trigger notification from io.adafruit.com to trigger emails when exceptional events occur.
