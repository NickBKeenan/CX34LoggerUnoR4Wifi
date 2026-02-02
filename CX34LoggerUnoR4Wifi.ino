
#include <ModbusClient.h>


#include "WifiLogger.h"
#include "CX34Status.h"

ModbusClient node;

extern const char * HoldingLabels[];



#define SERIAL_DE_PIN A3
#define SERIAL_RE_PIN A2



#define ModbusID 1 


WifiLogger logger;

void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  while(!Serial && millis()< 10000);

  Serial.print(__FILE__);
  Serial.print(" ");
  Serial.println(__DATE__);


  Serial1.begin(9600);
  

    node.begin(ModbusID, Serial1);

    node.SetPins(SERIAL_DE_PIN, SERIAL_RE_PIN);




     
  pinMode(SERIAL_DE_PIN, OUTPUT);
 pinMode(SERIAL_RE_PIN, OUTPUT);

  Serial.println("About to write pins");
  digitalWrite(SERIAL_DE_PIN, HIGH);
digitalWrite(SERIAL_RE_PIN, HIGH);

 // logger.setup(&lcd);
  logger.setup();

}
int x=0;
CX34Status HeatPumpStatus;

void loop() {
  // put your main code here, to run repeatedly:
  // check once a minute, log once every ten minutes
  //logger.PostUpdate("test,test");
  while(!DisplaySettings(&node, &HeatPumpStatus, "HeatPump"))
    ;
  delay(30000); // one minute
  
  return;
}


bool DisplaySettings(ModbusClient*pnode, CX34Status *pStatus, const char * pLabel)
{
  CX34Reading c34;
  if(!c34.Read(pnode, pLabel))
    return false;
  
  if(pStatus->changed(&c34))
  {
    Serial.println();
    
    logger.PostUpdate(pStatus->statusLine(pLabel));
    pStatus->Reset();
  }
  pStatus->Log(&c34);
  return true;
}






