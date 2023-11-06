#include <WiFi.h>
#include <ESPmDNS.h>

#include "config.h"  // Sustituir con datos de vuestra red
#include "functions.hpp"
#include "mDNS.hpp"

void setup()
{
  Serial.begin(115200);
  
  // ConnectWiFi_STA();
  // ConnectWiFi_AP();
  ConnectWiFi_STA();
  InitMDNS();

}
 
void loop() 
{
}