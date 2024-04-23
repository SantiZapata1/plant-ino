#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

#define MENU_1 1
#define MENU_2 2
#define MENU_3 3
#define MENU_4 4
#define MENU_5 5

#define PUL A2
#define EJE_X A0
#define EJE_Y A1


int menu = MENU_1;

unsigned long tiempoActual1;
unsigned long tiempoReferencia1;
unsigned long tiempoActual2;
unsigned long tiempoReferencia2;
unsigned long tiempoActual3;
unsigned long tiempoReferencia3;
unsigned long tiempoActual4;
unsigned long tiempoReferencia4;

Bounce pul1 = Bounce();
LiquidCrystal_I2C lcd(0x27, 16, 2);


//---------------------------------------------- SETUP
void setup() {

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(115200);

  pul1.attach(PUL, INPUT_PULLUP);
  pinMode(EJE_X, INPUT);
  pinMode(EJE_Y, INPUT);


  
}


//---------------------------------------------- LOOP

void loop() {

  pul1.update();
  bool Btn1 = pul1.fell();

  int X = analogRead(EJE_X);
  int Y = analogRead(EJE_Y);

  //suspender pantalla en caso de no uso
  tiempoActual3 = millis();
  if (tiempoActual3 - tiempoReferencia3 >= 60000) {

    tiempoReferencia3 = tiempoActual3;
    lcd.noBacklight();
  }
  if (X >= 700 || X <= 300 || Y >= 700 || Y <= 300 || Btn1 == true) {
    lcd.backlight();
  }

  //Manejo de menues
  tiempoActual1 = millis();

  if (tiempoActual1 - tiempoReferencia1 >= 250) {

    tiempoReferencia1 = tiempoActual1;

    if (menu == MENU_1) {

      if (X >= 900) {
        menu = MENU_2;
        lcd.clear();
      } else if (X <= 100) {
        menu = MENU_5;
        lcd.clear();
      }

    } else if (menu == MENU_2) {
      lcd.setCursor(0, 0);
      lcd.print("Inicio luz: ");
      

    }

    else if (menu == MENU_3) {
      lcd.setCursor(0, 0);
      lcd.print("fin luz: ");
      


      if (X >= 900) {
        menu = MENU_4;
        lcd.clear();
      } else if (X <= 100) {
        menu = MENU_2;
        lcd.clear();
      }

      else if (Y >= 900) {
        finLuz++;
        lcd.clear();
      } else if (Y <= 100) {
        finLuz--;
        lcd.clear();
      }

    } else if (menu == MENU_4) {
      lcd.setCursor(0, 0);
      lcd.print("tiempo ventis:");
      


      if (X >= 900) {
        menu = MENU_5;
        lcd.clear();
      } else if (X <= 100) {
        menu = MENU_3;
        lcd.clear();
      }

      else if (Y >= 900) {
        tiempoVentis++;
        lcd.clear();
      } else if (Y <= 100) {
        tiempoVentis--;
        lcd.clear();
      }

    } else if (menu == MENU_5) {
      lcd.setCursor(0, 0);
      lcd.print("luz:");




      if (X >= 900) {
        menu = MENU_1;
        lcd.clear();
      } else if (X <= 100) {
        menu = MENU_4;
        lcd.clear();
      }
    }
  }
}



