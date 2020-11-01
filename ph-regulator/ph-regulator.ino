/**
  pH Regulator
  @auther Jeremy Bohrer
  @github https://github.com/jjbskir
  
  The circuit:
  - Peristaltic pump 
    https://www.dfrobot.com/product-1698.html
    https://wiki.dfrobot.com/Gravity__Digital_Peristaltic_Pump_SKU__DFR0523
    Power supply: 5-6v
    Interface: Black GND; Red 5V; Green: PPM digital (9 on my arduino uno)
    * Pump rate with tubing around 30ml/min
    * Docs say: 85ml/min
    0   -> clockwise maximum speed rotation
    90  -> stop
    180 -> counterclockwise maximum speed rotation

  - Analog pH sensor
    https://www.dfrobot.com/product-1110.html
    https://wiki.dfrobot.com/PH_meter_SKU__SEN0161_
    Power supply: 3.3-5v
    Interface: Black GND; Red 5V; Blue: analog 
    * Calibration:
    * 1) Put the pH sensor in 7.00 pH calibration liquid.
    * 2) Uncomment phCalibration() and upload the sketch.
    * 3) Update const phOffset with the result. 
    * 4) Put the pH sensor in 4.00 pH calibration liquid and let the value stabilize. 
*/

// setup peristaltic pump
#include <Servo.h>

Servo waterPump;

// digital ppm pin
const int WATER_PUMP_PIN = 9;
// analog input pin
const int PH_SENSOR_PIN = 0;
const int doseDuration = 2000;
const int sensorReadInterval = 5000;

// Change to your desired pH
const float phTarget = 7;
const float phToleration = 0.25;
// Calibrate your pH sensor in 7.0 solution. 
// The difference 7.0 and the pH read is your offset. 
// More instructions can be found in the wiki
const float phOffset = 0.00;
boolean isDosing = false;

void setup() {
  Serial.begin(9600);
  waterPump.attach(WATER_PUMP_PIN);
  Serial.println("Starting");
}

void loop() {     
  mainLoop();
//  phCalibration();
}

void mainLoop() {
  float ph = getPh();
  Serial.println("pH: " + String(ph));
  // Can also do reverse check ph > phTarget + phToleration
  if (ph < phTarget) {
    if (ph < phTarget - phToleration) {
      // start to raise the pH
      Serial.println("pH lower then target, begining to raise acidity");
      isDosing = true;
    }
    if (isDosing) {
      dose(); 
    }
  } else {
    isDosing = false;
  }
  
  delay(sensorReadInterval);
}

void phCalibration() {
  float ph = getPh();
  Serial.println("pH: " + String(ph));
  Serial.println("Change pH offset to: " + String(7.00 - ph - phOffset));
  delay(sensorReadInterval);
}

void dose() {
  waterPumpOn();
  delay(doseDuration);
  waterPumpOff();
}

void waterPumpOn() {
  // change to 0 if you want water to flow otherway
  // doesn't do much until it reaches 150. 
  Serial.println("Peristaltic pump on");
  waterPump.write(180);
}

void waterPumpOff() {
  Serial.println("Peristaltic pump off");
  waterPump.write(90);
}

float getPh() {
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  int buf[10],temp;
  
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(PH_SENSOR_PIN);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue+phOffset;                      //convert the millivolt into pH value
  return phValue;
}
