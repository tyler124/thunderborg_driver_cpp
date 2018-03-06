#include "ThunderborgDriver.hpp"

#include <iostream>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

const uint8_t I2C_MAX_LEN = 6;
const float VOLTAGE_PIN_MAX = 36.3;  // Maximum voltage from the analog voltage monitoring pin
const float VOLTAGE_PIN_CORRECTION = 0.0;   // Correction value for the analog voltage monitoring pin
const uint8_t I2C_ID_THUNDERBORG = 0x15;


const uint8_t COMMAND_SET_LEDS      = 5;
const uint8_t COMMAND_GET_BATT_VOLT = 21;    // Get the battery voltage reading
const uint8_t COMMAND_GET_ID        = 0x99;  // Get the board identifier
const uint8_t COMMAND_SET_A_FWD     = 8;     // Set motor A PWM rate in a forwards direction
const uint8_t COMMAND_SET_A_REV     = 9;     // Set motor A PWM rate in a reverse direction
const uint8_t COMMAND_SET_B_FWD     = 11;    // Set motor B PWM rate in a forwards direction
const uint8_t COMMAND_SET_B_REV     = 12;    // Set motor B PWM rate in a reverse direction
const uint8_t COMMAND_ALL_OFF       = 14;     // Switch everything off

const uint16_t COMMAND_ANALOG_MAX = 0x3FF; //Maximum value for analog readings

const uint8_t READ_BUFFER_LENGTH = I2C_MAX_LEN + 1; //Leave room for null terminator

const char* I2C_BUS = "/dev/i2c-1";

int32_t ThunderborgDriver::initialize()
{
    file_i2c = open(I2C_BUS, O_RDWR);
    if (file_i2c < 0)
    {
        std::cerr << "Error: Failed to open the i2c bus" << std::endl;
        return file_i2c;
    }
    int status = ioctl(file_i2c, I2C_SLAVE, I2C_ID_THUNDERBORG);
    if (status < 0)
    {
        std::cerr << "Error: Failed to acquire bus access and/or talk to slave" << std::endl;
        return status;
    }
    else
    {
        //Driver successfully initialized
        initialized = true;
    }

    return DRIVER_SUCCESS;
}


int32_t ThunderborgDriver::rawWrite(const char* data, const uint8_t dataLength)
{
    int32_t bytesWritten = 0;

    if (data == nullptr)
    {
        std::cerr << "Write error: null pointer" << std::endl;
        return DRIVER_ERROR;
    }
    else if (dataLength > I2C_MAX_LEN)
    {
        std::cerr << "Error: data exceeds maximum allowed i2c length" << std::endl;
        return DRIVER_ERROR;
    }
    else
    {
        bytesWritten = write(file_i2c, data, dataLength);
        if (bytesWritten < dataLength)
        {
            std::cerr << "Error writing to i2c" << std::endl;
            return DRIVER_ERROR;
        }
    }

    return bytesWritten;
}


int32_t ThunderborgDriver::readCommandResponse(const uint8_t command, const uint8_t length, char* buffer)
{

    int32_t bytesRead = 0;

    /*
    * Read from i2c until either the command reply is found or we exhaust our number of retries.
    */
    bool commandReplyFound = false;
    uint8_t retryCount = 3;
    while (!commandReplyFound && (retryCount > 0))
    {
        bytesRead = read(file_i2c, buffer, length);
        if (bytesRead != length)
        {
            std::cerr << "Error reading from i2c bus" << std::endl;
            return DRIVER_ERROR;
        }
        else
        {
            if (buffer[0] == command)
            {
                commandReplyFound = true;
                break;
            }
            else
            {
                retryCount--;
            }
        }
    }
    if (!commandReplyFound)
    {
        std::cerr << "Unable to read command reply from i2c bus" << std::endl;
        return DRIVER_ERROR;
    }

    return bytesRead;
}


int32_t ThunderborgDriver::rawRead(const uint8_t command, const uint8_t length, char* buffer)
{
    int32_t bytesRead = 0;

    /*
    * First write the GET command
    */
    const uint8_t GET_COMMAND_DATA_SIZE = 1;
    //Leave room for null-terminator
    uint8_t GET_COMMAND_BUFFER_SIZE = GET_COMMAND_DATA_SIZE + 1;
    char getCommandData[GET_COMMAND_BUFFER_SIZE] = "";
    getCommandData[0] = command;
    int32_t writeStatus = rawWrite(getCommandData, GET_COMMAND_DATA_SIZE);

    if (writeStatus == DRIVER_ERROR)
    {
        std::cerr << "Error writing get command to i2c bus" << std::endl;
        return DRIVER_ERROR;
    }
    else
    {
        bytesRead = readCommandResponse(command, length, buffer);
    }

    return bytesRead;
}

bool ThunderborgDriver::checkForThunderborg()
{
    bool thunderborgFound = false;

    char readBuffer[READ_BUFFER_LENGTH] = "";
    int32_t bytesRead = rawRead(COMMAND_GET_ID, I2C_MAX_LEN, readBuffer);
    if (bytesRead != I2C_MAX_LEN)
    {
        std::cerr << "Could not find Thunderborg!" << std::endl;
    }
    else
    {
        if (readBuffer[1] == I2C_ID_THUNDERBORG)
        {
            printf("Found Thunderborg at i2c address %#02x\n", I2C_ID_THUNDERBORG);
            thunderborgFound = true;
        }
        else
        {
            printf("Found device but it is not a Thunderborg! (%#02x\n instead of %#02x\n)\n", readBuffer[1], I2C_ID_THUNDERBORG);
        }
    }

    return thunderborgFound;
}


