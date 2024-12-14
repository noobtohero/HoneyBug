#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>
#include <Wire.h>
#include <PCF8574.h>


enum button
{
    power,
    menu,
    up,
    down,
    left,
    right,
    ok,
    acPower
};
// set address & pin I/O Expendent

extern PCF8574 remote;
extern unsigned long stop_time;

void m_reset();
void m_start(int);
void m_stop();
void m_addTimes(int);
void m_home();
void m_disableVoiceCommands();
void m_massageStrength();
void m_balloonStrength();
void setAcPower(bool);
void remoteInit();
void click(button);

#endif