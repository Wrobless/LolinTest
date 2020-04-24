#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "ESPAsyncWebServer.h"
#include "WiFi.h"

#define INPUTPIN 27
#define DHTTYPE DHT11

const char *ssid = "CellSpot_2.4GHz_DF28";
const char *password = "Wrobeltoziomal";
const long interval = 10000;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h2 { font-size: 3.0rem; }
      p { font-size: 3.0rem; }
      .units { font-size: 1.2rem; }
      .ds-labels {
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
      }
    </style>
  </head>
  <body>
    <h2>ESP8266 Server</h2>
    <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
      <span class="ds-labels">Temperature</span> 
      <span id="temperature">%TEMPERATURE%</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      <i class="fas fa-tint" style="color:#00add6;"></i> 
      <span class="dht-labels">Humidity</span>
      <span id="humidity">%HUMIDITY%</span>
      <sup class="units">%</sup>
    </p>
  </body>
  <script>
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("temperature").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/temperature", true);
      xhttp.send();
    }, 10000);
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("humidity").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/humidity", true);
      xhttp.send();
    }, 10000 ) ;
  </script>
</html>
)rawliteral";

DHT dht(INPUTPIN, DHTTYPE);

AsyncWebServer server(80);

String readTemperature()
{
    float temperature = 0.0;
    temperature = dht.readTemperature();

    if (isnan(temperature))
    {
        Serial.println("Failed to read from sensor!");
        return "--";
    }
    else
    {
        Serial.print("Temperature: ");
        Serial.println(temperature);
    }
    return String(temperature);
}

String readHumidity()
{
    float humidity = 0.0;
    humidity = dht.readHumidity();

    if (isnan(humidity))
    {
        Serial.println("Failed to read from sensor!");
        return "--";
    }
    else
    {
        Serial.print("Humidity: ");
        Serial.println(humidity);
    }
    return String(humidity);
}

String processor(const String &var)
{
    if (var == "TEMPERATURE")
    {
        return readTemperature();
    }
    else if (var == "HUMIDITY")
    {
        return readHumidity();
    }
    return String();
}

void setup()
{
    Serial.begin(115200);
    dht.begin();
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readTemperature().c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readHumidity().c_str());
    });
    server.begin();
}

void loop()
{
    String temperature = readTemperature();
    String humidity = readHumidity();
    delay(interval);
}
