/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Color
*/

#ifndef COLOR_HPP_
#define COLOR_HPP_

typedef enum Color_e{
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    BLACK,
    TRANSPARENT
} Color_t;

class Color {
    public:
        Color();
        Color(Color_t color);
        ~Color();
        Color_t getColor() const;
        void setColor(Color_t newColor);

    private:
        Color_t color;
};

#endif /* !COLOR_HPP_ */
