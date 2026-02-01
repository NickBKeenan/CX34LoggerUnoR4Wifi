#include <ModbusClient.h>

class CX34Reading
{
public:
  float ambient, inlet, outlet, flow;
short setting;
float current;
float volts;  
float BTU;
float Watts;
float COP;
float supplemental;
float setpoint;
short frequency;
short defrost;


  public: 
  bool Read(ModbusClient*pnode, const char * pLabel);

};
class HLA  // compiles high, low, average stats
{
  int count;
  float sum;
  float high;
  float low;
  char statline[100];
public:
  void Reset();
  void Add(float newvalue);
  float GetHigh();
  float GetLow();
  float GetAverage();
  char * StatLine();
};
class CX34Status
{
  unsigned long starttime;
  unsigned long lasttime;
  
  char m_StatusLine[200];

  //
  short lastfrequency;
  short lastdefrost;

  HLA hlaAmbient;
  HLA hlaSetpoint;
  HLA hlaInlet;
  HLA hlaOutlet;
  HLA hlaFlow;
  HLA hlaFreq;

  float BTU;
  float Watts;
  float supplemental;

  public:
  CX34Status();
  void Log(CX34Reading * pcxr);
  void Reset();
  
  bool changed(CX34Reading *preading);

char * statusLine(const char *pLabel);
    
};