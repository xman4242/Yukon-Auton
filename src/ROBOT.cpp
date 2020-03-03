#include "ROBOT.h"

ROBOT::ROBOT(YETI_YUKON &rYukon) : Yukon(rYukon),
     DriveLeft(_DriveLeftPWM, &Yukon.PWM, _DriveLeftDir, &Yukon.GPIO, _DriveLeftReversed), 
     DriveRight(_DriveRightPWM, &Yukon.PWM, _DriveRightDir, &Yukon.GPIO, _DriveRightReversed), 
     LiftMotor(_LiftMotorPWM, &Yukon.PWM, _LiftMotorDir, &Yukon.GPIO, _LiftMotorReversed), 
     ClawMotor(_ClawMotorPWM, &Yukon.PWM, _ClawMotorDir, &Yukon.GPIO, _ClawMotorReversed), 
     BalanceMotor(_BalanceMotorPWM, &Yukon.PWM, _BalanceMotorDir, &Yukon.GPIO, _BalanceMotorReversed), 
     LeftEncoder(_LeftEncoderPin1, _LeftEncoderPin2), 
     RightEncoder(_RightEncoderPin1, _RightEncoderPin2), 
     LiftEncoder(_LiftEncoderPin1, _LiftEncoderPin2), 
    Drive(*this),
    Lift(*this),
    Claw(*this),
    Balance(*this),
    Auton(*this)
    
        
    

{
}

//Init Pins, motor, and PS4
void ROBOT::Setup()
{
    DriveLeft.Init();
    DriveRight.Init();
    LiftMotor.Init();
    ClawMotor.Init();
    BalanceMotor.Init();
    PS4.begin("20:20:20:20:20:20"); 
    Serial.println("PS4 Ready");

    pinMode(_Button0, INPUT_PULLUP);
    pinMode(_LEDBuiltIn, OUTPUT);
    pinMode(EnablePin, INPUT_PULLUP);
    digitalWrite(_LEDBuiltIn, LOW);
}

//Wifi, gyro, sensor read
void ROBOT::GeneralTask()
{   
    //Wifi and Gyro Setup
    if (digitalRead(_Button0) == LOW)
    {
        delay(1000);
        if (digitalRead(_Button0) == LOW)
        {
            Yukon.DisableWatchdog();

            Yukon.OLED.clear();
            Yukon.OLED.drawString(0, 0, "GYRO CALIBRATION!");
            Yukon.OLED.display();

            delay(1000);
            Yukon.GYRO.RunCalibration();
        }
        else
        {
            //else toggle wifi
            Yukon.ToggleWIFI();
        }
    }

    //Read The Enable Pin
    State.AutonLightSensorActive = EnableVal;
    State.AutonLightSensorActive = digitalRead(EnablePin);


}

//Writes to motor contollers. No need to change
void ROBOT::WriteRobot()
{
    if (Yukon.IsDisabled())
    {
    DriveLeft.SetMotorSpeed(0);
    DriveRight.SetMotorSpeed(0);
    LiftMotor.SetMotorSpeed(0);
    ClawMotor.SetMotorSpeed(0);
    BalanceMotor.SetMotorSpeed(0);
    }
    else
    {

    DriveLeft.SetMotorSpeed(State.DriveLeftSpeed);
    DriveRight.SetMotorSpeed(State.DriveRightSpeed);
    LiftMotor.SetMotorSpeed(State.LiftMotorSpeed);
    ClawMotor.SetMotorSpeed(State.ClawMotorSpeed);
    BalanceMotor.SetMotorSpeed(State.BalanceMotorSpeed);
    }
}

