#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
// Es una librería que te permite gestionar la conexión WiFi en tu dispositivo ESP32 de forma sencilla.
// Permite configurar la conexión WiFi de forma dinámica, sin tener que escribir los datos de conexión en el código.
// También puede gestionar la reconexión automática en caso de pérdida de conexión.

#include <HTTPClient.h>
// Esta librería te permite realizar solicitudes HTTP desde tu ESP32, lo que te permite comunicarte con servidores web y servicios en la nube.
// Puedes enviar solicitudes GET, POST, PUT, DELETE, etc., y recibir y procesar respuestas HTTP.

#include "DHT.h"
// Es una librería que te permite interactuar con sensores de temperatura y humedad DHT11, DHT21 y DHT22.
// Te permite leer los valores de temperatura y humedad del sensor y utilizarlos en tu código.

#include <string>
// string proporciona una clase para representar cadenas de texto de forma segura.

#include <sstream>
// sstream proporciona clases y funciones para realizar operaciones de entrada/salida en cadenas de texto.

#include <string.h>
// string.h proporciona funciones para manipular cadenas de texto a nivel de caracteres.

// Sound speed in air
#define SOUND_SPEED 340
#define TRIG_PULSE_DURATION_US 10
#define DHTTYPE DHT22   // DHT 22  (AM2302)

long ultrason_duration;

// no se que es esto
// ID: 1 , securitybot (en rcia)
// ID: 2 , fat semillero (en greenhouse)
// ID: 3 , skinny outdoor 
using namespace std;
const char* serverName = "http://52.67.123.15/api";
// @TODO add: userId 659ae411529f6a35572f6831\", \"contrId to variables so I set this up in a single place all together

// variables of the plant
int tempeture = 0;
int airHumidity = 0;
const int airHumidityPin = 5; 

// current sensor variables
const int currentSensorPin = 99;

// light sensor variables
// used to be 34, check again wichon e use
#define light_sensor_ping 34
const int dark = 4095;
const int bright = 280;
int light = 0;

// distance/capacity sensor variables
// supongo que ultrasonido
float duration_us, distance_cm;
 
// soil humidity sensor variables
#define sensor_pin_for_soil_humidity1 36
#define sensor_pin_for_soil_humidity2 39
// we set one as default
int minHumiditySetted = 40;
int soilHumidity1 = 0;
int soilHumidity2 = 0;
const int wet1 = 4095;
const int dry1 = 1840;
const int wet2 = 3047;
const int dry2 = 983;
// const int dry1 = 2960;
// const int wet1 = 1105;

// relay variables
const int relayOnePin = 26; //P26
const int relayTwoPin = 25; // P25
const int relayThreePin = 33; //P33
const int relayFourPin = 32;  //P32
bool isRelayOneOn = false; 
bool isRelayTwoOn = false;
bool isRelayThirdOn = false;
bool isRelayFourthOn = false;

// motion detection sensor variables
const int buzzerSensorPin = 12; //P35
const int motionSensorPin = 13; //P35
int motionPinStateCurrent   = LOW;  // current state of pin
int motionPinStatePrevious  = LOW;  // previous state of pin
bool alarmOn = false; 

// button for resetting wifi variables
const int resetWifiPin = 35;
int wifiButtonLastState = LOW;
int wifiButtonCurrentState = LOW;

// distance sensor variables
const int distanceSensorOneTriggerPin = 27;  
const int distanceSensorOneEchoPin  = 14;

// distance sensor variables
const int distanceSensorTwoTriggerPin = 23;  
const int distanceSensorTwoEchoPin  = 22;

const int phPin = 18;  

// Actualizaciones de estado de los reles
void handleRaleyActions() {
    if (isRelayOneOn) { digitalWrite(relayOnePin, LOW);Serial.println("raley one ON"); } else { digitalWrite(relayOnePin, HIGH);Serial.println("raley one OFF"); };
    if (isRelayTwoOn) { digitalWrite(relayTwoPin, LOW);Serial.println("raley two ON"); } else { digitalWrite(relayTwoPin, HIGH);Serial.println("raley two OFF"); };
    if (isRelayThirdOn) { digitalWrite(relayThreePin, LOW);Serial.println("raley three ON"); } else { digitalWrite(relayThreePin, HIGH);Serial.println("raley three OFF"); };
    if (isRelayFourthOn) { digitalWrite(relayFourPin, LOW);Serial.println("raley four ON"); } else { digitalWrite(relayFourPin, HIGH);Serial.println("raley four OFF"); };
}

