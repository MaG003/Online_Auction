#include "randomID.h"
#include <iostream>
#include <random>
#include <ctime>

std::string generateRandomString()
{
    int length = 30;
    std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string randomString;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    for (int i = 0; i < length; ++i)
    {
        randomString += characters[dis(gen)];
    }

    return randomString;
}