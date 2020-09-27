#include <cengine/cengine.h>

#define CLAMP_TEMPLATE() if( *Value < Min ) { *Value = Min; } else if( *Value > Max ) { *Value = Max; }

void clampf( float* Value, const float Min, const float Max )
{
        CLAMP_TEMPLATE();
}

void clampui( unsigned int* Value, const unsigned int Min, const unsigned int Max )
{
        CLAMP_TEMPLATE();
}

void check_memory()
{
        f_debug_memory();
}
