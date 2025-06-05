/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Color
*/

#include "Color.hpp"

Color::Color() : color(WHITE)
{
}

Color::Color(Color_t color) : color(color)
{
}

Color::~Color()
{
}


Color_t Color::getColor() const
{
    return color;
}

void Color::setColor(Color_t newColor)
{
    color = newColor;
}
