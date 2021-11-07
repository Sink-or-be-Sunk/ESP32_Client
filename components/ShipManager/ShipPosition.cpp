#include "ShipPosition.h"

// static ship_position_t size_to_type(int size)
// {
//     if (size < 0)
//     {
//         size *= -1; // absolute val
//     }

//     if (size > 4 || size < 1) // valid sizes from subtracting of [1, 4]
//     {
//         return INVALID;
//     }

//     return (ship_position_t)(size - 1); // map [1,4] -> [0,3]
// }

// void ShipPosition::position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
// {
//     this->front_r = r1;
//     this->front_c = c1;
//     this->back_r = r2;
//     this->back_c = c2;

//     if (this->front_r == this->back_r)
//     {
//         // ship is horizontal
//         this->type = size_to_type(this->front_c - this->back_c);
//         this->isReady = true;
//         return;
//     }
//     else if (this->front_c == this->back_c)
//     {
//         // ship is horizontal
//         this->type = size_to_type(this->front_r - this->back_r);
//         this->isReady = true;
//         return;
//     }

//     this->type = INVALID;
// }