// activar o desactivar la bomba de riego segun el limite establecido
void updateRelaysStateWithLastConfig() {
  // if (oldConfig)
    isRelayOneOn = soilHumidity1 > minHumiditySetted ? false : true;
    isRelayTwoOn = soilHumidity2 > minHumiditySetted ? false : true;
    // isRelayThirdOn = soilHumidity1 > minHumiditySetted ? false : true;
    // isRelayFourthOn = soilHumidity1 > minHumiditySetted ? false : true;

    // Serial.print("raleys state after OFFLINE local update:");
    // Serial.print("isRelayOneOn:");Serial.println(isRelayOneOn);
    // Serial.print("isRelayTwoOn:");Serial.println(isRelayTwoOn);
    // Serial.print("isRelayThirdOn:");Serial.println(isRelayThirdOn);
    // Serial.print("isRelayFourthOn:");Serial.println(isRelayFourthOn);
}

// estudiar este, pero creo que se trata de actualizar los datos de los sensores en la pagina y hacer los cambios en el sistema
void updateServerAndRelaysState() {
    WiFiClient client;
    HTTPClient http;    
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // we convert here three variables from int to const char
    int distance = (int)distance_cm;
    stringstream strs1;
    stringstream strs2;
    stringstream strs3;
    stringstream strs4;
    stringstream strs5;
    stringstream strs6;
    strs1 << soilHumidity1;
    strs2 << airHumidity;
    strs3 << tempeture;
    strs4 << distance;
    strs5 << soilHumidity2;
    strs6 << light;
    string temp_str1 = strs1.str();
    string temp_str2 = strs2.str();
    string temp_str3 = strs3.str();
    string temp_str4 = strs4.str();
    string temp_str5 = strs5.str();
    string temp_str6 = strs6.str();
    
    const char* addSoilHumidity1 = (char*) temp_str1.c_str();
    const char* addAirHumidity = (char*) temp_str2.c_str();
    const char* addTempeture = (char*) temp_str3.c_str();
    const char* addDistance = (char*) temp_str4.c_str();
    const char* addSoilHumidity2 = (char*) temp_str5.c_str();
    const char* addLight = (char*) temp_str6.c_str();
    // end converting

    // now we put everything together
    char queryString[950];
    const char *first = "{\"operationName\": \"UpdatePlant\",\"variables\":{\"id\": \"659ae411529f6a35572f6831\", \"contrId\": 3, \"hum1\": ";
    const char *secon = ", \"airHum\": ";
    const char *third = ", \"temp\": ";
    const char *fourth = ", \"dist\": ";
    const char *fifth = ", \"hum2\": ";
    const char *sixth = ", \"light\": ";
    const char *sixthAndhalf = alarmOn ? ", \"alarm\": true" : ", \"alarm\": false";
    const char *seventh = isRelayOneOn ? ", \"isRelayOneOn\": true" : ", \"isRelayOneOn\": false";
    const char *eighth = isRelayTwoOn ? ", \"isRelayTwoOn\": true" : ", \"isRelayTwoOn\": false";
    const char *nineth = isRelayThirdOn ? ", \"isRelayThirdOn\": true" : ", \"isRelayThirdOn\": false";
    const char *tenth = isRelayFourthOn ? ", \"isRelayFourthOn\": true" : ", \"isRelayFourthOn\": false";
    const char *last= "},\"query\":\"mutation UpdatePlant($id: ID!, $contrId: Int!, $hum1: Int, $airHum: Int, $temp: Int, $dist: Int, $hum2: Int, $light: Int, $alarm: Boolean, $isRelayOneOn: Boolean, $isRelayTwoOn: Boolean, $isRelayThirdOn: Boolean, $isRelayFourthOn: Boolean) { updatePlant(id: $id, contrId: $contrId, hum1: $hum1, airHum: $airHum, temp: $temp, dist: $dist, hum2: $hum2, light: $light, alarm: $alarm, isRelayOneOn: $isRelayOneOn, isRelayTwoOn: $isRelayTwoOn, isRelayThirdOn: $isRelayThirdOn, isRelayFourthOn: $isRelayFourthOn) { isRelayOneOn, isRelayTwoOn, isRelayThirdOn, isRelayFourthOn }}\"}";
    
    strcpy(queryString,first);
    strcat(queryString,addSoilHumidity1);
    strcat(queryString,secon);
    strcat(queryString,addAirHumidity);
    strcat(queryString,third);
    strcat(queryString,addTempeture);
    strcat(queryString,fourth);
    strcat(queryString,addDistance);
    strcat(queryString,fifth);
    strcat(queryString,addSoilHumidity2);
    strcat(queryString,sixth);
    strcat(queryString,addLight);
    strcat(queryString,sixthAndhalf);
    strcat(queryString,seventh);
    strcat(queryString,eighth);
    strcat(queryString,nineth);
    strcat(queryString,tenth);
    strcat(queryString,last);
    
    Serial.println(queryString);

    // this is how the body looks like
    // {"operationName": "UpdatePlant","variables":{"id": "64558a8356b560e1c8172407", "controllerId": 20, "soilHumidity": 42, "airHumidity": 0, "tempeture": 0, "isRelayOneOn": false, "isRelayTwoOn": false, "isRelayThirdOn": false, "isRelayFourthOn": false},"query":"mutation UpdatePlant($id: ID!, $controllerId: Int!, $soilHumidity: Int, $airHumidity: Int, $tempeture: Int, $isRelayOneOn: Boolean, $isRelayTwoOn: Boolean, $isRelayThirdOn: Boolean, $isRelayFourthOn: Boolean) { updatePlant(id: $id, controllerId: $controllerId, soilHumidity: $soilHumidity, airHumidity: $airHumidity, tempeture: $tempeture, isRelayOneOn: $isRelayOneOn, isRelayTwoOn: $isRelayTwoOn, isRelayThirdOn: $isRelayThirdOn, isRelayFourthOn: $isRelayFourthOn) { isRelayOneOn, isRelayTwoOn, isRelayThirdOn, isRelayFourthOn }}"}
    const char* body = queryString;

    int httpResponseCode = http.POST(body);
   
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);   
    String payload = http.getString();

    // this is how the payload looks like:
    //{"data":{"updatePlant":{"isRelayOneOn":"OF","isRelayTwoOn":"OF","isRelayThirdOn":"OF","isRelayFourthOn":"ON"}}}
    
    // Serial.print("payload");
    // Serial.println(payload);


    if (payload.substring(40, 42) == "ON") { isRelayOneOn = true;} else { isRelayOneOn = false; };
    if (payload.substring(60, 62) == "ON") { isRelayTwoOn = true;} else { isRelayTwoOn = false; };
    if (payload.substring(82, 84) == "ON") { isRelayThirdOn = true; } else { isRelayThirdOn = false; };
    if (payload.substring(105, 107) == "ON") { isRelayFourthOn = true;} else { isRelayFourthOn = false; };

    // Free resources
    http.end();
}


