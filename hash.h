#pragma once

#include <string>
#include <sstream>
#include <random>

std::string Hash (std::string input)
{
    int output [64];
    std::fill_n(output, 64, 15);
    
    int sum = 0;

    for (char character : input)
    {
        std::mt19937 chargen(character);
        sum+=chargen();
    };

    std::mt19937 gen(sum);
    std::uniform_int_distribution<> distr(0, 15);

    std::stringstream stream;
    for (int num : output)
    {   
        stream << std::hex << num-distr(gen);
    };
    
    return stream.str();
}