#include <SD.h>
//#include <IniFile.h>

#include <IniFile.h>

void setup()
{
  const int macLen = 18;
  char mac[macLen];
  int i;
  
  Serial.begin(9600);
  IniFile ini("/etc/net.ini");
  if (ini.isOpen()) {
    Serial.println("Ini file exists");
    i = ini.getValue(mac, macLen, "mac");
    if (i >= macLen)
      Serial.println("Buffer too small to hold value");
    else {
      Serial.print("Mac address is ");
      Serial.println(mac);
    }
  }
  else {
    Serial.print("Ini file does not exist");
  }
}


void loop()
{


}
