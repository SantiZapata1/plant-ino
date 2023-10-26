#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebSrv.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPIFFS.h>

AsyncWebServer server(80);

const char* ssid = "Fibertel WiFi692 2.4GHz";
const char* password = "leticia2020";
const char* PARAM_MESSAGE = "santiago";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Pin para controlar el relé
const int pinRelay = 2;  // Asígnalo al pin que estás utilizando

// Configuración para la pantalla LCD
const int I2C_ADDR = 0x27;  // Dirección I2C de la pantalla
const int LCD_COLS = 16;    // Número de columnas en la pantalla
const int LCD_ROWS = 2;     // Número de filas en la pantalla
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Incluye los manejadores de las páginas web aquí
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Sirve el archivo index.html desde SPIFFS
        request->send(200, "text/plain", "estamos");
    });
    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();

    // El resto del código de setup se mantiene igual
    int cont = 0;
    while (WiFi.status() != WL_CONNECTED) {
        cont++;
        delay(1000);
        Serial.print(cont);
        Serial.println("- Conectando a WiFi...");
    }

    timeClient.setTimeOffset(-3 * 3600);
    timeClient.begin();

    pinMode(pinRelay, OUTPUT);
    digitalWrite(pinRelay, LOW);

    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.backlight();

    if (!SPIFFS.begin(true)) {
        Serial.println("Error al montar SPIFFS");
        return;
    }
}

void loop() {
    // El resto del código loop se mantiene igual
    if (WiFi.status() != WL_CONNECTED) {
        lcd.clear();
        lcd.print("WiFi Desconectado");
        lcd.setCursor(0, 1);
        lcd.print("Apagando el foco");
        digitalWrite(pinRelay, LOW);
        delay(60000);
        return;
    }

    timeClient.update();
    Serial.println(timeClient.getFormattedTime());

    int currentHour = timeClient.getHours();

    int hoursRemaining = 0;

    if (currentHour >= 8 && currentHour < 20) {
        lcd.clear();
        lcd.print("Encendiendo foco");
        Serial.println("Encendiendo el foco");

        hoursRemaining = 20 - currentHour;
        digitalWrite(pinRelay, HIGH);
    } else {
        lcd.clear();
        lcd.print("Apagando foco");
        Serial.println("Apagando el foco");
        digitalWrite(pinRelay, LOW);

        if (currentHour >= 20) {
            hoursRemaining = 32 - currentHour;
        } else {
            hoursRemaining = 8 - currentHour;
        }
    }

    lcd.setCursor(0, 1);
    lcd.print("Hs restantes: ");
    lcd.print(hoursRemaining);

    Serial.print("Hs restantes: ");
    Serial.println(hoursRemaining);

    delay(9000);
}
