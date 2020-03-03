
#pragma once

#include <Arduino.h>
#include <YUKON_COMMAND.h>

class ROBOT;

class BALANCE : public YUKON_COMMAND
{

    public:
        BALANCE(ROBOT &refRobot);
        ROBOT &Robot;

        void Loop();

        void OISetSpeed(int16_t BalanceMotorSpeed);

        bool ForAsync(long durationMS, int16_t BalanceMotorSpeed, float SlowDown, uint8_t HoldPercent);

        //Each Sensor
        


    private:
        int16_t _CmdBalanceMotorSpeed = 0;
        int16_t _OIBalanceMotorSpeed = 0;


};
