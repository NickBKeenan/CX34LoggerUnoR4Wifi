//

#include <SPI.h>


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
  
  int read;
  public:
  void PostUpdate(char * status);
  private:
  void WaitUpdate();
  void GetRedirect();
  void WaitRedirect();
  void ConnectWiFi();
  
  public:
  
  
  void setup();
  void check();
  
};


