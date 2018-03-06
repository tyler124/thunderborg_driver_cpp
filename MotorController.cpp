#include "MotorController.hpp"

#include "iostream"
#include "chrono"
#include "thread"

const float timeForward1m = 5.7;                     // Number of seconds needed to move about 1 meter
const float timeSpin360   = 4.8;                     // Number of seconds needed to make a full left / right spin

void MotorController::performMove(uint8_t leftDirection, uint8_t rightDirection, uint32_t milliseconds)
{
    thunderborgDriver.setMotorPower(MOTOR_ONE, leftDirection, MAX_POWER);
    thunderborgDriver.setMotorPower(MOTOR_TWO, rightDirection, MAX_POWER);
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    thunderborgDriver.motorsOff();
}

void MotorController::performSpin(float angle)
{
    uint8_t leftDirection;
    uint8_t rightDirection;
    if (angle < 0.0)
    {
        //Left turn
        leftDirection = REVERSE;
        rightDirection = FORWARD;
        //Make the angle positive
        angle *= -1;
    }
    else
    {
        //Right turn
        leftDirection = FORWARD;
        rightDirection = REVERSE;
    }

    double secondsToMove = (angle / 360.0) * timeSpin360;
    uint32_t numMillisecondsToMove = secondsToMove * 1000;

    performMove(leftDirection, rightDirection, numMillisecondsToMove);

}


void MotorController::driveForward(uint32_t centimeters)
{
    double timeForward1cm = (timeForward1m / 100);
    double secondsToMove = timeForward1cm * centimeters;
    uint32_t numMillisecondsToMove = secondsToMove * 1000;

    performMove(FORWARD, FORWARD, numMillisecondsToMove);
}


void MotorController::driveBackwords(uint32_t centimeters)
{
    double timeForward1cm = (timeForward1m / 100);
    double secondsToMove = timeForward1cm * centimeters;
    uint32_t numMillisecondsToMove = secondsToMove * 1000;

    performMove(REVERSE, REVERSE, numMillisecondsToMove);
}
