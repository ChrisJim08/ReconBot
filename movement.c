#include "movement.h"

void move_foward(oi_t *sensor, double cm ) {
    oi_init(sensor);
    double i = 0;
    double mm = cm*10;

    oi_setWheels(100, 100);

    while(i < mm) {
        oi_update(sensor);
        i += sensor->distance;

        if (sensor->bumpLeft) {
            go_around_right(sensor);
            oi_init(sensor);
            oi_setWheels(100, 100);
            i += 1;
        }
        else if (sensor->bumpRight) {
            go_around_left(sensor);
            oi_init(sensor);
            oi_setWheels(100, 100);
            i += 1;
        }
    }

    oi_setWheels(0, 0); // stop
    oi_free(sensor);

}

void move_backward(oi_t *sensor, double cm ) {
        oi_init(sensor);
        double i = 0;
        double mm = cm*10;
        oi_setWheels(-100, -100);
        while(i < mm) {
            oi_update(sensor);
            i -= sensor->distance;
        }
        oi_setWheels(0, 0); // stop
        oi_free(sensor);
    }

void turn_clockwise(oi_t *sensor, int deg) {
    oi_init(sensor);
    double i = 0;

    oi_setWheels(50, -50);

    while(i < deg) {
        oi_update(sensor);
        i += sensor->angle;
    }

    oi_setWheels(0, 0); // stop
    oi_free(sensor);
}

void turn_cclockwise(oi_t *sensor, int deg) {
    oi_init(sensor);
    int i = 0;

    oi_setWheels(-50, 50);

    while(i > (-deg)) {
        oi_update(sensor);
        i += sensor->angle;
    }

    oi_setWheels(0, 0); // stop
    oi_free(sensor);
}

void go_around_left(oi_t *sensor) {
        move_backward(sensor, 5);
        turn_clockwise(sensor, 90);
        move_foward(sensor, 9);
        turn_cclockwise(sensor, 90);
        move_foward(sensor, 5);
        turn_cclockwise(sensor, 22);
}
void go_around_right(oi_t *sensor) {
        move_backward(sensor, 5);
        turn_cclockwise(sensor, 90);
        move_foward(sensor, 9);
        turn_clockwise(sensor, 90);
        move_foward(sensor, 5);
        turn_clockwise(sensor, 22);

}
