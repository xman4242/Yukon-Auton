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
                                   Auton(*this),
    
        Btd(&Usb), PSx(&Btd) //, PAIR)
    

{
}

void ROBOT::Setup()
{
    Usb.Init();
    DriveLeft.Init();
    DriveRight.Init();
    LiftMotor.Init();
    ClawMotor.Init();
    BalanceMotor.Init();

    pinMode(_Button0, INPUT_PULLUP);
    pinMode(_LEDBuiltIn, OUTPUT);
}

void ROBOT::GeneralTask()
{
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
    //Read The Light Sensor
    LightSensorVal = Yukon.ADC.readADC(_AutonLightSensor);

    //Keep track of the max light sensor value since power on.
    if (LightSensorVal > MaxLightSensorVal)
        MaxLightSensorVal = LightSensorVal;

    // Make sure the light sensor pin hasn't been detecting light since the robot turned on.
    // This is a sign that the light sensor may be disconnected, or is not shielded enough.
    // Or that the threshold is set too close to zero.
    if (MaxLightSensorVal > _AutonLightSensorThreshold)
    {
        bool tempActive = (LightSensorVal <= _AutonLightSensorThreshold);

        if (State.AutonLightSensorActive != tempActive)
        {
            //Change in light sensor
            if (State.AutonLightSensorState > 0)
                State.AutonLightSensorState++;

            switch (State.AutonLightSensorState)
            {
            case 4:
                Yukon.Disable();
            case 5:
                Yukon.Enable();
            }

            State.AutonLightSensorActive = tempActive;
        }
    }
}

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

void ROBOT::USBOIGYRO()
{
     //Read The Controller
    Usb.Task();
        
            if (PSx.connected())
            {
        Drive.OISetSpeed(Yukon.JoystickTo255(PSx.getAnalogHat(LeftHatY), 10),Yukon.JoystickTo255(PSx.getAnalogHat(RightHatY), 10));
        Lift.OISetSpeed(PSx.getAnalogButton(R2) - PSx.getAnalogButton(L2));
        Claw.OISetSpeed((PSx.getButtonPress(R1)*255) - (PSx.getButtonPress(L1)*255));
        Balance.OISetSpeed(Yukon.JoystickTo255(PSx.getAnalogHat(LeftHatY), 10));
    
        if (PSx.getButtonClick(LEFT))
        Auton.QueuePrev();
        if (PSx.getButtonClick(RIGHT))
        Auton.QueueNext();
        if (PSx.getButtonClick(DOWN))
        Auton.ToggleArmed();


        if (PSx.getButtonClick(X))
        {
            if (Auton.IsRunning())
            {
                Yukon.Disable();
            }
            else
            {
                Auton.LaunchQueued();
            }
        }

        if (PSx.getButtonClick(SQUARE))
        Auton.ToggleLockArmed();

        if (PSx.getButtonClick(PS))
        PSx.disconnect();
        }
    


    Yukon.GYRO.Loop();
}
int ScreenToShow = 0;
unsigned long SecondsPerScreen = 5000;
unsigned long NextScreen = SecondsPerScreen;

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
    }
}