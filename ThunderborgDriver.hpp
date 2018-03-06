#ifndef THUNDERBORG_DRIVER_H
#define THUNDERBORG_DRIVER_H

#include <stdint.h>

//Return statuses
const int32_t DRIVER_SUCCESS = 0;
const int32_t DRIVER_ERROR = -1;
const int32_t DRIVER_UNINITIALIZED_ERROR = -2;

//Max motor power
const uint8_t PWM_MAX = 255;
const uint8_t FORWARD = 1;
const uint8_t REVERSE = 2;
const uint8_t MOTOR_ONE = 1;
const uint8_t MOTOR_TWO = 2;

class ThunderborgDriver
{
    public:
        ThunderborgDriver() : file_i2c(0), initialized(false) {};
        int32_t initialize();
        bool checkForThunderborg();
        int32_t setLedColor(uint8_t red, uint8_t green, uint8_t blue);
        float getBatteryReading();
        int32_t setMotorPower(uint8_t motorNumber, uint8_t direction, uint8_t power);
        int32_t motorsOff();

    private:
        //Private and unimplemented copy constructor
        ThunderborgDriver( const ThunderborgDriver& );
        //Private and unimplemented equals operator
        ThunderborgDriver& operator=( const ThunderborgDriver& );

        int32_t rawWrite(const char* data, const uint8_t dataLength);
        int32_t readCommandResponse(const uint8_t command, const uint8_t length, char* buffer);
        int32_t rawRead(const uint8_t command, const uint8_t length, char* buffer);
        bool validateDirection(uint8_t direction);
        uint8_t determineMotorCommand(uint8_t motorNumber, uint8_t direction);

    private:
        int file_i2c;
        bool initialized;



};

#endif
