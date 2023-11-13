//librerias
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//defines 
#define lighStartTime 7
#define lighEndTime 20

//credenciales WiFi
const char *ssid = "Fibertel WiFi692 2.4GHz";
const char *password = "leticia2020";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Pin para controlar el relé
const int pinLigh = 2;  

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando a WiFi");
    delay(1000);
  }

  timeClient.setTimeOffset(-3 * 3600);
  timeClient.begin();

  // Configurar el pin de la iluminacion como salida
  pinMode(pinLigh, OUTPUT);
  digitalWrite(pinLigh, LOW);
}

void loop() {

  //si no hay wifi se apaga la iluminacion
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado");
    digitalWrite(pinLigh, LOW); 
    delay(60000); // Se reintenta conectar cada 1 min
  }

  timeClient.update();
  Serial.println("\ncurrent time arg: "+timeClient.getFormattedTime());

  int currentHour = timeClient.getHours();

  // Calcular las horas restantes hasta el próximo cambio de estado
  int hoursRemaining;

    // Si la hora actual está en el horario de iluminación, se enciende el sistema de iluminación
  if (currentHour >= lighStartTime && currentHour < lighEndTime) {
      Serial.println("ligh ON");
      hoursRemaining = lighEndTime - currentHour;
      digitalWrite(pinLigh, HIGH);

  }else{
      
    Serial.println("ligh OFF");
    digitalWrite(pinLigh, LOW);

    if (currentHour >= lighEndTime) {
      hoursRemaining = (24 - currentHour) + lighStartTime;
    } 
    else {
      hoursRemaining = lighStartTime - currentHour;
    }
  } 

  // Mostrar las horas restantes
  Serial.print("Hours Remaining: ");
  Serial.println(hoursRemaining);

  delay(1000); // Espera 1 segundo
}
