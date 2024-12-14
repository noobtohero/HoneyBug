#ifndef UNTILS_H
#define UNTILS_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;
extern String oldMsg;
extern String oldMsg2;

void lcdInit();
void display1(String msg);
void display2(String msg);

String displayRemaining(const uint32_t seconds);

#endif