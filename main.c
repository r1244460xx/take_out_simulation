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
#define END_TIME 72000

typedef struct node{
    int time;
    struct node* next;
}NODE;

typedef struct {
    NODE *head;
    NODE *tail;
}PTR_SET;

int sim_time = 0;
int next_event_type;
int order_status = IDLE;
int kitchen_status = IDLE;
int receive_status = IDLE;

int num_order_q = 0;
int num_kitchen_q = 0;
int num_food_q = 0;
int time_last_event = 0;

int num_cust_served = 0;
int total_delay = 0;
int time_next_event[5];

NODE *order_head = NULL; 
NODE *order_tail = NULL;

NODE *cook_head = NULL;
NODE *cook_tail = NULL;

NODE *food_head = NULL;
NODE *food_tail = NULL;

void enqueue(int data, NODE** tail) {
    NODE* temp = (NODE*)malloc(sizeof(NODE));
    if(temp == NULL) {
        printf("The pointer is NULL\n");
        exit(1);
    }
    if(*tail == NULL) {
        *tail = temp;

        (*tail)->next = NULL;
        (*tail)->time = data;
    }
    else {
        (*tail)->next = temp;
        *tail = (*tail)->next;
        (*tail)->time = data;
        (*tail)->next = NULL;
    }
}

int dequeue(NODE** head) {
    int data;
    if(*head != NULL) {
        NODE* temp = *head;
        data = (*head)->time;
        *head = (*head)->next;
        free(temp);
        return data;
    }
    else {
        printf("Dequeue NULL node!!\n");
        exit(1);
    } 
}

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
    sim_time = min_next_event;
}

void update_stat(void) {

}

void arrive_order(void) {
    time_next_event[ARRIVE] = sim_time + new_arrival();
    if(order_status == BUSY) {
        num_order_q++;
        enqueue(sim_time, &order_tail);
    }
    else {
        order_status = BUSY;
        time_next_event[DEPART_ORDER] = sim_time + new_order_delay();
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
        dequeue(&order_head);
    }
    arrive_kitchen();
    //arrive_receive();
}

void arrive_kitchen(void) {
    if(kitchen_status == BUSY) {
        num_kitchen_q++;
        enqueue(sim_time, &cook_tail);
    }
    else {
        kitchen_status = BUSY;
        time_next_event[DEPART_KITCHEN] = sim_time + new_cook_delay();
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
}

void arrive_food(void) {
    if(receive_status == IDLE) {
        receive_status = BUSY;
        time_next_event[DEPART_STORE] = sim_time + new_receive_delay();
    }
    else {
        enqueue(sim_time, &food_tail);
        num_food_q++;
    }
}

void arrive_receive(void) {
    
}

void depart_store(void) {
    if( num_food_q == 0) {
        time_next_event[DEPART_STORE] = INF;
        receive_status = IDLE;
    }
    else {
        time_next_event[DEPART_STORE] = sim_time + new_receive_delay();
        num_food_q--;
        dequeue(&food_head);
        num_cust_served++;
    }
}
 
void report(void) {
    printf("Number of serverd customers: %d\n", num_cust_served);
    printf("Average delay of one customer: %d\n", total_delay / num_cust_served);
}

void print_time_next_event(void) {
    for(int i = 0; i < 5; i++) {
        printf("time_next_event[%d]: %d\n", i, time_next_event[i]);
    }
}

int main(void) {
    initialize();
    while(1) {
        timing();
        printf("sim_time: %d\n", sim_time);
        update_stat();
        switch(next_event_type) {
            case ARRIVE:
                printf("New arrive event\n");
                arrive_order();                
                break;
            
            case DEPART_ORDER:
                printf("New order joinning kitchen event\n");
                depart_order();               
                break;

            case DEPART_KITCHEN:
                printf("New food processed event\n");
                depart_kitchen();            
                break;
             
            case DEPART_STORE:
                printf("New depart event\n");
                depart_store();
                break;

            case END:
                printf("Ending event\n");
                report();              
                exit(1);
            
            default:
                printf("Error type\n");
                exit(1);
                break;
        }
        print_time_next_event();
        printf("\n");
    }  
}
