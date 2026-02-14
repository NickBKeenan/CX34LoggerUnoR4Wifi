#include "WifiLogger.h"


// https://script.google.com/macros/s/AKfycbzujkJAIL1gBMGyvaykXgk6egfC3WfcE7Of0y8mYZDOzqxLIn4BV-b71duKwzKNSemqtA/exec?Action=LogHPRun&Status=Radiant,0:00:31,46.9 46.9 46.9,89.6 89.6 89.6,87.6 87.6 87.6,87.4 87.4 87.4,2.4 2.4 2.4,0.0,0.0


  char ssid[] = SECRET_SSID;  // your network SSID (name)
  char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

  int status = WL_IDLE_STATUS;
  char server[] = "script.google.com";  // name address for Google (using DNS)


void reboot()
{
  NVIC_SystemReset();
} 

  WiFiSSLClient client;
  WiFiSSLClient redirect;


  void WifiLogger::check() {

      if(mode== MODE_WAITUPDATE)
      {
          WaitUpdate();
      
      }
      
      
      
  }
  void WifiLogger::ConnectWiFi()
  {
      // attempt to connect to WiFi network:
      status = WL_IDLE_STATUS;
      int nstatus = 0;
      int attemptCount=0;
      while (status != WL_CONNECTED) {
        attemptCount++;
          Serial.print("Attempting to connect to SSID: ");

          Serial.println(ssid);
          char cstatus;
          if (nstatus)
              cstatus = 'x';
          else
              cstatus = '+';
          nstatus = 1 - nstatus;


          


          status = WiFi.begin(ssid, pass);

          // wait 10 seconds for connection:

          delay(10000);
          if(attemptCount>100)
            reboot();
      }
     

      Serial.println("Connected to wifi");

      
  }
  void WifiLogger::setup() {
      //Initialize serial and wait for port to open:
      
      mode = MODE_IDLE;
      
      
   
      read = 0;


      // check for the WiFi module:

      if (WiFi.status() == WL_NO_MODULE) {

          Serial.println("Communication with WiFi module failed!");

          // don't continue

          while (true)
              ;
      }

      String fv = WiFi.firmwareVersion();
        Serial.println("Firmware:");
        Serial.println(fv);
   //     Serial.println(WIFI_FIRMWARE_LATEST_VERSION);

   
      ConnectWiFi();
  }
  // 

#define SCRIPT_URL "/macros/s/" SCRIPT_ID "/exec?Action=LogHPRun"






  void WifiLogger::PostUpdate(char * statusString)

  {
      

      read = 0;
      if (!client.connected())
      {
          Serial.println("\nConnecting to server...");
            client.stop();

          if (!client.connect(server, 443))

          {
              Serial.println(WiFi.status());
              client.stop();
              //mode = MODE_IDLE;
              



              //lastWebCheck +=30000; // set to retry in 5 minutes
              //if(lastWebCheck > millis())
                //lastWebCheck=millis()-1;
           
              Serial.println("unable to connect to server");
              WiFi.disconnect();
              WiFi.end();
              ConnectWiFi();
              return;
          }
          Serial.println("connected to server");
      }

      



          // Make a HTTP request:
          Serial.println();
          client.print("GET ");
          Serial.print("GET ");
          client.print(SCRIPT_URL);
          Serial.print(SCRIPT_URL);
          client.print("&Status=");
          Serial.print("&Status=");
          client.print(statusString);
          Serial.print(statusString);
          
          client.println(" HTTP/1.1");
          Serial.println();

          client.println("Host: script.google.com");

          client.println("Connection: keep-alive");

          client.println();
          if(client.connected()) // on keepalive timeout, server will disconnect once we send a request.
          {
            mode=MODE_WAITUPDATE;
            while(mode!= MODE_IDLE)
            {
              check();
            }
          }
          else
          {
            Serial.println("client is no longer connected");      
          }
      

  }
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  char redirect_URL[600];
#define COMPARE_STRING "Location: https://script.googleusercontent.com"
  char compare_string[] = COMPARE_STRING;
  /////////////////////////////////////////////
  int waitCount=0;
  void WifiLogger::WaitUpdate() {

      int linelength = 0;
      int maxlinelength = 0;
      char lineBuffer[600];
      bool haveredirect = 0;
      if(client.connected())
      {
        
      
        if(client.available())
        {
        
          waitCount=0;
        }
        else
        {
          waitCount++;
          delay(10);
          Serial.print("+");
        }
      }
      if(waitCount > 600) // roughly 60 seconds
      {
        Serial.println("Closing Client");
        
        client.stop();
        mode = MODE_IDLE;
        waitCount=0;
        return;
      }


      while (client.connected() && client.available()) {

          char c = client.read();
          read++;
          if (c == '\n') {
              if (linelength > maxlinelength) {
                  maxlinelength = linelength;
              }
              lineBuffer[linelength] = 0;
              //Serial.println(lineBuffer);
              if (lineBuffer[0] == compare_string[0]) {
                  if (strncmp(lineBuffer, compare_string, strlen(compare_string)) == 0) {
                      strcpy(redirect_URL, lineBuffer + strlen(compare_string));
                      Serial.println(redirect_URL);

                      haveredirect = 1;
                  }
              }
              linelength = 0;
          }
          else {
              if (linelength < sizeof(lineBuffer) - 2) {
                  lineBuffer[linelength] = c;
                  linelength++;
              }
          }
          if((client.connected() && !client.available()))
          {
            delay(10);
          }


      }

      // if the server's disconnected, stop the client:
      if (!client.connected()) {
        Serial.println("Server disconnect detected");
          client.stop();
                    if( !haveredirect)
          {

            mode = MODE_IDLE;
            
            return;
          }
      }
      else
      {
          if (read == 0)
          {
              
              delay(10);
              Serial.print(".");
              return;
          }
      }

      Serial.println();


      Serial.print("Bytes read:");
      Serial.println(read);
      Serial.print("Max line length:");
      Serial.println(maxlinelength);
      client.stop();

      // now we have to read the buffer for a redirect string. If it's there, go onto the next mode. Otherwise go back to mode zero
      if (haveredirect) {
          //mode = MODE_GETREDIRECT;
          mode=MODE_IDLE;
      }
      else {
        Serial.println("Error, redirect string not found");
        Serial.println();
          mode = MODE_IDLE;
          
      }

  }
  void WifiLogger::GetRedirect() {



      if (!redirect.connected())
      {
     // delay(5000);

      Serial.println("Connecting to redirect");
      redirect.stop();
          if (!redirect.connect("script.googleusercontent.com", 443))
          {





              Serial.println("Unable to connect to redirect");
              Serial.println(redirect_URL);
              Serial.println(WiFi.status());

              redirect.stop();


              

              WiFi.disconnect();
              WiFi.end();
              ConnectWiFi();
              
              Serial.println("Redirect refused connection");
              return;
          }
          else
          {
              Serial.println("Redirect connect succssful");

          }

      }
      {

        
          redirect.print("GET ");
          redirect.print(redirect_URL);
          redirect.println(" HTTP/1.1");


          redirect.println("Host: script.googleusercontent.com");

          redirect.println("Connection: keep-alive");


          redirect.println();
          Serial.println();
          if (redirect.connected()) // if we had a keepalive connection and it timed out, the server will disconnect right here
          // in that case, just loop back and try again on the next pass
          {
      
           // mode = MODE_WAITREDIRECT;
            read = 0;
          }
      }

  }
  



    

  /*************************************** End of Thermostat ***************************************************************/
 