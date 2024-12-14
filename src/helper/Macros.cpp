#include "Macros.h"
#include "Untils.h"

PCF8574 remote(0x20, 21, 22);
unsigned long stop_time = 0;

void m_reset()
{
    // ถ้าไฟดับในท่านอน จะมี่ error ต้องมี่ sensor เช็ค หรือเช็คตัวแปรใน rom ว่าทำงานครบรอบ
    display1("+resetting.");
    Serial.println("+resetting..");
    // delay(7000);
    setAcPower(true); // เปิดเครื่อง
    delay(5000);      // รอรีโมททำงาน

    click(power); // power ON
    delay(30000); // รอเก้าอี้เข้าสู่ท่านั่ง

    // disable voice commands
    m_disableVoiceCommands();

    click(power); // power OFF
    display1("+power OFF");
    Serial.println("+power OFF");
    delay(30000);
    setAcPower(false); // ปิดเครื่อง

    display1("+reset complete.");
    Serial.println("resetting complete!..");
}

void m_start(int mode)
{
    setAcPower(true); // เปิดเครื่อง
    delay(5000);
    Serial.println("state: start program..");
    display1(" Start Massage. ");

    click(power); // power on
    delay(10000);

    for (int i = 0; i < mode; i++)
    {
        click(down);
    }

    click(ok); // start program
    display2("   Body Scan.   ");
    delay(30000); // แสกนร่างกาย
    click(ok);    // skip

    m_massageStrength();
    m_balloonStrength();

    // if countdown > 40min ; countdown = 40min
    //   if (countdown > 2400000)
    //   {
    //     countdown = 2400000;
    //   }

    if (mode > 0)
    {
        m_addTimes(2); // any mode
    }
    else
    {
        m_addTimes(3); // quick mode
    }
}

void m_stop()
{
    Serial.print("Stop Massage... wait 30sec");
    click(power);
    delay(30000);
    Serial.println(" >> success.");
    stop_time = millis();
    setAcPower(false);
}

void m_addTimes(int loop)
{
    Serial.print("macro: add time ");
    Serial.print(loop);
    Serial.println(" loop");

    click(ok);

    click(down);
    click(down);
    click(down);

    click(right);

    for (int i = 0; i < loop; i++)
    {
        // if (star < 8)
        // {
        click(ok);
        Serial.println("click: add +10");
        // }
    }

    m_home();
}

void m_disableVoiceCommands()
{
    display2("disable voice");
    Serial.println("disable voice");
    click(menu);
    click(down);
    click(down);
    click(down);
    click(down);
    click(ok);
    click(down);
    click(down);
    click(right);
    click(ok);
    display2(" ");
}

void m_massageStrength()
{
    // set massage strength
    display2("set massage STR.");
    Serial.println("set massage strength");
    click(ok);
    click(right);
    click(ok);
    m_home();
}

void m_balloonStrength()
{
    // set balloon strength
    display2("set Balloon STR.");
    Serial.println("set balloon strength");
    click(ok);
    click(down);
    // click(down);
    // click(down);
    // click(down);
    click(right);
    click(down);
    click(ok);
    m_home();
}

void m_home()
{
    Serial.println("go to home..");
    click(left);
    click(left);
    click(left);
}

void setAcPower(bool swOn)
{
    if (swOn)
    {
        remote.digitalWrite(acPower, LOW);
        Serial.println("ac power ON");
    }
    else
    {
        remote.digitalWrite(acPower, HIGH);
        Serial.println("ac power OFF");
    }
}

void remoteInit()
{
    Serial.print("remote Init... ");
    remote.pinMode(P0, OUTPUT, HIGH);
    remote.pinMode(P1, OUTPUT, HIGH);
    remote.pinMode(P2, OUTPUT, HIGH);
    remote.pinMode(P3, OUTPUT, HIGH);
    remote.pinMode(P4, OUTPUT, HIGH);
    remote.pinMode(P5, OUTPUT, HIGH);
    remote.pinMode(P6, OUTPUT, HIGH);
    remote.pinMode(P7, OUTPUT, HIGH);

    if (remote.begin())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
    }
}

void click(button button)
{
    Serial.print("Click > ");
    Serial.print(button);
    remote.digitalWrite(button, LOW);
    delay(700);
    Serial.print(".");
    remote.digitalWrite(button, HIGH);
    delay(700);
    Serial.println(".");
}