//Read contoller and Gyro
void ROBOT::USBOIGYRO()
{
        DriveLeftSpeed = (PS4.data.analog.stick.ly)*2;
        DriveRightSpeed = (PS4.data.analog.stick.ry)*2;
        BalanceSpeed = (PS4.data.analog.stick.rx)*2;

        if(DriveRightSpeed < -255) DriveRightSpeed = -255;
        if(DriveLeftSpeed < -255) DriveLeftSpeed = -255;

        if(abs(DriveLeftSpeed) < 11)
        {
            DriveLeftSpeed = 0;
        }

        if(abs(DriveRightSpeed) < 11)
        {
            DriveRightSpeed = 0;
        }

        if(PrecisionMode)
        {
            DriveLeftSpeed = PS4.data.analog.stick.ly;
            DriveRightSpeed = PS4.data.analog.stick.ry;
        }
           
        LiftSpeed = (PS4.data.analog.button.r2)-(PS4.data.analog.button.l2);
        ClawSpeed = (PS4.data.button.l1-PS4.data.button.r1)*255;
        Drive.OISetSpeed(DriveLeftSpeed,DriveRightSpeed);
        Lift.OISetSpeed(LiftSpeed);
        Claw.OISetSpeed(ClawSpeed);
        Balance.OISetSpeed(BalanceSpeed);

        if(_NextModeMillis < millis())
        {
            if(PS4.data.button.circle == 1) 
            {
                PrecisionMode = !PrecisionMode;
            }
            if (PS4.data.button.left == 1) Auton.QueuePrev();

            if (PS4.data.button.right == 1) Auton.QueueNext();

            if (PS4.data.button.down == 1) Auton.ToggleArmed();

            if(PS4.data.button.up == 1)
            {
                EnableVal = true;
            }
            _NextModeMillis = millis() + 150;
        }
    Yukon.GYRO.Loop();
}

int ScreenToShow = 0;
unsigned long SecondsPerScreen = 5000;
unsigned long NextScreen = SecondsPerScreen;

//Print to the ESP32 display and PS4 Led
void ROBOT::OLEDLoop()
{   
    
    
    if (Auton.IsArmLocked())
    {
        Yukon.OLED.clear();

        Yukon.OLED.drawString(0, 0, "LOCKED! ");

        Yukon.OLED.drawString(0, 10, Auton.QueuedProgramName());

        Yukon.OLED.display();
    }
    else if (Auton.IsArmed())
    {
        Yukon.OLED.clear();

        Yukon.OLED.drawString(0, 0, "ARMED " + String(LightSensorVal));
        Yukon.OLED.drawString(0, 10, Auton.QueuedProgramName());

        Yukon.OLED.display();
    }
    else if (Auton.QueuedProgramName() != "")
    {
        Yukon.OLED.clear();

        Yukon.OLED.drawString(0, 0, Auton.QueuedProgramName());

        Yukon.OLED.display();
    }
    else
    {
        if (State.AutonLightSensorActive)
        {
            Yukon.OLED.invertDisplay();
        }
        else
        {
            Yukon.OLED.normalDisplay();
        }


        if (ScreenToShow == 0)
        {
            Yukon.OLED.clear();
            Yukon.OLED.setFont(ArialMT_Plain_16);

                
                Yukon.OLED.drawString(0, 0, "Left: " + String(LeftEncoder.read()));
                
                
                Yukon.OLED.drawString(0, 16, "Righ: " + String(RightEncoder.read()));
                
                
                Yukon.OLED.drawString(0, 32, "Lift: " + String(LiftEncoder.read()));
                

            //Yukon.OLED.drawString(0, 0, "Left: " + String(DriveLeftEnc.read()));

            //Yukon.OLED.drawString(0, 16, "Right: " + String(DriveRightEnc.read()));

            //Yukon.OLED.drawString(0, 32, "Cat: " + String(CatapultMotorEnc.read()));

            Yukon.OLED.drawString(0, 48, "Head: " + String(Yukon.GYRO.Heading()));

            Yukon.OLED.display();

            if (millis() > NextScreen)
            {
                ScreenToShow++;
                NextScreen = millis() + SecondsPerScreen;
            }
        }
        else if (ScreenToShow == 1)
        {

            Yukon.OLED.clear();
            Yukon.OLED.setFont(ArialMT_Plain_16);

            Yukon.OLED.drawString(0, 0, "Ch A: " + String(Yukon.ChAVolts()));

            Yukon.OLED.drawString(0, 16, "Ch B: " + String(Yukon.ChBVolts()));

            Yukon.OLED.drawString(0, 32, "Ch C: " + String(Yukon.ChCVolts()));

            Yukon.OLED.drawString(0, 48, "LS: " + String(State.AutonLightSensorState));

            Yukon.OLED.display();

            if (millis() > NextScreen)
            {
                ScreenToShow = 0;
                NextScreen = millis() + SecondsPerScreen;
            }
        }

        if(PrecisionMode)
        {
            PS4.setLed(0,255,0);
        }

        if(!PrecisionMode)
        {
            PS4.setLed(0,0,255);
        }
        PS4.sendToController();
    }
}