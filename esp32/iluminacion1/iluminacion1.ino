//librerias
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


//credenciales WiFi
const char *ssid = "Fibertel WiFi692 2.4GHz";
const char *password = "leticia2020";

int lighStartTime = 7;
int lighEndTime = 20;


unsigned long tiempoAnterior = 0;  // Variable para almacenar el último momento en que se cambió el estado del relé
bool estadoRele = false;           // Variable para almacenar el estado actual del relé


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Pin para controlar el relé
const int pinLigh = 25;
const int pinFan = 26;

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  timeClient.setTimeOffset(-3 * 3600);
  timeClient.begin();

  // Configurar el pin de la iluminacion como salida
  pinMode(pinLigh, OUTPUT);
  pinMode(pinFan, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {

  //si no hay wifi se apaga la iluminacion
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado");
    digitalWrite(pinLigh, LOW);
    digitalWrite(2, HIGH);

    delay(60000);  // Se reintenta conectar cada 1 min
  } else {
    digitalWrite(2, LOW);
  }

  timeClient.update();
  Serial.println("\ncurrent time arg: " + timeClient.getFormattedTime());

  int currentHour = timeClient.getHours();

  // Calcular las horas restantes hasta el próximo cambio de estado
  int hoursRemaining;

  // Si la hora actual está en el horario de iluminación, se enciende el sistema de iluminación
  if (currentHour >= lighStartTime && currentHour < lighEndTime) {
    Serial.println("ligh ON");
    hoursRemaining = lighEndTime - currentHour;
    digitalWrite(pinLigh, HIGH);

  } else {

    Serial.println("ligh OFF");
    digitalWrite(pinLigh, LOW);

    if (currentHour >= lighEndTime) {
      hoursRemaining = (24 - currentHour) + lighStartTime;
    } else {
      hoursRemaining = lighStartTime - currentHour;
    }
  }

  // Mostrar las horas restantes
  Serial.print("Hours Remaining: ");
  Serial.println(hoursRemaining);

  unsigned long tiempoActual = millis();

  // Verificar si ha pasado el intervalo de tiempo para cambiar el estado del relé
  if (tiempoActual - tiempoAnterior >= 600000) {
    tiempoAnterior = tiempoActual;
    estadoRele = !estadoRele;                       // Cambiar el estado del relé
    digitalWrite(pinFan, estadoRele ? HIGH : LOW);  // Encender o apagar el relé según el estado actual
  }
  Serial.print("fan: ");
  Serial.println(estadoRele);

  delay(1000);  // Espera 1 segundo
}
