#include "final.h"

int main(void) {
    initialize();

    //socket_echo();
    while(1){
        //while(!flag);
        //switch (uart_data) {
        switch ('o') {
            case 'm':
                scan(); socket_response('m'); break;
            case 'w':
                move_foward(sensor_data, 1); socket_response('w'); break;
            case 'a':
                turn_clockwise(sensor_data, 10); socket_response('a'); break;
            case 's':
                move_backward(sensor_data, 1); socket_response('s'); break;
            case 'd':
                turn_cclockwise(sensor_data, 10); socket_response('d'); break;
            case 'h':
                scan(); auto_move(); socket_response('h'); break;
            case 'c':
                calibrate(); socket_response('c'); break;
            case 'o':
                while (1) {
                    sprintf(buffer, "%d   %d", total_x_distance, total_y_distance);
                    lcd_printf(buffer);
                }
                break;
        }
        flag = 0;
    }
}

void scan() {
    int angle = 0, in_obj = 0, num_obj = 0;
    float distances[93], close_tol = 0.25, far_tol = 0.30, clean_tol = 0.25;
    typedef struct { float distance; int radial_width; float width;
        int start_angle; int end_angle; int mid_angle;} object;
    object obj_arr[10]; distances[91] = 6.00; distances[92] = 6.00;
    float last_curr, curr_next, last_next, last_2next;                                                                                                                          //float curr_2/3next;
    int smallest = 0;

    uart_sendStr("Angle(Degrees)  Distance(cm)\r\n\0");

    servo_move(0, angle); timer_waitMillis(500);
    while(angle <= 180) {
        servo_move(0, angle); timer_waitMillis(300);
        int ir = adc_read(); distances[angle/2] = a2*pow(ir, 2) + a1*ir + a0;

       sprintf(buffer, "%d\t\t%.2f\r\n", angle, distances[angle/2]); uart_sendStr(buffer);

       angle += 2;
    }
    uart_sendStr("END\n");

    for(i = 1; i < 91; i++) { //DETECT OBJECTS
        last_curr = fabs(distances[i-1] - distances[i]);
        last_next = fabs(distances[i-1] - distances[i+1]);
        last_2next = fabs(distances[i] - distances[i+2]);
        curr_next = fabs(distances[i] - distances[i+1]); //curr_2next = fabs(distances[i] - distances[i+2]);

        if (last_next < clean_tol) { //Cleaning up data
            distances[i] = (distances[i-1] + distances[i+1])/2;
        }
        else if ((curr_next < clean_tol) && (last_2next < clean_tol)) {
            distances[i] = (distances[i-1] + distances[i+2])/2;
        }


        if ( (last_curr > far_tol) && (curr_next < close_tol) && (in_obj == 0) && (distances[i] <= .80) ) { //Enter Object                                                      //curr_2next < min_tol ehhh
            obj_arr[num_obj].start_angle = (i-1)*2;
            in_obj = 1;
        }

        //Exit Object
        else if ( (distances[i+1] > .95) && (in_obj == 1) ) {                                                                                                                   //((curr_2next > far_tol) || (curr_3next > far_tol)) ehhhhhhhhh
            if (((obj_arr[num_obj].start_angle - obj_arr[num_obj-1].end_angle) < 10) && (num_obj > 0) ) {
                num_obj--;
            }
            obj_arr[num_obj].end_angle = (i-1)*2;
            obj_arr[num_obj].radial_width = obj_arr[num_obj].end_angle - obj_arr[num_obj].start_angle;
            obj_arr[num_obj].mid_angle = obj_arr[num_obj].start_angle + (obj_arr[num_obj].radial_width/2);
            if (obj_arr[num_obj].radial_width > 6) {                                                                                                                            //??????????????????
                num_obj++;
            }
            in_obj = 0;
        }
    }

    if(num_obj > 0) { //OBJECT LOGIC
        uart_sendStr("Object#         Angle\t\tDistance        Width\r\n\0");

        servo_move(0, obj_arr[0].mid_angle+1);
        timer_waitMillis(1000);
        obj_arr[0].distance = ping_read();
        obj_arr[0].width = (2 * 3.14  * obj_arr[0].distance * obj_arr[0].radial_width) / 360.0;
        sprintf(buffer, "%d\t\t%d\t\t%.2f\t\t%.2f\r\n", 1,obj_arr[0].mid_angle, obj_arr[0].distance, obj_arr[0].width); uart_sendStr(buffer);

        for (i = 1; i < num_obj; i++) {
            servo_move(0, obj_arr[i].mid_angle+1);
            timer_waitMillis(1000);
            obj_arr[i].distance = ping_read();
            obj_arr[i].width = (2 * 3.14 * obj_arr[i].distance * obj_arr[i].radial_width) / 360.0;
            sprintf(buffer, "%d\t\t%d\t\t%.2f\t\t%.2f\r\n", i+1,obj_arr[i].mid_angle, obj_arr[i].distance, obj_arr[i].width); uart_sendStr(buffer);

            if((obj_arr[i].width < obj_arr[smallest].width)) {
                smallest = i;
            }
        }
        small_deg = obj_arr[smallest].mid_angle;
        small_distance = obj_arr[smallest].distance;
        sprintf(buffer, "%d %d\r\n", smallest+1, small_deg); uart_sendStr(buffer);
    }
}

