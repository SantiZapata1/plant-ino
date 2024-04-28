/*POSIBLE ERROR: al conectar el esp32 con el sensor de humedad puede que este lea 0%, lo que hay que hacer es sacar o ponerlo en la tierra
para que detecte otra lectura y arranque a funcionar correctamente

TENER EN CUENTA: las lecturas del sensor varian segun que tanto lo metamos en la tierra.

Feature: medir cuanto tarda desde que esta seca la tierra hasta que se estabilicen las lecturas despues de regar


*/
//library
#include <Arduino.h>

//vars & const
const int sensorPinSoilMeasure = 34;  // Pin al que está conectado el sensor de humedad de suelo
const int ledSoilHumidity = 2;        // Pin del LED interno de la placa ESP32

//tener en cuenta que este valor depende de cuanto metamos el sensor en la tierra, y del lugar en donde lo pongamos, porque
//mientras mas al costado este el sensor menor va a ser la lectura de humedad
const int humidityLimit = 35;  // Humedad de suelo límite

int soilHumidity = 0;  // Variable para almacenar la lectura de humedad de suelo
bool ledOn = false;    // Estado actual del LED

unsigned long previousMillis;
int interval;
int minValue = 250;
int maxValue = 560;
int contShowData=0;
//setup
void setup() {
  pinMode(sensorPinSoilMeasure, INPUT);
  pinMode(ledSoilHumidity, OUTPUT);
  Serial.begin(115200);
}

//loop
void loop() {

  //read soil humidity
  soilHumidity = readSoilHumidity(sensorPinSoilMeasure);
  //show data
  showSoilHumidityData(soilHumidity, 5000, contShowData);
  //control led
  controlRhythmLed(soilHumidity, humidityLimit, ledSoilHumidity, ledOn, previousMillis, interval);
}

//function for read the soil humidity
int readSoilHumidity(int sensorPinSoilMeasure) {

  int readValue, mappingValue;
  

  //lectura del sensor de humedad de tierra
  readValue = analogRead(sensorPinSoilMeasure);
  if (readValue < minValue) {
    minValue = readValue;
  }
  if (readValue > maxValue) {
    maxValue = readValue;
  }
  mappingValue = map(readValue, minValue, maxValue, 100, 0);
  mappingValue = constrain(mappingValue, 0, 100);

  return mappingValue;
}
//funcion para mostrar las lecturas por el monitor serial
void showSoilHumidityData(int& soilHumidity, int delay, int& contShowData) {
  // Imprimir la humedad actual cada 3 segundos
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= delay) {
    contShowData++;
    // Serial.print("lectura:");Serial.println(valorLeidoHumedad);
    Serial.print(contShowData);

    Serial.print("_ Soil Humidity: ");
    Serial.print(soilHumidity);
    Serial.println("%");
    lastUpdate = millis();
  }
}
// procedure for control led
void controlRhythmLed(int soilHumidity, int humidityLimit, int ledPin, bool& ledOn, unsigned long& previousMillis, int& interval) {
  unsigned long currentMillis = millis();

  // Controlar el LED según la humedad del suelo
  if (soilHumidity < humidityLimit && soilHumidity > 0) {
    interval = 1000;  // Tiempo de titilación rápido
  } else if (soilHumidity >= humidityLimit) {
    interval = 5000;  // Tiempo de titilación lento
  } else if (soilHumidity <= 0) {
    interval = 100;  // Tiempo de titilación lento
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Invertir el estado del LED
    digitalWrite(ledPin, !digitalRead(ledPin));
    ledOn = !ledOn;
  }
}
