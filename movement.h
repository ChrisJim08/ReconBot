#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdio.h>
#include "open_interface.h"

void move_foward(oi_t *sensor, double cm );
void turn_clockwise(oi_t *sensor, int deg);
void go_around_left(oi_t *sensor);
void go_around_right(oi_t *sensor);
void move_backward(oi_t *sensor, double cm );
void turn_cclockwise(oi_t *sensor, int deg);

#endif //MOVEMENT_H
