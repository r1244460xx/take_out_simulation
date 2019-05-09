#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define ARRIVE 0
#define DEPART 1
#define END 2

#define ORDER 4
#define COOK 5

#define IDLE 0
#define BUSY 1

#define INF 100000000

int sim_time = 0;
int next_event_type;
int order_status = IDLE;
int kitchen_status = IDLE;
int receive_status = IDLE;

int num_order_q = 0;
int num_receive_q = 0;
int time_last_event = 0;
int end_time = 3600;

int num_cust_served = 0;
int total_delay = 0;
int time_next_event[3];

int time_order_arrive[200];
int time_receive_arrival[200];

int new_arrival(void) {
    return rand() % 115;
}

int new_value(void) {
    return rand() % 1000;
}

int new_order_delay(void) {
    return rand() % 120;
}

int new_cook_delay(void) {
    return rand() % 96;
}

int new_receive_delay(void) {
    return rand() % 120;
}

void initialize(void) {
    srand(time(NULL));
    time_next_event[0] = sim_time + new_arrival();
    time_next_event[1] = INF;
    time_next_event[2] = end_time;
}

void timing(void) {
    int min_next_event = INF - 1;
    next_event_type = NULL;
    for(int i = 0; i < 3; i++) {
        if(time_next_event[i] < min_next_event) {
            min_next_event = time_next_event[i];
            next_event_type = i;
        }
    }
    sim_time = min_next_event;
}

void update_stat(void) {

}

void arrive(void) {
    int delay;
    time_next_event[0] = sim_time + new_arrival();
    if(order_status == BUSY) {
        //check the length of q
        num_order_q++;
    }
    else {
        //delay = 0;
        //total_delay += delay;
        num_cust_served++;
        //time_next_event[2] = sim_time + new_cook_delay;
    }
}

void depart(void) {

}

void report(void) {
    printf("Number of serverd customers: %d\n", num_cust_served);
    printf("Average delay of one customer: %d\n", total_delay / num_cust_served);
}



int main(void) {
    initialize();
    while(sim_time > 0) {
        timing();
        update_stat();
        switch(next_event_type) {
            case ARRIVE:
                arrive();
                break;
            case DEPART:
                depart();
                break;
            case END:
                report();
                break;
            case COOK:
                
                break;
            case ORDER:
                break;
        }
    }
    
}
