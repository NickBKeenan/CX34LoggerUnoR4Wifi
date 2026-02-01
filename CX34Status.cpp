#include "CX34Status.h"
  float ltogal(short data);
  float CtoF(short data);

bool CX34Reading::Read(ModbusClient*pnode, const char * pLabel)
{
  uint8_t  result;
     int holdingCount = 64; 
  
    int holdingFirst = 200;
  
    short holdingData[holdingCount];
    




  // slave: read (6) 16-bit registers starting at register 2 to RX buffer
  
  result = pnode->readHoldingRegisters( holdingFirst, holdingCount);
  

  // do something with data if read is successful
  int j;
  if (result ==  0)
  {
    for (j = 0; j < holdingCount; j++)
    {
      holdingData[j] = pnode->ResponseBufferGetAt(j);
    }
  }
  else
  {
    
    delay(1000);
    return false;
  }
// ambient c02, inlet temp c281, outlet temp 205 , waterflow c13
// pump setting c49, compressor current c56

  ambient=CtoF(holdingData[2]);
  
  outlet=CtoF(holdingData[5]);
  flow=ltogal(holdingData[13]);
  setting=holdingData[48];
  current=holdingData[56]/10.0;
  volts=holdingData[55];
  supplemental=holdingData[62];
  frequency=holdingData[27];
  defrost=holdingData[16];
  

  result = pnode->readHoldingRegisters( 281, 1);
  if(result!=0)
  {
    
    delay(1000);
    return false;
  }


  inlet=CtoF(pnode->ResponseBufferGetAt(0));

  // get  setpoint
  // cooling is 142
  // heating is 143
  // mode is 141

  result=pnode->readHoldingRegisters( 141, 3);
  if(result!=0) 
  {
    
    delay(1000);
    return false;
  }

  if(pnode->ResponseBufferGetAt(0) ==0 )  // cool
  {
    setpoint=pnode->ResponseBufferGetAt(1);
  }
  else
  {
    setpoint=pnode->ResponseBufferGetAt(2);
  }
  setpoint=CtoF(setpoint*10);
  
  BTU=(outlet-inlet)*flow*500;
  Watts=current*240; // the emporia watts 
  if(Watts>0)
    COP=BTU/3.412/Watts;
  else
    COP=0;
  Serial.println();
  Serial.print(pLabel);

  Serial.print(" Ambient: ");
  Serial.print(ambient);
  Serial.print(" Setpt: ");
  Serial.print(setpoint);
  Serial.print(" in: ");
  Serial.print(inlet);
  
  
  Serial.print(" F out: ");
  Serial.print(outlet);
  Serial.print(" F Flow: ");
  Serial.print(flow);
  Serial.print(" Pump Setting: ");
  Serial.print(setting);
  Serial.print(" Current: ");
  Serial.print(current);
  Serial.print(" Volts: ");
  Serial.print(volts);
  Serial.print(" BTU: ");
  Serial.print(BTU);
  Serial.print(" Watts: ");
  Serial.print(Watts);
  Serial.print(" COP: ");
  Serial.print(COP);
  Serial.print(" Supp: ");
  Serial.print(supplemental);
  Serial.print(" Freq: ");
  Serial.print(frequency );
  Serial.print(" Def: ");
  Serial.print(defrost );
    return true;
  //delay(5000);
}
CX34Status::CX34Status()
{
  Reset();
}
  void CX34Status::Reset()
  {
    hlaAmbient.Reset();
    hlaSetpoint.Reset();
    hlaInlet.Reset();
    hlaOutlet.Reset();
    hlaFlow.Reset();
    hlaFreq.Reset();
    BTU=0;
    Watts=0;
    supplemental=0;
    lastdefrost=0;

  starttime=millis();
  lasttime=starttime;
  lastfrequency=-1;

  }
