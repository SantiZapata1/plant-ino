//librerias
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <DHT.h>

//defines
#define PIN_SENSOR_TIERRA A2
#define PIN_SENSOR_TEMP A5
#define MODELO_DE_SENSOR DHT11

#define PIN_LUZ 2
#define PIN_BOMBA 3

#define TIEMPO_MEDIR_TIERRA 3600000
#define HUM_TIERRA_MIN 40
#define TIEMPO_DE_RIEGO 2000

//variables
int valorMaximo2 = 560;//humedad tierra
int valorMinimo2 = 250;
int valorLeidoHumedad;
int mapeoTierra;

int humedad;//temperatura y humedad
int temperatura;

int hora;
int minutos;

unsigned long tiempoReferencia = 0;//tareas temporales
unsigned long tiempoMillis;

//objetos
ThreeWire myWire(3, 4, 2);
RtcDS1302<ThreeWire> Rtc(myWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(PIN_SENSOR_TEMP, MODELO_DE_SENSOR);

//setup
void setup(){

  //inicializaciones
  dht.begin();
  lcd.init();
  lcd.backlight();
  Rtc.Begin();
  RtcDateTime fechaYHoraDeCompilacion = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(fechaYHoraDeCompilacion);

  pinMode(PIN_BOMBA, OUTPUT);
  pinMode(PIN_LUZ, OUTPUT);

  Serial.begin(115200);
  Serial.println("setup ok");

}

//loop
void loop(){

  //hora
  RtcDateTime tiempoActual = Rtc.GetDateTime();
  hora = tiempoActual.Hour();
  minutos = tiempoActual.Minute();

  //temperatura y humedad
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();

  //ciclos de luz
  /*si esta dentro del horario de iluminacion los focos estaran encendidos, sino permaneceran apagados*/
  if (hora > 8 && hora < 19) {
    digitalWrite(PIN_LUZ, true);
  }else {
    digitalWrite(PIN_LUZ, false);
  }

  //lectura humedad tierra
  medirHumTierra();

  //riego
  activarBomba();

  //pantalla
  lcd.setCursor(0, 0);//fila 1
  lcd.print("t:");
  lcd.print(temperatura);
  lcd.print(" h:");
  lcd.print(humedad);

  lcd.setCursor(0, 1);//fila 2
  lcd.print("ht:");
  lcd.print(mapeoTierra);
  lcd.print("  ");
  lcd.print(hora);
  lcd.print(":");
  lcd.print(minutos);

  //mostrar estado del sistema 
  reporteSistema();
  
}

//procedimiento para mostrar estado del sistema por el monitor serie
void reporteSistema(){

      unsigned long tiempoMilli=millis();
      unsigned long tiempoReferencia=0;

      if (tiempoMillis - tiempoReferencia >= 2000) {
      tiempoReferencia = tiempoMillis;

      Serial.println(" ");
      
      Serial.print("hora: ");
      Serial.print(hora);
      Serial.print(":");
      Serial.println(minutos);

      Serial.print("temperatura: ");
      Serial.print(temperatura);
      Serial.print("º    humedad: ");
      Serial.print(humedad);
      Serial.println("%");

      Serial.print("humedad tierra: ");
      Serial.print(mapeoTierra);
      Serial.println("%");

      Serial.print("sistema de iluminacion: ");
      Serial.println(PIN_LUZ);

      Serial.print("sistema de riego: ");
      Serial.println(PIN_BOMBA);
      
      Serial.println(" ");

  
      }
      
}

//procedimiento que mide la humedad del suelo
/*se lee la humedad del suelo, actualizando los limites de la variable y mapeando el valor en 0-100*/
void medirHumTierra(){

  valorLeidoHumedad = analogRead(PIN_SENSOR_TIERRA);

  if (valorLeidoHumedad < valorMinimo2) {
    valorMinimo2 = valorLeidoHumedad;
  }else if (valorLeidoHumedad > valorMaximo2) {
    valorMaximo2 = valorLeidoHumedad;
  }

  mapeoTierra = map(valorLeidoHumedad, valorMinimo2, valorMaximo2, 100, 0);
  mapeoTierra = constrain(mapeoTierra, 0, 100);

}

//procedimiento para activar bomba de riego
/*cada 1 hora se verifica si la humedad de a tierra esta bien, y si no esta, se riega durante 1s*/
void activarBomba(){

    unsigned long tiempoMilli=millis();
    unsigned long tiempoReferencia=0;

  if (tiempoMillis - tiempoReferencia >= TIEMPO_MEDIR_TIERRA) {

      tiempoReferencia = tiempoMillis;

      if (mapeoTierra < HUM_TIERRA_MIN) {

        digitalWrite(PIN_BOMBA, true);
        delay(TIEMPO_DE_RIEGO);
        digitalWrite(PIN_BOMBA, false);
      }
  }


}













