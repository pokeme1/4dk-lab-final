#ifndef SIMLIB_H
#define SIMLIB_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _event_ Event;
typedef struct _simulation_run_ * Simulation_Run_Ptr;
typedef struct _fifoqueue_ * Fifoqueue_Ptr;
typedef struct _server_ * Server_Ptr;

typedef enum { FREE = 0, BUSY = 1 } Server_State;

struct _event_ {
  char description[64];
  void (*function)(Simulation_Run_Ptr, void *);
  void *attachment;
};

struct _simulation_run_ {
  double current_time;
  Event *events;
  double *event_times;
  size_t event_count;
  size_t event_capacity;
  void *data;
};

struct _fifoqueue_ {
  struct _fifoqueue_node_ *head;
  struct _fifoqueue_node_ *tail;
  size_t size;
};

typedef struct _fifoqueue_node_ {
  void *item;
  struct _fifoqueue_node_ *next;
} Fifoqueue_Node;

struct _server_ {
  void *item;
  Server_State state;
};

Simulation_Run_Ptr simulation_run_new(void);
void simulation_run_attach_data(Simulation_Run_Ptr, void *);
void *simulation_run_data(Simulation_Run_Ptr);
void simulation_run_free_memory(Simulation_Run_Ptr);
long simulation_run_schedule_event(Simulation_Run_Ptr, Event, double);
void simulation_run_execute_event(Simulation_Run_Ptr);
double simulation_run_get_time(Simulation_Run_Ptr);

Fifoqueue_Ptr fifoqueue_new(void);
void fifoqueue_put(Fifoqueue_Ptr, void *);
void *fifoqueue_get(Fifoqueue_Ptr);
size_t fifoqueue_size(Fifoqueue_Ptr);

Server_Ptr server_new(void);
void server_put(Server_Ptr, void *);
void *server_get(Server_Ptr);
Server_State server_state(Server_Ptr);

void random_generator_initialize(unsigned seed);
double exponential_generator(double lambda);

void *xmalloc(size_t size);
void xfree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* SIMLIB_H */
