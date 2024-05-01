//el rele funciona con 5v y gnd. Los pines funcionan pero a veces la señal no llega al rele porque esta mal la protoboard
//no se porque los focos toman el sentido opuesto al indicado, high=focos apagados y low =focos encendidos, sin embargo por lo que recuerdo
// a veces es al reves, o sea que funciona "bien"

/*Inversión de la lógica del relé: Algunos relés activan la carga cuando el pin de control está en LOW y la desactivan cuando está en HIGH. 
Esto puede ser confuso, pero es una característica de algunos tipos de relés. 
Puedes probar a cambiar la lógica en tu código para adaptarla a esta situación.*/

//librarys
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//own files
#include "config.h"
#include "functions.cpp"

//vars & const
const int ledPin = 2;
int lighStartTime = 6;
int lighEndTime = 21;
int pinLigh = 23;
int pinVenti = 22;
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
  pinMode(pinVenti, OUTPUT);

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

  // digitalWrite(pinLigh, HIGH);
  // delay(1000);
  // digitalWrite(pinLigh, LOW);
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
  if (currentHour >= lighStartTime && currentHour <= lighEndTime) {

    hoursRemaining = lighEndTime - currentHour;
    digitalWrite(pinLigh, LOW);
    ventilationSystem(pinVenti, minToMillis(10));

  } else {

    digitalWrite(pinLigh, HIGH);
    digitalWrite(pinVenti, HIGH);

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
      Serial.println("  light ON");
    } else {
      Serial.println("  lightin OFF");
    }

    lastUpdate = millis();
  }
}
//change led states
void toggleLED(int interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  // Cambiar el estado del LED cada cierto intervalo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}
//ventilation system
void ventilationSystem(int pinVenti, unsigned int interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  // Cambiar el estado del venti cada cierto intervalo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(pinVenti, !digitalRead(pinVenti));
  }
}
//minutes to millis
unsigned long minToMillis(int minutos) {return minutos * 60000;}




