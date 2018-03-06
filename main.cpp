#include <iostream>
#include "ThunderborgDriver.hpp"
#include "MotorController.hpp"

#include "chrono"
#include "thread"

int main()
{
    ThunderborgDriver thunderborgDriver;
    thunderborgDriver.initialize();
    MotorController motorController(thunderborgDriver);
    //motorController.performMove(FORWARD, FORWARD, 1000);
    //motorController.performSpin(-355);
    //motorController.driveBackwords(100);

    char letter;
    while (letter != 'q')
    {
        std::cin >> letter;
        if (letter == 'w')
        {
            motorController.driveForward(10);
        }
        if (letter == 's')
        {
            motorController.driveBackwords(10);
        }
        if (letter == 'a')
        {
            motorController.performSpin(-10);
        }
        if (letter == 'd')
        {
            motorController.performSpin(10);
        }
    }
    /**
    thunderborgDriver.setLedColor(0, 50, 0);
    //thunderborgDriver.test();
    thunderborgDriver.checkForThunderborg();
    float voltage = thunderborgDriver.getBatteryReading();
    std::cout << "Current battery voltage: " << voltage << std::endl;
    //thunderborgDriver.setMotor1(-0.95);
    //thunderborgDriver.setMotor2(-0.95);
    thunderborgDriver.setMotorPower(1, 2, 243);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    thunderborgDriver.motorsOff();
    //thunderborgDriver.setMotor2(0);
    //printf("hello2\n");
    **/
    return 0;
}
