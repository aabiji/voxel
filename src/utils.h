#pragma once

#include <iostream>

enum class Logtype { INFO, ERROR, WARNING };

static void log(std::string tag, std::string message, Logtype type = Logtype::INFO)
{
    int color = type == Logtype::ERROR ? 31 : type == Logtype::WARNING ? 33 : 36;
    std::cout << std::format("\x1b[1;{}m[{}]: {}\u001b[0m\n", color, tag, message);
}
