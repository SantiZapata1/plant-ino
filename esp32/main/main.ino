//librerias
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//archivos extras
#include "config.h" 
#include "functions.cpp"

//parametros sistema de iluminacion
int lighStartTime = 7;
int lighEndTime = 20;
int pinLigh = 2;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  //configuramos zona horaria 
  timeClient.setTimeOffset(-3 * 3600);
  timeClient.begin();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando WiFi");
    delay(1000);
  }

  // Configurar el pin de la iluminacion como salida
  pinMode(pinLigh, OUTPUT);
  digitalWrite(pinLigh, LOW);

}
 
void loop() 
{

  //hora actual
  timeClient.update();
  Serial.println("\ncurrent time arg: "+timeClient.getFormattedTime());

  //llamamos al metodo que administra la iluminacion
  iluminationSystem(pinLigh,lighStartTime,lighEndTime);

}

//si no hay wifi se apagan los focos, y si hay segun el horario se encienden o apagan.
void iluminationSystem(int pinLigh,int lighStartTime,int lighEndTime){
  
  //si no hay wifi se apaga la iluminacion
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado");
    digitalWrite(pinLigh, LOW); 
    delay(60000); // Se reintenta conectar cada 1 min
  }

  // Calcular las horas restantes hasta el próximo cambio de estado
  int currentHour = timeClient.getHours();
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

}