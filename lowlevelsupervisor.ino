/* ////////////////////////////////////////////////////////////////////////////
** File:      LowLevelSupervisor.ino
*/                                  
 unsigned char  Ver[] = "LinoLLS     0.8.0 Guiott 07-12"; // 30+1 char

/**
* \mainpage LowLevelSupervisor.ino
* \author    Guido Ottaviani-->guido@guiott.com<--
* \version 0.8.0
* \date 07/12
* \details 
 *
-------------------------------------------------------------------------------
\copyright 2012 Guido Ottaviani
guido@guiott.com

    LowLevelSupervisor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LowLevelSupervisor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LowLevelSupervisor.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------------------------
*/


// Compiler options
#define DEBUG_MODE // If defined the serial output is in ASCII for debug


#include <Wire.h>
#include <Metro.h>

// Digital OUT
int Led = 13;             // standard yellow led            
int Batt_1_En = 46;       // Enable 14.4V on PowerControl 1
int Pwr_1_En  = 48;       // Enable 7V on PowerControl 1         
int Batt_2_En = 50;       // Enable 14.4V on PowerControl 2               
int Pwr_2_En  = 52;       // Enable 7V on PowerControl 2                
int Sw_Power_latch = 53;  // Relay self retention on LL board
int Buzzer = 51;          // Buzzer on LLS board

// PWM
int Light_L = 2;          // Headlights intensity control
int Light_R = 3;


//Analog IN
int V7 = A4;              // Switching power supply on LLS board
int Pwr1_Vin = A3;        // Switching power supply on PwrCtrl 1 board
int Pwr2_Vin = A2;        // Switching power supply on PwrCtrl 2 board
int Batt2_Vin = A1;       // Battery 1 level
int Batt1_Vin = A0;       // Battery 2 level
int Temp1 = A5;           // Temperature sensor 1
int Temp2 = A6;           // Temperature sensor 2

//Analog values
int V7_Val;
int Pwr1_Vin_Val;
int Pwr2_Vin_Val;
int Batt1_Vin_Val;
int Batt2_Vin_Val;
int Temp1_Val;
int Temp2_Val;

//Summation values
int V7_Sum=0;
int Pwr1_Vin_Sum=0;
int Pwr2_Vin_Sum=0;
int Batt1_Vin_Sum=0;
int Batt2_Vin_Sum=0;
int Temp1_Sum=0;
int Temp2_Sum=0;

int AverageCount = 0;

// Conversion and calibration factors
const float V7_K = 2.32;
const float Pwr1_Vin_K = 2.52;
const float Pwr2_Vin_K = 2.51;
const float Batt1_Vin_K = 2.422;
const float Batt2_Vin_K = 2.422;
const float Temp1_K = 5.35;
const float Temp2_K = 5.13;

#define VBATT_THRESHOLD  1350 // Battery alert in Volt * 10
#define VBATT_THRESHOLD1 1300 // Battery alert in Volt * 10
#define PWR_THRESHOLD_MIN 650 // Power Supply alert in Volt * 10
#define PWR_THRESHOLD_MAX 750 // Power Supply alert in Volt * 10
#define TEMP_THRESHOLD 4500   // Temeperature limit in degrees * 100
#define UP 1                  // Arrow direction
#define DN 2                  // Arrow direction
#define AVERAGE_MAX 64        // Number of iterations before averaging analog values
#define AVERAGE_CYCLE 50      // Time between cycles 64 iterations * 50ms = 3.2s for each measure

int ErrCode=0;                // Error code

#define BLINK_ON 200          // HeartBeat Blink On time
#define BLINK_OFF 800         // HeartBeat Blink Off time
#define BLINK_ALRT 100        // HeartBeat Blink Alert time

int LedStat = LOW;

#define I2C_DISP 0x22         // Display I2C address

Metro BlinkCycle = Metro(BLINK_OFF,1);  // LED blink cycle
Metro AnalogCycle = Metro(10,1);        // Display write cycle


int TimeElapsed = millis();

//-----------------------------------------------------------------------------      

void setup()
{
  pinMode(Led, OUTPUT);      // sets the digital pin as output
  pinMode(Batt_1_En, OUTPUT);
  pinMode(Pwr_1_En, OUTPUT);
  pinMode(Batt_2_En, OUTPUT);
  pinMode(Pwr_2_En, OUTPUT);
  pinMode(Sw_Power_latch, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Light_L, OUTPUT);
  pinMode(Light_R, OUTPUT);

  Serial.begin(9600); 
  Wire.begin();                // join i2c bus (as master)
  
  LLSstartup (); 
}

//-----------------------------------------------------------------------------      

void loop()
{  
 
  if (BlinkCycle.check() == 1) {HeartBeat();}  // Led blink
  if (AnalogCycle.check() == 1) {AnalogRead();}   // Write On display

}

