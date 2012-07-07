//-----------------------------------------------------------------------------      
void AnalogRead (void)
{
/**
 *\brief read and cumulate the analog port to compute average value
  *
 */
  
 // Summation of N values
  V7_Sum += analogRead(V7);            
  Pwr1_Vin_Sum += analogRead(Pwr1_Vin); 
  Pwr2_Vin_Sum += analogRead(Pwr2_Vin); 
  Batt1_Vin_Sum += analogRead(Batt1_Vin); 
  Batt2_Vin_Sum += analogRead(Batt2_Vin); 
  Temp1_Sum += analogRead(Temp1); 
  Temp2_Sum += analogRead(Temp2); 
 
  AverageCount ++;
  if(AverageCount >= AVERAGE_MAX)
  {// after N cycles compute the mean values
      AverageCount = 0;
      
      // ---------------------------------------LLS PWR supply
      V7_Val = V7_Sum >> 5;                       // average dividing by 32,
      V7_Sum = 0;                                 // restart for another cycle
      V7_Val = int((float(V7_Val) * V7_K)+0.5);   // multiplying by K to obtain the final value int * 100
      
      if((V7_Val < PWR_THRESHOLD_MIN) || (V7_Val > PWR_THRESHOLD_MAX))
      {// range test
        Defcon1(1); // never returns because this procedure switches off LLS
      }
      
  
      // --------------------------------------- Batt1
      Batt1_Vin_Val = Batt1_Vin_Sum >> 5;                   // average dividing by 32,
      Batt1_Vin_Sum = 0;                                 // restart for another cycle
      Batt1_Vin_Val = int((float(Batt1_Vin_Val) * Batt1_Vin_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if(Batt1_Vin_Val < VBATT_THRESHOLD1) 
      {
       Defcon1(2); // never returns because this procedure switches off LLS
      }
      else if(Batt1_Vin_Val < VBATT_THRESHOLD) 
      {
       Defcon2(51); // never returns because this procedure hangs the program
      } 
    
    
      // --------------------------------------- Batt2
      Batt2_Vin_Val = Batt2_Vin_Sum >> 5;                   // average dividing by 32,
      Batt2_Vin_Sum = 0;                                 // restart for another cycle
      Batt2_Vin_Val = int((float(Batt2_Vin_Val) * Batt2_Vin_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if(Batt2_Vin_Val < VBATT_THRESHOLD1) 
      {
       Defcon1(3); // never returns because this procedure switches off LLS
      }
      else if(Batt2_Vin_Val < VBATT_THRESHOLD) 
      {
       Defcon2(52); // never returns because this procedure hangs the program
      }   
  
      // --------------------------------------- Power Supply 1
      Pwr1_Vin_Val = Pwr1_Vin_Sum >> 5;                   // average dividing by 32,
      Pwr1_Vin_Sum = 0;                               // restart for another cycle
      Pwr1_Vin_Val = int((float(Pwr1_Vin_Val) * Pwr1_Vin_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if((Pwr1_Vin_Val < PWR_THRESHOLD_MIN) || (Pwr1_Vin_Val > PWR_THRESHOLD_MAX))
      {
       Defcon2(4); // never returns because this procedure switches off LLS
      }
      
      // --------------------------------------- Power Supply 2
      Pwr2_Vin_Val = Pwr2_Vin_Sum >> 5;                   // average dividing by 32,
      Pwr2_Vin_Sum = 0;                               // restart for another cycle
      Pwr2_Vin_Val = int((float(Pwr2_Vin_Val) * Pwr2_Vin_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if((Pwr2_Vin_Val < PWR_THRESHOLD_MIN) || (Pwr2_Vin_Val > PWR_THRESHOLD_MAX))
      {
       Defcon2(5); // never returns because this procedure switches off LLS
      }  
    
    
      // --------------------------------------- Temperature 1
      Temp1_Val = Temp1_Sum >> 5;                   // average dividing by 32,
      Temp1_Sum = 0;                               // restart for another cycle
      Temp1_Val = int((float(Temp1_Val) * Temp1_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if((Temp1_Val > TEMP_THRESHOLD))
      {
        Defcon2(7); // never returns because this procedure hangs the program
      }    
    
       // --------------------------------------- Temperature 2
      Temp2_Val = Temp2_Sum >> 5;                   // average dividing by 32,
      Temp2_Sum = 0;                               // restart for another cycle
      Temp2_Val = int((float(Temp2_Val) * Temp2_K)+0.5);// multiplying by K to obtain the final value int * 100
          
      if((Temp2_Val > TEMP_THRESHOLD))
      {
        Defcon2(8); // never returns because this procedure hangs the program
      } 
   
     Display(); // update the display with the new values   
  }
  
}

//-----------------------------------------------------------------------------      
void Display (void)
{
/**
 *\brief write numbers on display via I2C bus
  *
 */
    static unsigned char DispNum=0;
    unsigned char LeftBar;
    unsigned char LeftNum;
    unsigned char RightNum;
    unsigned char RightBar;
    unsigned char Arrow;
    unsigned char Buzz;
    int Temp_Val;
    int Temp_Val_T;
    int Temp_Val_U;
    static int TempFlag = 0;
    
    
    //Temperatures
    if(TempFlag == 0)
    {// alternating Temp 1 and Temp 2
      Temp_Val = Temp1_Val;
      TempFlag = 1;
      Arrow = DN; //DOWN arrow
    }
    else    
    {
      Temp_Val = Temp2_Val;
      TempFlag = 0;
      Arrow = UP; //UP arrow
    }
    Temp_Val_T=Temp_Val/100;
    Temp_Val_U=(Temp_Val-(Temp_Val_T*100))/10;

    if(!ErrCode)
    { 
      Wire.beginTransmission(I2C_DISP);
        Wire.write(0x00); //write on first register 
        Wire.write(BatteryLevel(Batt1_Vin_Val));    // LEDs bar left 
        Wire.write(Temp_Val_T);                     // Degrees units 
        Wire.write(Temp_Val_U);                     // Degrees tens
        Wire.write(BatteryLevel(Batt2_Vin_Val));    // LEDs bar right 
        Wire.write(Arrow);                          // Arrows, DN means Temp 1 
      Wire.endTransmission();
    }
    else
    {
      DisplayError();
    }
    
/*
    Wire.beginTransmission(I2C_DISP);
    Wire.write(0x01); //write on first register 
    Wire.endTransmission(false); //trasmission without stop, follows a restart with read
    
    Wire.requestFrom(I2C_DISP, 2);
    if(4 <= Wire.available())   // if four bytes were received
  {
    Serial.println(Wire.read());// receive high byte
    Serial.println(Wire.read());// receive high byte
    Serial.println(Wire.read());// receive high byte
    Serial.println(Wire.read());// receive high byte
  }
 */   
}

//-----------------------------------------------------------------------------       
void DisplayError(void)
{
  /**
 *\brief Display the error code
  *
 */
 
   Wire.beginTransmission(I2C_DISP);
      Wire.write(0x00); //write on first register 
      Wire.write(BatteryLevel(Batt1_Vin_Val));  // LEDs bar left 
      Wire.write(ErrCode/10 | 0XF30);           // Error Code tens, fast blinking
      Wire.write(ErrCode    | 0XF30);           // Error Code units,fast blinking 
      Wire.write(BatteryLevel(Batt2_Vin_Val));  // LEDs bar right 
      Wire.write(0);                            // Switch off Arrows 
   Wire.endTransmission();
}    
    
//-----------------------------------------------------------------------------       
unsigned char BatteryLevel (int Value)
{
  /**
 *\brief transform the Battery level value to LEDs bar segments
  *
 */
  if (Value >= 1600)
  {
    return(5);    // full charge
  }
  else if (Value < 1600 && Value >= 1500)
  {
    return(4);    // 75% charge
  }
  else if (Value < 1500 && Value >= 1450)
  {
    return(3);    // 50% charge
  }
  else if (Value < 1450 && Value >= 1350)
  {
    return(2);    // 25% charge
  }
  else if (Value < 1350 && Value >= 1300)
  {
    return(1); // alarm, blinking 
  }
  else if (Value < 1300)
  {
    return(0X31);    // cutoff
  }
}


//-----------------------------------------------------------------------------      
void Beep (int Duration)
{
/**
 *\brief perform a beep on the LLS board of the given duration in milliseconds
  *
 */
  digitalWrite(Buzzer,HIGH);
  delay(Duration);
  digitalWrite(Buzzer,LOW); 
}

//-----------------------------------------------------------------------------      
void I2cBeep (int Duration)
{
/**
 *\brief perform a beep on display of the given duration in milliseconds
  *
 */
    Wire.beginTransmission(I2C_DISP);
    Wire.write(0x05); //write on sixth register: buzzer
    Wire.write(0X00); //beep ON 
    Wire.endTransmission();
    delay(Duration);
    Wire.beginTransmission(I2C_DISP);
    Wire.write(0x05); //write on sixth register: buzzer
    Wire.write(0X10); //beep OFF
    Wire.endTransmission();   
}

//-----------------------------------------------------------------------------      
void Defcon1 (int Code)
{
/**
 *\brief do what is needed to manage a critical error:
 *Display error, wait, switch off
  *
 */
  ErrCode=Code;
  DisplayError();
  Beep(1000);
  digitalWrite(Batt_1_En,LOW);
  digitalWrite(Batt_2_En,LOW);
  digitalWrite(Pwr_1_En,LOW);
  digitalWrite(Pwr_2_En,LOW);
  I2cBeep(1000);
  delay(3000);
  digitalWrite(Sw_Power_latch,LOW);
}

//-----------------------------------------------------------------------------      
void Defcon2 (int Code)
{
/**
 *\brief do what is needed to manage a serious error:
 *Display error, wait for the operator
  *
 */
  ErrCode=Code;
  DisplayError();
  Beep(1000);
  
  digitalWrite(Batt_1_En,LOW);
  digitalWrite(Batt_2_En,LOW);
  digitalWrite(Pwr_1_En,LOW);
  digitalWrite(Pwr_2_En,LOW);

  #ifdef DEBUG_MODE
    Serial.println("***** PROGRAM STOPPED *****");
  #endif
  
  I2cBeep(1000);

  while(1); // halt
}

//-----------------------------------------------------------------------------      
void Defcon3 (int Code)
{
/**
 *\brief do what is needed to manage a warning:
 *change display from normal to allarm (blinking)
  *
 */
  ErrCode=Code;
  DisplayError();
}

//-----------------------------------------------------------------------------      
void DelayBar (unsigned int Duration)
{
/**
 *\brief perform a delay of given duration in milliseconds, sending a
  *dotted progression bar on Serial port
  *just for debug
 */
  unsigned int MsStep;
  int J;
  
  MsStep = Duration / 50;
  for(J=0; J<=50; J++)
  {
    delay(MsStep);
    Serial.print('.');
  }
   Serial.println(' ');
}

//-----------------------------------------------------------------------------      
void HeartBeat (void)
{ // regular blink as heart beat signal
  if (LedStat == LOW)
  {
    LedStat = HIGH;
    BlinkCycle.interval(BLINK_ON);  // change to ON period
  }
  else
  {
    LedStat = LOW;
    if (ErrCode != 0) 
    {// if battery low blink faster [5]
      BlinkCycle.interval(BLINK_ALRT);
    }
    else
    {
      BlinkCycle.interval(BLINK_OFF);  // change to OFF period
    }
  }
  
  digitalWrite(Led,LedStat);
}

