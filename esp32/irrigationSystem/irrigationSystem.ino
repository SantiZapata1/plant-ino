//defines
int pinGroungSensor = A0;
int pinPumpIrrigation = 13;

//vars to ground measure
int humGround;
int limHumGround = 30;

unsigned long timeTryIrrigation;
unsigned long timeIrrigate;

unsigned long timeRefSystemReport = 0;
int cont=0;


//setup
void setup() {

  Serial.begin(115200);
  pinMode(pinPumpIrrigation, OUTPUT);

  Serial.println("--------- setup ok ------------ "); 

}

//loop
void loop() {

  //asignations
  timeTryIrrigation = minToMillis(30);
  timeIrrigate = secToMillis(5);

  //read grund humidity
  humGround = measureGround(pinGroungSensor);

  //hum actual - limite minimo - cada cuanto tiempo intentara - tiempo de riego - pin de bomba
  irrigationSystem(humGround, limHumGround, timeTryIrrigation, timeIrrigate, pinPumpIrrigation);

  //mostrar estado del sistema
  systemReport(secToMillis(1));
}


//procedimiento para mostrar estado del sistema por el monitor serie
void systemReport(unsigned long timeReport) {

  unsigned long timeActual = millis();

  if (timeActual - timeRefSystemReport >= timeReport) {
    timeRefSystemReport = timeActual;

    cont=cont+1;

    Serial.println(" "); 
    Serial.println(cont);

    /*
    Serial.println(timeReport);
    Serial.println(timeRefSystemReport);
    Serial.println(timeActual);*/


    Serial.print("Ground humidity:");
    Serial.println(humGround);

    Serial.print("Limit ground humidity:");
    Serial.println(limHumGround);

    Serial.print("pinPump:");
    Serial.println(pinPumpIrrigation);

    Serial.print("time irrigate:");
    Serial.println(timeIrrigate);

    Serial.print("time to try irrigation:");
    Serial.println(timeTryIrrigation);

    Serial.println(" ");
  }
}

//procedimiento que mide la humedad del suelo
/*se lee la humedad del suelo, actualizando los limites de la variable y mapeando el valor en 0-100*/
int measureGround(int pinSensor) {

  int readValueHumidity = analogRead(pinSensor);
  int valueMin = 250;
  int valueMax = 560;

  //ajuste re rangos
  if (readValueHumidity < valueMin) {
    valueMin = readValueHumidity;
  } else if (readValueHumidity > valueMax) {
    valueMax = readValueHumidity;
  }

  int mapGroundHumidity = map(readValueHumidity, valueMin, valueMax, 100, 0);
  mapGroundHumidity = constrain(mapGroundHumidity, 0, 100);

  return mapGroundHumidity;
}

//procedure of system irrigation
/*cada x tiempo se verifica si la humedad de la tierra esta bien, y si no esta, se riega durante 1s*/
void irrigationSystem(int humGround, int groudHumMin, int timeTry, int timeIrrigation, int pinPump) {

  unsigned long timeAcPump = millis();
  unsigned long timeRefPump = 0;

  if (timeAcPump - timeRefPump >= timeTry) {

    timeRefPump = timeAcPump;

    if (humGround < groudHumMin) {

      digitalWrite(pinPump, true);
      delay(timeIrrigation);
      digitalWrite(pinPump, false);
    }
  }
}

//funciones para pasar de unidades de tiempo
unsigned long secToMillis(int seconds) {
  unsigned long millis = seconds * 1000;
  return millis;
}

unsigned long minToMillis(int minutes) {
  unsigned long millis = minutes*60000 ;
  return millis;
}
