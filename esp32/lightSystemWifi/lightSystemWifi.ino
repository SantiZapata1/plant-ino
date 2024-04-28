//librarys
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//own files
#include "config.h"
#include "functions.cpp"

//vars
const int ledPin = 2;
int lighStartTime = 6;
int lighEndTime = 21;
int pinLigh = 15;
int hoursRemaining;
boolean isConnected = false;


//objet for time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//setup
void setup() {

  //set up pin light as output
  pinMode(ledPin, OUTPUT);
  pinMode(pinLigh, OUTPUT);
  Serial.begin(115200);
  Serial.println("\n\n---- Iniciamos ----\n\n");

  WiFi.begin(ssid, password);
  // WiFi.begin(ssid2, password2);

  timeClient.setTimeOffset(-3 * 3600);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connecting WiFi");
    delay(1000);
  }

  isConnected = true;
  timeClient.begin();
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    // WiFi.begin(ssid2, password2);
    isConnected = false;
    Serial.println("WiFi connection lost");
    delay(1000);
  } else {
    // Serial.println("WiFi Ok again :)");
    isConnected = true;
  }
  if (isConnected) {
    toggleLED(5000);
  } else {
    toggleLED(2000);
  }

  // call to light system
  iluminationSystem(pinLigh, lighStartTime, lighEndTime);
  //call to show light system data
  showLightData(3000);
}

//si no hay wifi se apagan los focos, y si hay segun el horario se encienden o apagan.
void iluminationSystem(int pinLigh, int lighStartTime, int lighEndTime) {

  int currentHour;

  timeClient.update();
  currentHour = timeClient.getHours();


  // Si la hora actual está en el horario de iluminación, se enciende el sistema de iluminación
  if (currentHour >= lighStartTime && currentHour < lighEndTime) {

    hoursRemaining = lighEndTime - currentHour;
    digitalWrite(pinLigh, HIGH);

  } else {

    digitalWrite(pinLigh, LOW);

    if (currentHour >= lighEndTime) {
      hoursRemaining = (24 - currentHour) + lighStartTime;
    } else {
      hoursRemaining = lighStartTime - currentHour;
    }
  }
}
//funcion para mostrar las lecturas por el monitor serial
void showLightData(int delay) {

  int estado = digitalRead(pinLigh);  // Lee el estado del pin

  //hora actual
  timeClient.update();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= delay) {

    // Mostrar las horas restantes
    Serial.print("-time: " + timeClient.getFormattedTime());
    Serial.print("  Remain: ");
    Serial.print(hoursRemaining);

    if (estado == HIGH) {
      Serial.println("  light pin ON");
    } else {
      Serial.println("  light pin OFF");
    }

    lastUpdate = millis();
  }
}
void toggleLED(int interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  // Cambiar el estado del LED cada cierto intervalo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}