int32_t ThunderborgDriver::setLedColor(uint8_t red, uint8_t green, uint8_t blue)
{
    const uint8_t RGB_MAX_VALUE = 255;
    int32_t status = DRIVER_SUCCESS;

    if (!initialized)
    {
        std::cerr << "Error: driver not initialized" << std::endl;
        return DRIVER_UNINITIALIZED_ERROR;
    }
    else if ( (red > RGB_MAX_VALUE) || (green > RGB_MAX_VALUE) || (blue > RGB_MAX_VALUE) )
    {
        std::cerr << "Error: invalid input LED color value" << std::endl;
        status = DRIVER_ERROR;
    }
    else
    {
        const uint8_t SET_LED_DATA_SIZE = 4;
        //Leave room at the end for null-terminator
        const uint8_t LED_DATA_BUFFER_SIZE = SET_LED_DATA_SIZE + 1;
        char setLedData[LED_DATA_BUFFER_SIZE] = "";

        setLedData[0] = COMMAND_SET_LEDS;
        setLedData[1] = red;
        setLedData[2] = green;
        setLedData[3] = blue;

        status = rawWrite(setLedData, SET_LED_DATA_SIZE);
    }

    return status;
}

float ThunderborgDriver::getBatteryReading()
{
    float voltage = 0;

    char readBuffer[READ_BUFFER_LENGTH] = "";
    int32_t bytesRead = rawRead(COMMAND_GET_BATT_VOLT, I2C_MAX_LEN, readBuffer);
    if (bytesRead != I2C_MAX_LEN)
    {
        std::cerr << "Error: Could not read battery voltage" << std::endl;
        return DRIVER_ERROR;
    }
    else
    {
        float raw = (readBuffer[1] << 8) + readBuffer[2];
        float level = raw / static_cast<float>(COMMAND_ANALOG_MAX);
        level *= VOLTAGE_PIN_MAX;
        float correctedVoltage = level + VOLTAGE_PIN_CORRECTION;
        voltage = correctedVoltage;
    }

    return voltage;
}


bool ThunderborgDriver::validateDirection(uint8_t direction)
{
    if ( (direction != FORWARD) && (direction != REVERSE) )
    {
        return false;
    }

    return true;
}


uint8_t ThunderborgDriver::determineMotorCommand(uint8_t motorNumber, uint8_t direction)
{
    uint8_t motorCommand = 0;

    if (motorNumber == 1)
    {
            if (direction == FORWARD)
            {
                motorCommand = COMMAND_SET_A_FWD;
            }
            else
            {
                motorCommand = COMMAND_SET_A_REV;
            }
    }
    else
    {
        if (direction == FORWARD)
        {
            motorCommand = COMMAND_SET_B_FWD;
        }
        else
        {
            motorCommand = COMMAND_SET_B_REV;
        }
    }

    return motorCommand;
}

int32_t ThunderborgDriver::setMotorPower(uint8_t motorNumber, uint8_t direction, uint8_t power)
{
    int32_t status = DRIVER_ERROR;

    bool validDirection = validateDirection(direction);

    if (!validDirection)
    {
        std::cerr << "Error: invalid direction" << std::endl;
        return DRIVER_ERROR;
    }
    else if ( (motorNumber != MOTOR_ONE) && (motorNumber != MOTOR_TWO) )
    {
        std::cerr << "Error: invalid motor number" << std::endl;
        return DRIVER_ERROR;
    }
    else if (!initialized)
    {
        std::cerr << "Error: driver not initialized" << std::endl;
        return DRIVER_UNINITIALIZED_ERROR;
    }
    else
    {
        uint8_t motorCommand = determineMotorCommand(motorNumber, direction);
        if (motorCommand == 0)
        {
            std::cerr << "Error determining motor command" << std::endl;
            return DRIVER_ERROR;
        }
        else
        {
            const uint8_t SET_MOTOR_DATA_SIZE = 2;
            //Leave room at the end for null-terminator
            const uint8_t MOTOR_DATA_BUFFER_SIZE = SET_MOTOR_DATA_SIZE + 1;
            char setMotorData[MOTOR_DATA_BUFFER_SIZE] = "";

            if (power > PWM_MAX)
            {
                power = PWM_MAX;
            }

            setMotorData[0] = motorCommand;
            setMotorData[1] = power;
            status = rawWrite(setMotorData, SET_MOTOR_DATA_SIZE);
        }
    }

    return status;
}


int32_t ThunderborgDriver::motorsOff()
{
    const uint8_t SET_ALL_OFF_DATA_SIZE = 1;
    //Leave room at the end for null-terminator
    const uint8_t SET_ALL_OFF_BUFFER_SIZE = SET_ALL_OFF_DATA_SIZE + 1;
    char setAllOffData[SET_ALL_OFF_BUFFER_SIZE] = "";

    setAllOffData [0] = COMMAND_ALL_OFF;

    int32_t status = rawWrite(setAllOffData, SET_ALL_OFF_DATA_SIZE);

    return status;
}

