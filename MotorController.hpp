#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "ThunderborgDriver.hpp"


const uint8_t MAX_POWER = PWM_MAX * 0.95; //Maximum motor power, we limit it to 95% to allow the RPi to get uninterrupted power

class MotorController
{

    public:
        MotorController(ThunderborgDriver& thunderborgDriverInput) : thunderborgDriver(thunderborgDriverInput){};
        void performMove(uint8_t leftDirection, uint8_t rightDirection, uint32_t milliseconds);
        void performSpin(float angle);
        void driveForward(uint32_t centimeters);
        void driveBackwords(uint32_t centimeters);

    private:
        //Private and unimplemented copy constructor
        MotorController( const MotorController& );
        //Private and unimplemented equals operator
        MotorController& operator=( const MotorController& );

        ThunderborgDriver& thunderborgDriver;



};

#endif
