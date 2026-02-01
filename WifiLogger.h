//

#include <SPI.h>

//#include <WiFiNINA.h> //R
//#include <WiFiS3.h> // R4

#include <WiFi.h> // for Giga
#include <WiFiSSLClient.h> // For giga only

#include "arduino_secrets.h"


#define MODE_IDLE 0
#define MODE_GETUPDATE 1
#define MODE_WAITUPDATE 2


class WifiLogger
{
  private:

  int mode ;
  unsigned long lastWebCheck;
  
  unsigned long delayEnd; // if we're in delay mode, when it ends
  
  unsigned int errorlength;
  char lastStatus[10];
  int read;
  public:
  void PostUpdate(char * status);
  private:
  void WaitUpdate();
  void GetRedirect();
  void WaitRedirect();
  void ConnectWiFi();
  
  public:
  
  char * GetStatus();
  void setup();
  void check();
  void printStatus();
};