void auto_move() {
    sprintf(buffer, "Moving %.2f cms towards %d degrees\r\n", small_distance, small_deg); uart_sendStr(buffer);

    while(!flag); if (uart_data != 'h') return;

    if (small_deg <= 90) {
        turn_cclockwise(sensor_data, 90-small_deg-13);
        move_foward(sensor_data, (small_distance*100)-9);
    }
    else if (small_deg > 90) {
        turn_clockwise(sensor_data, small_deg-90-13);
        move_foward(sensor_data, (small_distance*100)-12);
    }
}

void calibrate() {
    int ir, ping = 10, last;

    servo_move(0, 90); move_backward(sensor_data, 6);

    for(i = 0; i < 15; i++){
        timer_waitMillis(1000);

        ir = adc_read(); last = ping;
        ping = ping_read();

        j = 0;
        while((ping < last) || (abs(ping-last) > 6)) {
            //sprintf(buffer, "%d\t%d\r\n", last, ping); uart_sendStr(buffer);
            ping = ping_read();
            timer_waitMillis(250);
            if (!(j % 10)) { move_backward(sensor_data, 0.5); }
            j++;
        }

        //sprintf(buffer, "%d\t %d\t %d\r\n", i+1, ir, ping); uart_sendStr(buffer);

        M.m[0][0]+= 1;            M.m[0][1]+= ir;           M.m[0][2]+= pow(ir,2);    b[0] += ping;
        M.m[1][0]+= ir;           M.m[1][1]+= pow(ir,2);    M.m[1][2]+= pow(ir,3);    b[1] += ping * ir;
        M.m[2][0]+= pow(ir,2);    M.m[2][1]+= pow(ir,3);    M.m[2][2]+= pow(ir,4);    b[2] += ping * pow(ir,2);

        move_backward(sensor_data, 2);
    }

    double detM = det(M);       a0 = det(M_i(0)) / detM;
    a1 = det(M_i(1)) / detM;    a2 = det(M_i(2)) / detM;
    //sprintf(buffer, "y = %.8f*x^2 + %.8f*x + %.8f\r\n", a2, a1, a0); uart_sendStr(buffer);
}

double det(matrix mx) {
    return (   mx.m[0][0] * (mx.m[1][1] * mx.m[2][2] - mx.m[1][2] * mx.m[2][1])
             - mx.m[0][1] * (mx.m[1][0] * mx.m[2][2] - mx.m[1][2] * mx.m[2][0])
             + mx.m[0][2] * (mx.m[1][0] * mx.m[2][1] - mx.m[1][1] * mx.m[2][0]) );
}

matrix M_i(int i) {
    matrix mi = M;
    for (j = 0; j < 3; j++) mi.m[j][i] = b[j];
    return mi;
}

void initialize() {
    timer_init();   adc_init();   lcd_init();
    ping_initi();
    uart_init();
    servo_init();
    optical_init();
    sensor_data   = oi_alloc();   IntMasterEnable();
    //servo_move(0,90);
}
