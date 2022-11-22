#ifndef GEN_DEPH_H
#define GEN_DEPH_H

#include <math.h>
#define LN_10 2.30258509
#define a 95

uint8_t inline calculate_gen_depth(uint16_t x)
{
    float f;

    // f(x) = 9 * log_10 (x) - 9
    // Using taylor series 
    // for a = 100
    // P(x) = f(a) + f'(a)*(x-a) + 1/2! * f''(a)*(x-a)^2 + 1/3! * f'''(a)*(x-a)^3 + ... 
    // i get polynomial P(x) = 9 * log_10(100) - 9 + 9*(x-100) / (ln(100) * 100) - 9*(x-100)^2 / (2 * 100^2 * ln(100)) + 3 * (x-100)^3 / (100^3 * ln(100))
    // P(x) = 9 + 9*(x-100)/(ln100 * 100) - 9 * (x-100)^2 / ( 2 * 100^2 * ln100) + 3*(x-100)^3 / (100^3 * ln100) 
    // i used ln10 instead of ln100, to increase the slope, changed some parameters to my liking

    f = 11;
    f +=9*(x-a)/(LN_10 * a);
    f -= 9*(x-a)*(x-a)/(2*a*a* LN_10);
    f += 3*(x-a)*(x-a)*(x-a)/(a*a*a * LN_10);

    //Serial.print(" f(" + String(x) + ") = "+String(f));
    return round(f);
}

#endif