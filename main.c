#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>

#define IDLE 0
#define BUSY 1

#define INF 100000000

#define ARRIVE 0
#define DEPART_ORDER 1
#define DEPART_KITCHEN 2
#define DEPART_STORE 3
#define END 4
#define END_TIME 3600

typedef struct node{
    int time;
    struct node* next;
}NODE;

typedef struct {
    NODE *head;
    NODE *tail;
}PTR_SET;

int sim_time = 0;
int next_event_type = 5;
int order_status = IDLE;
int kitchen_status = IDLE;
int receive_status = IDLE;

int num_order_q = 0;
int num_kitchen_q = 0;
int num_food_q = 0;
int num_receive_q = 0;
int num_cust_served = 0;
int total_delay = 0;
int time_next_event[5];
int time_last_event = 0;
int rest_cust = 0;
int value = 0;
int order_delay = 0;
int cook_delay = 0;
int food_delay = 0;
int receive_delay = 0;

PTR_SET order_set;
PTR_SET cook_set;
PTR_SET food_set;
PTR_SET receive_set;

void enqueue(int data, PTR_SET *set) {
    NODE* temp = (NODE*)malloc(sizeof(NODE));
    if(temp == NULL) {
        printf("The pointer is NULL\n");
        exit(1);
    }
    if((*set).tail == NULL) {
        (*set).tail = temp;
        (*set).head = temp;
        (*set).tail->next = NULL;
        (*set).tail->time = data;
    }
    else {
        (*set).tail->next = temp;
        (*set).tail = (*set).tail->next;
        (*set).tail->time = data;
        (*set).tail->next = NULL;
    }
}

int dequeue(PTR_SET *set) {   
    if((*set).head != NULL) {
        int data;
        NODE* temp = (*set).head;
        data = (*set).head->time;
        (*set).head = (*set).head->next;
        free(temp);
        if((*set).head == NULL) {
            (*set).tail = NULL;
        }
        return data;
    }  
    else {
        printf("The pointer is NULL\n");
        exit(1);
    }
}

int new_arrival(void) {
    return rand() % 115 + 1;
}

int new_value(void) {
    return rand() % 1000 + 1;
}

int new_order_delay(void) {
    return rand() % 120 + 1;
}

int new_cook_delay(void) {
    return rand() % 96 + 1;
}

int new_receive_delay(void) {
    return rand() % 120 + 1;
}

void initialize(void) {
    srand(time(NULL));
    time_next_event[ARRIVE] = sim_time + new_arrival();
    time_next_event[DEPART_ORDER] = INF;
    time_next_event[DEPART_KITCHEN] = INF;
    time_next_event[DEPART_STORE] = INF;
    time_next_event[END] = END_TIME;
    printf("Initialize over\n\n");
}

void timing(void) {
    int min_next_event = INF - 1;
    for(int i = 0; i < 5; i++) {
        if(time_next_event[i] < min_next_event) {
            min_next_event = time_next_event[i];
            next_event_type = i;
        }
    }
    time_last_event = sim_time;
    sim_time = min_next_event;
}

void update_stat(void) {
    order_delay += num_order_q * (sim_time - time_last_event);
    cook_delay += num_kitchen_q * (sim_time - time_last_event);
    food_delay += num_food_q * (sim_time - time_last_event);
    receive_delay += num_receive_q * (sim_time - time_last_event);
}

void arrive_order(void) {
    time_next_event[ARRIVE] = sim_time + new_arrival();
    rest_cust++;
    if(order_status == BUSY) {
        num_order_q++;
        enqueue(sim_time, &order_set);
    }
    else {
        order_status = BUSY;
        time_next_event[DEPART_ORDER] = sim_time + new_order_delay();
    }
}

void arrive_kitchen(void) {
    if(kitchen_status == BUSY) {
        num_kitchen_q++;
        enqueue(sim_time, &cook_set);
    }
    else {
        kitchen_status = BUSY;
        time_next_event[DEPART_KITCHEN] = sim_time + new_cook_delay();
    }
}

void depart_order(void) {
    if(num_order_q == 0) {
        order_status = IDLE;
        time_next_event[DEPART_ORDER] = INF;
    }
    else {
        num_order_q--;
        time_next_event[DEPART_ORDER] = sim_time + new_order_delay();
        dequeue(&order_set);
    }
    value += new_value();
    arrive_kitchen();
    
    enqueue(sim_time, &receive_set);
    num_receive_q++;
}

void arrive_food(void) {
    if(receive_status == IDLE) {
        receive_status = BUSY;
        time_next_event[DEPART_STORE] = sim_time + new_receive_delay();
    }
    else {
        enqueue(sim_time, &food_set);
        num_food_q++;
    }
}

void depart_kitchen(void) {
    if(num_kitchen_q == 0) {
        kitchen_status = IDLE;
        time_next_event[DEPART_KITCHEN] = INF;
    }
    else {
        num_kitchen_q--;
        time_next_event[DEPART_KITCHEN] = sim_time + new_cook_delay();
    }
    arrive_food();
    dequeue(&receive_set);
    num_receive_q--;
}

void depart_store(void) {
    if(num_food_q == 0) {
        time_next_event[DEPART_STORE] = INF;
        receive_status = IDLE;
    }
    else {
        time_next_event[DEPART_STORE] = sim_time + new_receive_delay();
        num_food_q--;
        dequeue(&food_set);
        num_cust_served++;
    }
    rest_cust--;
}
 
void report(void) {
    printf("Total simulation time(s): %d\n", END_TIME);
    printf("Number of serverd customers: %d\n", num_cust_served);
    printf("Average delay of one customer: %d\n", total_delay / num_cust_served);
    printf("Rest customers in store: %d\n", rest_cust); 
    printf("Number of people in order q: %d\n", num_order_q);
    printf("Number of people in kitchen q: %d\n", num_kitchen_q);
    printf("Number of food in q: %d\n", num_food_q);
    printf("Number of people in receive q: %d\n", num_receive_q);
    printf("The income of this time: %d\n", value);
    printf("Total delay in ordering counter: %d\n", order_delay);
    printf("Total delay in kitchen: %d\n", cook_delay);
    printf("Total delay in receive counter: %d", food_delay);
}

void print_time_next_event(void) {
    printf("sim_time: %d\n", sim_time);
    for(int i = 0; i < 5; i++) {
        printf("time_next_event[%d]: %d\n", i, time_next_event[i]);
    }
}

int main(void) {
    initialize();
    while(1) {
        timing();
        //print_time_next_event();
        update_stat();
        switch(next_event_type) {
            case ARRIVE:
                //printf("New arrive event\n");
                arrive_order();                
                break;
            
            case DEPART_ORDER:
                //printf("New order joinning kitchen event\n");
                depart_order();               
                break;

            case DEPART_KITCHEN:
                //printf("New food processed event\n");
                depart_kitchen();            
                break;
             
            case DEPART_STORE:
                //printf("New depart event\n");
                depart_store();
                break;

            case END:
                //printf("Ending event\n\n");
                report();              
                exit(1);
                break;
            
            default:
                //printf("Error type\n");
                exit(1);
                break;
        }
        //printf("\n");
    } 
}
