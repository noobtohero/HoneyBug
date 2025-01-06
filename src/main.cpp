#include <Arduino.h>

#include "helper/Untils.h"
#include "helper/Macros.h"



// ##### user settings #####
#define VERSION_NO "60125.1"
#define MACHINE_SN "Srinagarind-001"
#define MASSAGE_MODE 2 // mode: 0=นวดเร็วทั่วร่างกาย, 1=นวดสบาย, 2=นวดผ่อนคลาย, 3=นวดยึดตัว, 5=คอบ่าไหล่  14=ออฟฟิต
const char *WL_SSID = "Wiwamassage";
const char *WL_PASSWORD = "26462646";
// ##### end user settings #####

// pin setting for UNO SHIELD
// pulsePin -> 27
// inhibit, SSR relay -> 14,15,16
// i2c -> 21,22
// i2s -> 25,26,32
// SDCARD SPI -> 5,18,19,23
// do not use U2(Serial-1) RX,TX 

// NK77 DIP SWITCH -> 0110100111
// 2pulse:10bath

#define PULSE_PIN 27
#define SSR_PIN 14
#define INHIBIT_PIN 16

enum state
{
  standby,
  startProgram,
  inProgram,
  endProgram
};
state currentState;

// set ICT NK77

volatile int pulseCount = 0;
volatile unsigned long firstPulseTime = 0;
volatile unsigned long lastPulseTime = 0;
unsigned long debouceDelay = 200; // support 10 pulse:  ทดลองที่ 200ms

// global variable
unsigned long countdown = 0;
unsigned long newCredits = 0;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
long remainingTime = 0; // ติดลบได้
unsigned long previousMillis = 0;
unsigned long waitForCredits = 7000;

// limit add+ times
int star = 0;

void countPulse();
void identifyBill();
int convertCreditToTime(int);
void addTimeToCountdown(int);

bool sendLineNotify(String, int, int);

void setup()
{
  Serial.begin(9600);
  // setup ICT NK77
  pinMode(PULSE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PULSE_PIN), countPulse, FALLING);
  // setup i2c PCF8574
  remoteInit();
  // setup LCD 1602
  lcdInit();

  display1("Wait for setting");

  startTime = millis();
  currentState = standby;
  Serial.println("set state: standby");
  m_reset();

  Serial.println("ready to start..");
  // newCredits = 20;
}

void loop()
{
  volatile unsigned long currentMillis = millis();

  // debug overflow event
  if (currentMillis < previousMillis)
  {
    previousMillis = currentMillis;
  }

  // pulse counter
  if (pulseCount > 0 && (currentMillis - lastPulseTime >= debouceDelay))
  {
    if (currentState == standby && (currentMillis - stop_time > 1000))
    {
      // อนุญาติให้ใส่เงินได้แค่ช่วง standby ลดปัญหา pulseรวน
      identifyBill();
    }
    else
    {
      pulseCount = 0; // reset กรณีรวน
    }
  }

  // if have credit
  if (newCredits > 0)
  {
    if (currentMillis - lastPulseTime >= waitForCredits)
    {
      int massageTimes = convertCreditToTime(newCredits);
      addTimeToCountdown(massageTimes);
      newCredits = 0; // reset
    }
    else
    {
      display1("credits: " + String(newCredits) + ".-");
    }
  }

  // display & countdown
  if (currentMillis - previousMillis >= 1000)
  {
    previousMillis = currentMillis;

    // calculate time remaining : Bug Overflow
    elapsedTime = currentMillis - startTime;
    remainingTime = countdown - elapsedTime;

    if (remainingTime > 0)
    {
      // show remaining countdown..
      display1(" Massage times. ");
      display2(displayRemaining(remainingTime));

      // debug
      Serial.printf("run time: %d, countdown: ",elapsedTime);

      switch (currentState)
      {
      case standby:
        // running to inProgram State..
        currentState = startProgram;
        m_start(MASSAGE_MODE);
        currentState = inProgram;
        break;
      case inProgram:

        break;
      }
    }
    else
    {
      // resetall
      countdown = 0;

      // ####################
      // state Machine
      // ####################
      switch (currentState)
      {
      case inProgram:
        // set to stop massage
        currentState = endProgram;
        m_stop();
        currentState = standby;
        break;
      case standby:
        // wait credits

        Serial.println("standby mode. wait credit insert..");
        display1("insert credits..");
        display2("        ");
        break;
      }
    }
  }
  else
  {
  }
}

// ##############################
// PULSE COUNTER & TIMER
// ##############################

void countPulse()
{
  if (pulseCount == 0)
  {
    firstPulseTime = millis();
  }
  pulseCount++;
  lastPulseTime = millis();
}

void identifyBill()
{
  volatile int p = pulseCount;
  pulseCount = 0; // reset pulseCount

  int billValue = 0;
  unsigned long diff = lastPulseTime - firstPulseTime;
  Serial.print("Pulse >> ");
  Serial.print(p);
  Serial.print(", diff times: ");
  Serial.println(diff);

  if ((p >= 4 && p < 6) ) // || (diff > 100 && diff <= 155)
  {
    billValue = 20;
  }
  else if ((p >= 10 && p < 14) )// || (diff > 155 && diff <= 610)
  {
    billValue = 50;
  }
  else if (p >= 20 && p < 24 ) // || diff > 1300
  {
    billValue = 100;
  }
  else
  {
    billValue = 0;
  }

  newCredits += billValue;
  if (newCredits > 80)
  {
    newCredits = 80; // cut off 40min
  }
  Serial.print("insert Bill: ฿");
  Serial.print(billValue);
  Serial.print(" >> summary: ฿");
  Serial.println(newCredits);
}

int convertCreditToTime(int credit)
{
  return credit / 2; // 10min:20฿
  // return credit / 2.5; // 8min:20฿
  // return credit / 4; // 5min:20฿
}

void addTimeToCountdown(int min)
{
  if (countdown <= 0)
  {
    startTime = millis();
    Serial.print("set Start time at: ");
    Serial.println(startTime);
  }

  countdown += (min * (60 * 1000));
  if (countdown > 2400000)
  {
    countdown = 2400000;
    Serial.println("Maximum limit 40min!");
  }
  else
  {
    Serial.print("add Massage times: ");
    Serial.print(min);
    Serial.println(" min.");
  }
}

// ##############################
// REMOTE & MACRO
// ##############################