void readSoilHumidity1() {
    int rawHumidity = analogRead(sensor_pin_for_soil_humidity1);
    soilHumidity1 = map(rawHumidity, dry1, wet1, 100, 0);
    Serial.print("SoilHumidity 1 = ");
    Serial.print(soilHumidity1);  /* Print Temperature on the serial window */
    Serial.println("%");

    // Serial.print("rawHumidity 1 = ");
    // Serial.print(rawHumidity);  /* Print Temperature on the serial window */
}

//leer humedad de suelo
void readSoilHumidity2() {
    int rawHumidity2 = analogRead(sensor_pin_for_soil_humidity2);
    soilHumidity2 = map(rawHumidity2, dry2, wet2, 100, 0);
    Serial.print("SoilHumidity 2 = ");
    Serial.print(soilHumidity2);  /* Print Temperature on the serial window */
    Serial.println("%");

    // Serial.print("rawHumidity 2 = ");
    // Serial.print(rawHumidity2);  /* Print Temperature on the serial window */
}

// Leer sensores ultrasonicos
void readDistanceSensorOne() {
  digitalWrite(distanceSensorOneTriggerPin, LOW);
  delayMicroseconds(2);
 // Create a 10 µs impulse
  digitalWrite(distanceSensorOneTriggerPin, HIGH);
  delayMicroseconds(TRIG_PULSE_DURATION_US);
  digitalWrite(distanceSensorOneTriggerPin, LOW);
  // Return the wave propagation time (in µs)
  ultrason_duration = pulseIn(distanceSensorOneEchoPin, HIGH);
  //distance calculation
  distance_cm = ultrason_duration * SOUND_SPEED/2 * 0.0001;
  // Serial.print("Distance (cm): ");
  // Serial.println(distance_cm);
}
void readDistanceSensorTwo() {
  digitalWrite(distanceSensorTwoTriggerPin, LOW);
  delayMicroseconds(2);
 // Create a 10 µs impulse
  digitalWrite(distanceSensorTwoTriggerPin, HIGH);
  delayMicroseconds(TRIG_PULSE_DURATION_US);
  digitalWrite(distanceSensorTwoTriggerPin, LOW);
  // Return the wave propagation time (in µs)
  ultrason_duration = pulseIn(distanceSensorTwoEchoPin, HIGH);
  //distance calculation
  distance_cm = ultrason_duration * SOUND_SPEED/2 * 0.0001;
  // Serial.print("Distance (cm): ");
  // Serial.println(distance_cm);
}

// Leer sensores de humedad y temperatura
void readAirHumidityAndTempeture() {

  DHT dht(airHumidityPin, DHTTYPE);
  dht.begin();
  airHumidity = dht.readHumidity();
  // Read temperature as Celsius
  tempeture = dht.readTemperature();

    Serial.print("airHumidity: ");
   Serial.print(airHumidity);
   Serial.println(" %");

   Serial.print("tempeture: ");
   Serial.print(tempeture);
   Serial.println(" *");
}

