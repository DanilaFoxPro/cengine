#include "utility.h"

void clampf( float* Value, const float Min, const float Max )
{
        if( *Value < Min ) {
                *Value = Min;
        } else if( *Value > Max ) {
                *Value = Max;
        }
}
