#include "helper/Untils.h"

// set address LCD display
LiquidCrystal_I2C lcd(0x27, 21, 22);
String oldMsg = "";
String oldMsg2 = "";

void lcdInit()
{
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.clear();
}

void display1(String msg)
{
    if (msg != oldMsg)
    {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(msg);
        oldMsg = msg;
    }
}

void display2(String msg)
{
    if (msg != oldMsg2)
    {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(msg);
        oldMsg2 = msg;
    }
}

String displayRemaining(const uint32_t seconds)
{
    String txt = "";
    uint32_t t = seconds / 1000;
    uint16_t h;
    uint8_t m;
    uint8_t s;

    s = t % 60;

    t = (t - s) / 60;
    m = t % 60;

    t = (t - m) / 60;
    h = t;

    if (h < 10)
    {
        txt += "0";
        Serial.print("0");
    }
    txt += String(h) + ":";
    Serial.print(h);
    Serial.print(":");

    if (m < 10)
    {
        txt += "0";
        Serial.print("0");
    }
    txt += String(m) + ":";
    Serial.print(m);
    Serial.print(":");

    if (s < 10)
    {
        txt += "0";
        Serial.print("0");
    }
    txt += String(s);
    Serial.println(s);

    return "    " + txt + "    ";
}
