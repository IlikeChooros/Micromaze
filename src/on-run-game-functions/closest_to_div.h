#ifndef CLOSETS_TO_DIV_H
#define CLOSETS_TO_DIV_H

uint8_t inline closest_to_divider(uint8_t num, uint8_t divisors[], uint8_t number_of_divsors, uint8_t *idx)
{
    int16_t val,min=250, abs_val;
    for (uint8_t i=0;i<number_of_divsors;i++)
    {
        val = divisors[i] - num;
        abs_val = abs(val);
        //Serial.print("I: "+String(i) + " div["+String(i)+"] = "+String(divisors[i]) + " VAL: "+String(val) + " ABS: "+String(abs_val));
        if (abs_val < min && val <= 0)
        {
            min = abs_val;
            *idx = i;
        }
        //Serial.println(" ---MIN:"+String(min) + " IDX MIN: "+String(*idx));
    }
    return divisors[*idx];
}

#endif