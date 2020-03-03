#ifndef ROBOT_h
#define ROBOT_h

#include <YETI_YUKON.h>
#include <STATE.h>
#include <YUKON_CytronMD10.h>
#include <Encoder.h>
#include <PS4Controller.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <SPI.h>
#include "Subsystems\Drive.h"
#include "Subsystems\Lift.h"
#include "Subsystems\Claw.h"
#include "Subsystems\Balance.h"
#include "Autonomous\AUTONOMOUS.h"

class ROBOT
{
  public:
    ROBOT(YETI_YUKON &rYukon);

    void Setup();
    void GeneralTask();

    void WriteRobot();
    void USBOIGYRO();
    void OLEDLoop();

    YETI_YUKON &Yukon;

    STATE State;

    //Motors
    YUKON_CytronMD10 DriveLeft;
    YUKON_CytronMD10 DriveRight;
    YUKON_CytronMD10 LiftMotor;
    YUKON_CytronMD10 ClawMotor;
    YUKON_CytronMD10 BalanceMotor;

    //Sensors
    Encoder LeftEncoder;
    Encoder RightEncoder;
    Encoder LiftEncoder;

    //Subsystems
    DRIVE Drive;
    LIFT Lift;
    CLAW Claw;
    BALANCE Balance;
    AUTONOMOUS Auton;

    //USB Items

    
    
   

    //PIN Declarations
    static const int _DriveLeftPWM = 0;
    static const int _DriveLeftDir = 0;
    static const bool _DriveLeftReversed = false;
        
    static const int _DriveRightPWM = 1;
    static const int _DriveRightDir = 1;
    static const bool _DriveRightReversed = false;
        
    static const int _LiftMotorPWM = 2;
    static const int _LiftMotorDir = 2;
    static const bool _LiftMotorReversed = true;
        
    static const int _ClawMotorPWM = 3;
    static const int _ClawMotorDir = 3;
    static const bool _ClawMotorReversed = false;
        
    static const int _BalanceMotorPWM = 4;
    static const int _BalanceMotorDir = 4;
    static const bool _BalanceMotorReversed = false;
        

    
    static const int _LeftEncoderPin1 = 21;
    static const int _LeftEncoderPin2 = 22;
        
    static const int _RightEncoderPin1 = 17;
    static const int _RightEncoderPin2 = 14;
        
    static const int _LiftEncoderPin1 = 999;
    static const int _LiftEncoderPin2 = 999;

    static const int EnablePin = 32;
    bool PrecisionMode = false;
    int16_t DriveRightSpeed = 0;
    int16_t DriveLeftSpeed = 0;
    int16_t LiftSpeed = 0;
    int16_t ClawSpeed = 0;
    int16_t BalanceSpeed = 0;
    long _NextModeMillis = 0;
    bool EnableVal = false;

    static const uint8_t _Button0 = 0;
    static const uint8_t _LEDBuiltIn = 25;
    static const uint8_t _AutonLightSensor = 0;
    
    static const uint16_t _AutonLightSensorThreshold = 150; //Value 0 - 1024

  private:
    uint16_t LightSensorVal = 0;
    uint16_t MaxLightSensorVal = 0;
};

#endif