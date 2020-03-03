
#include "BALANCE.h"
#include "ROBOT.h"

BALANCE::BALANCE(ROBOT &refRobot) : Robot(refRobot)
{
}

void BALANCE::Loop()
{
    //Only Run Command Code When Valid
    if (!CmdTimedOut() && CmdIsRunning())
    {
        if (CmdName() == "ForAsync")
        {
            Robot.State.BalanceMotorSpeed = _CmdBalanceMotorSpeed;
            while (CmdUpdatePercent(millis()))
            {
                delay(20);
            }
        }
    }
    Robot.State.BalanceMotorSpeed = _OIBalanceMotorSpeed;

    delay(20);

    long nextLoopTime = millis()+20;
    //Only Run Command Code When Valid
    if (!CmdTimedOut() && CmdIsRunning())
    {
        if (CmdName() == "ForAsync")
        {
            Robot.State.BalanceMotorSpeed = _CmdBalanceMotorSpeed;
            long nextTime = millis();

            while (CmdUpdatePercent(millis()))
            {
                delay(20);
                nextTime = millis() + 25;

                int16_t tmpBalanceMotorSpeed = BurnoutControl(CalcControlLoop(_CmdBalanceMotorSpeed), Robot.State.BalanceMotorSpeed);


                Robot.State.BalanceMotorSpeed = tmpBalanceMotorSpeed;

                while (millis() < nextTime)
                {
                    delay(1);
                }
            }
        }
                        _SlowDown = 0;
    }

    int16_t calculatedBalanceMotorSpeed = _OIBalanceMotorSpeed;
    
    if (BurnoutControlEnabled)
    {
        calculatedBalanceMotorSpeed = BurnoutControl(calculatedBalanceMotorSpeed, Robot.State.BalanceMotorSpeed);
        
    }
    Robot.State.BalanceMotorSpeed = calculatedBalanceMotorSpeed;

    long untilNextRun = nextLoopTime - millis();
    if(untilNextRun > 0)
        delay(untilNextRun);
}

void BALANCE::OISetSpeed(int16_t BalanceMotorSpeed)
{
    _OIBalanceMotorSpeed = BalanceMotorSpeed;
}

bool BALANCE::ForAsync(long durationMS, int16_t BalanceMotorSpeed, float SlowDown, uint8_t HoldUntilPercent)
{
     _SlowDown = SlowDown;

    _CmdBalanceMotorSpeed = BalanceMotorSpeed;
    
    bool retVal = CmdStart("ForAsync", millis(), millis() + durationMS, durationMS + 500);

    while (CmdPercentComplete() < HoldUntilPercent && retVal)
    {
        delay(20);
    }

    return retVal;
}