void readAirHumidity() {
    // define this method
}

// lectura de iluminacion
void readLight() {
   int rawLightValue = analogRead(light_sensor_ping);
   light = map(rawLightValue, bright, dark, 100, 0);
    // Serial.print("Light reading = ");
    // Serial.print(light);
    // Serial.print("rawLightValue reading = ");
    // Serial.print(rawLightValue);
}

// supongo que esto es de la seguridad
void readCurrentSensor() {

  int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
  int Watt = 0;
  double Voltage = 0;
  double VRMS = 0;
  double AmpsRMS = 0;

  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3; //0.3 is the error I got for my sensor
 
  Serial.print(AmpsRMS);
  Serial.print(" Amps RMS  ---  ");
  Watt = (AmpsRMS*110/1.2);
  // note: 1.2 is my own empirically established calibration factor
// as the voltage measured at D34 depends on the length of the OUT-to-D34 wire
// 240 is the main AC power voltage – this parameter changes locally
  Serial.print(Watt);
  Serial.println(" Watts");

  if (Watt > 4) {
    Serial.println("Motion extension detected!");
    alarmOn = true;
    digitalWrite(buzzerSensorPin, HIGH); // turn on
  }
  else {
    Serial.println("Motion extension stopped!");
    digitalWrite(buzzerSensorPin, LOW); // turn on
    alarmOn = false;
  }
}

float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(currentSensorPin);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 3.3)/4096.0; //ESP32 ADC resolution 4096
      
   return result;
 }

void readMotionSensor() {
  motionPinStatePrevious = motionPinStateCurrent; // store old state
  motionPinStateCurrent = digitalRead(motionSensorPin);   // read new state

  if (motionPinStatePrevious == LOW && motionPinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");
    alarmOn = true;
    digitalWrite(buzzerSensorPin, HIGH); // turn on
  }
  else if (motionPinStatePrevious == HIGH && motionPinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");
    digitalWrite(buzzerSensorPin, LOW); // turn on
    alarmOn = false;
  }
}

// Lectura sensor de PH del suelo
void phSensor() {
    float ph;
    float Value=0;
    Value= analogRead(phPin);
    Serial.print(Value);
    Serial.print(" | ");
    float voltage=Value*(3.3/4095.0);
    ph=(3.3*voltage);
    Serial.println(ph);
    delay(500);
}

// Funcion del boton para resetear
void checkWifiResetButtonState() {
  wifiButtonCurrentState = digitalRead(resetWifiPin);
  
  if(wifiButtonLastState == LOW && wifiButtonCurrentState == HIGH) {
    Serial.println("Reseting wifi");
    wifiButtonLastState = wifiButtonCurrentState;
    WiFi.mode(WIFI_STA); //Optional
    WiFiManager wm;
    bool res;

    wm.resetSettings();

    res = wm.autoConnect("DittoBot WiFi", "password"); // password protected ap

    while(!res) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
  }
}


void wifiSetupStuff() {
    WiFi.mode(WIFI_STA); //Optional
    WiFiManager wm;

    // Supress Debug information
    // wm.setDebugOutput(true);
    bool res;
    res = wm.autoConnect("DittoBot WiFi", "password"); // password protected ap

    while(!res) {
        Serial.print(".");
        delay(100);
    }
    
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}
// Void setup
void setup() {
    Serial.begin(115200);
    wifiSetupStuff();
    // relays pins
    pinMode(relayOnePin, OUTPUT);
    pinMode(relayTwoPin, OUTPUT);
    pinMode(relayThreePin, OUTPUT);
    pinMode(relayFourPin, OUTPUT);
    pinMode(motionSensorPin, INPUT);
    pinMode(phPin, INPUT);

    // configure the trigger and echo pins to output and input mode
    pinMode(distanceSensorOneTriggerPin, OUTPUT);
    pinMode(distanceSensorOneEchoPin, INPUT);
    pinMode(resetWifiPin, INPUT_PULLUP);

     pinMode(buzzerSensorPin, OUTPUT);   
}

// void loop
void loop() { 
  
    readSoilHumidity1();
    readSoilHumidity2();
    readLight();
    readDistanceSensorOne();
    checkWifiResetButtonState();
    readAirHumidityAndTempeture();
    readMotionSensor();
    readCurrentSensor();
    phSensor();
    
    // Check WiFi connection status, if no conection, we do NOTHING
    if(WiFi.status() != WL_CONNECTED) {
      // updateRelaysStateWithLastConfig();
    } else {
      updateServerAndRelaysState();
    }
    handleRaleyActions();

    // we update every 5 secs just for now
    delay(2000);

}