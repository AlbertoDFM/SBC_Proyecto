#include "DHT.h"
#include <WiFi.h>
#include <ThingsBoard.h>

#define WIFI_AP "JAZZTEL_114A"
#define WIFI_PASSWORD "26b19bd303332d83beb0"

#define TOKEN "nKOiE72ozR4HiMqKfZtT"

// DHT
#define DHTPIN 26
#define DHTTYPE DHT22

char thingsboardServer[] = "iot.etsisi.upm.es";

WiFiClient wifiClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{ pinMode(27,OUTPUT);
  Serial.begin(115200);
  dht.begin();
  delay(10);
  InitWiFi();
  lastSend = 0;
}

void loop()
{
  //if ( !tb.connected() ) {
  //  reconnect();
  //}

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  // tb.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Humedad y temperatura 
  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");

  tb.sendTelemetryFloat("temperature", temperature);
  tb.sendTelemetryFloat("humidity", humidity);

  //Nivel agua
  int sensorAgua = analogRead(32);
  sensorAgua = map(sensorAgua,0,4095,100,0);
  Serial.print("Water level: ");
  Serial.println(sensorAgua);
  tb.sendTelemetryFloat("Water_Level", sensorAgua);


  //Light sensor
  int sensorLuz = analogRead(33);
  sensorLuz = map(sensorLuz,0,4095,0,100);
  Serial.print("Light Sensor : ");
  Serial.println(sensorLuz);
  tb.sendTelemetryFloat("Light_Sensor", sensorLuz); 
  
  //Humedad Tierra
  int sensorTierra = analogRead(34);
  int asensorTierra = sensorTierra;
  sensorTierra = map(sensorTierra,0,4095,0,100);
  Serial.print("Soil-moisture: ");
  Serial.println(sensorTierra);
  tb.sendTelemetryFloat("Soil_moisture", sensorTierra); 

   //Sonda Tierra 2
  int sensorTierra2 = analogRead(25);
  sensorTierra2 = map(sensorTierra2,0,4095,0,100);
  Serial.print("Capacitive Moisture Sensor : ");
  Serial.println(sensorTierra2);
  tb.sendTelemetryFloat("Moisture_Sensor", sensorTierra2); 

  //relé
  if (asensorTierra<3000) {
    digitalWrite(27,HIGH);
  }

  else digitalWrite(27,LOW);
  
  delay(100); 
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
