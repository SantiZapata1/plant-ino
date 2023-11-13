#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "Fibertel WiFi692 2.4GHz";
const char *password = "leticia2020";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Pin para controlar el relé
const int pinRelay = 2;  // Asígnalo al pin que estás utilizando

// Configuración para la pantalla LCD
const int I2C_ADDR = 0x27; // Dirección I2C de la pantalla
const int LCD_COLS = 16;   // Número de columnas en la pantalla
const int LCD_ROWS = 2;    // Número de filas en la pantalla
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  int cont = 0;
  while (WiFi.status() != WL_CONNECTED) {
    cont++;
    delay(1000);
    Serial.print(cont);
    Serial.println("- Conectando a WiFi...");
  }

  timeClient.setTimeOffset(-3 * 3600);
  timeClient.begin();

  // Configurar el pin del relé como salida
  pinMode(pinRelay, OUTPUT);
  // Inicialmente apagar el foco (o relé)
  digitalWrite(pinRelay, LOW);

  // Inicializar la pantalla LCD
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.backlight(); // Encender la retroiluminación
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("WiFi Desconectado");
    Serial.println("WiFi desconectado");
    lcd.setCursor(0, 1);
    lcd.print("Apagando el foco");
    digitalWrite(pinRelay, LOW); // Apagar el relé por precaución
    delay(60000); // Esperar 1 minuto antes de intentar reconectar
    return;
  }

  timeClient.update();
  Serial.println(timeClient.getFormattedTime());

  int currentHour = timeClient.getHours();

  // Calcular las horas restantes hasta el próximo cambio de estado
  int hoursRemaining = 0;

  if (currentHour >= 8 && currentHour < 20) {
    lcd.clear();
    lcd.print("Encendiendo foco");
    Serial.println("Iluminacion encendida");
    hoursRemaining = 20 - currentHour;
    digitalWrite(pinRelay, HIGH); // Encender el relé
  } else {
    lcd.clear();
    lcd.print("Apagando foco");
    Serial.println("Iluminacion apagada");
    hoursRemaining = 8 - currentHour;
    digitalWrite(pinRelay, LOW); // Apagar el relé
  }

  // Mostrar las horas restantes en la segunda línea
  lcd.setCursor(0, 1);
  lcd.print("Horas restantes: ");
  lcd.print(hoursRemaining);
  Serial.print("Horas restantes: ");
  Serial.println(hoursRemaining);


  delay(1000); // Espera 1 segundo
}
