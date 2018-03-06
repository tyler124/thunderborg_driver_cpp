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
    
    return 0;
}