void CX34Status::Log(CX34Reading * pcxr)
{
  // Elapsed time
  // HLA
    // Ambient
    // setpoint
    // inlet
    // outlet
    // flow
  hlaAmbient.Add(pcxr->ambient);
  hlaSetpoint.Add(pcxr->setpoint);
  hlaInlet.Add(pcxr->inlet);
  hlaOutlet.Add(pcxr->outlet);
  hlaFlow.Add(pcxr->flow);
  hlaFreq.Add(pcxr->frequency);


  lastfrequency=pcxr->frequency;
  lastdefrost=pcxr->defrost;
  unsigned long now=millis();
  unsigned long elapsed=now-lasttime;
  if(elapsed > 0)
  {
    float hours=(elapsed)/3600000.0;
    BTU+=pcxr->BTU*hours;
    Watts+=pcxr->Watts*hours;
    supplemental+=pcxr->supplemental*hours;
  }
  lasttime=now;

  //Total
    //BTU
    //Watts

};

  bool CX34Status::changed(CX34Reading *preading)
  {
    if(lastfrequency==-1)  // first time called since reset, no data
      return false; 
    short newfreq;
    newfreq=preading->frequency;
    
    if((lastfrequency==0 && newfreq!=0) ||(lastfrequency!=0 && newfreq==0))
      return true;

    unsigned long now=millis();
    unsigned long elapsed=now-starttime;
    float hours=(elapsed)/3600000.0;
    if(hours>1.00)
    {
      return true;
    }

  return false;
    
  }

  char * CX34Status::statusLine(const char * pLabel)
  {
      // Elapsed time
  // HLA
    // Ambient
    // setpoint
    // inlet
    // outlet
    // flow
  //Total
    //BTU
    //Watts

  //average
    //cop
    //BTU/hr
    unsigned long hour;
    unsigned long minutes;
    unsigned long seconds;
    unsigned long now = millis()-starttime;
    seconds = now / 1000;
    minutes = now / 60000;
    seconds -= minutes * 60;

    hour = minutes / 60;
    minutes -= hour * 60;

    float COP;
    float BTUperhour;
    COP=BTU/Watts/3.412;
    BTUperhour=BTU/(now/3600000.0);

    char  fmtOn[]="%s,%d:%02d:%02d,%s,%s,%s,%s,%s,%.1f,%.1f,%.0f";
    char  fmtOff[]="%s,%d:%02d:%02d,%s,%s,%s,%s,%s,%.1f,%.1f,%.0f,%.2f,%.0f,%.1f";
    char * fmtLine;
    if(lastfrequency>0)
    {
      fmtLine=fmtOff;
    }
    else
    {
      fmtLine=fmtOn;
    }



    sprintf(m_StatusLine, fmtLine, 
    pLabel,
    hour,
    minutes, 
    seconds,
    
    hlaAmbient.StatLine(),
    hlaSetpoint.StatLine(),
    hlaInlet.StatLine(),
    hlaOutlet.StatLine(),
    hlaFlow.StatLine(),
    BTU,
    Watts,
    supplemental,
    COP,
    BTUperhour,
    hlaFreq.GetAverage()

    );
    return m_StatusLine;
  }
    



  float ltogal(short data)
  {
    float retval=data;
    retval/=10;
    retval*=0.264172;
    return retval;
  }
  float CtoF(short data)
  {
    float retval;
    retval=data;
    retval/=10;
    retval*=1.8;
    retval+=32;
    return retval;
  }

  char * HLA::StatLine()
  {
    sprintf(statline, "%.1f,%.1f,%.1f", GetHigh(), GetLow(), GetAverage());
    return statline;
  }
  void HLA::Reset()
  {
    count=0;
    sum=0;
    high=0;
    low=0;

  }
  void HLA::Add(float newvalue)
  {
    if(count > 0)
    {
      if(newvalue>high)
        high=newvalue;
      if(newvalue<low)
        low=newvalue;
    }
    else
    {
      high=newvalue;
      low=newvalue;
    }
    sum+=newvalue;
    count++;

  }
  float HLA::GetHigh()
  {
    return high;
  }
  float HLA::GetLow()
  {
    return low;
  }
  float HLA::GetAverage()
  {
    if(count > 0)
      return sum/count;
    else
      return 0;
    }
