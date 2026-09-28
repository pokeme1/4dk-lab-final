#include "simlib.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void simulation_run_ensure_capacity(Simulation_Run_Ptr run, size_t required)
{
  if (run == NULL) {
    return;
  }

  if (required <= run->event_capacity) {
    return;
  }

  size_t new_capacity = run->event_capacity == 0 ? 8 : run->event_capacity;
  while (new_capacity < required) {
    new_capacity *= 2;
  }

  Event *new_events = (Event *) realloc(run->events, new_capacity * sizeof(Event));
  double *new_times = (double *) realloc(run->event_times, new_capacity * sizeof(double));

  if (new_events == NULL || new_times == NULL) {
    free(new_events);
    free(new_times);
    return;
  }

  run->events = new_events;
  run->event_times = new_times;
  run->event_capacity = new_capacity;
}

Simulation_Run_Ptr simulation_run_new(void)
{
  Simulation_Run_Ptr run = (Simulation_Run_Ptr) calloc(1, sizeof(*run));
  if (run == NULL) {
    return NULL;
  }

  run->current_time = 0.0;
  run->event_count = 0;
  run->event_capacity = 0;
  run->data = NULL;
  return run;
}

void simulation_run_attach_data(Simulation_Run_Ptr simulation_run, void *data)
{
  if (simulation_run != NULL) {
    simulation_run->data = data;
  }
}

void *simulation_run_data(Simulation_Run_Ptr simulation_run)
{
  if (simulation_run == NULL) {
    return NULL;
  }
  return simulation_run->data;
}

void simulation_run_free_memory(Simulation_Run_Ptr simulation_run)
{
  if (simulation_run == NULL) {
    return;
  }

  free(simulation_run->events);
  free(simulation_run->event_times);
  free(simulation_run);
}

long simulation_run_schedule_event(Simulation_Run_Ptr simulation_run, Event event,
                                  double event_time)
{
  if (simulation_run == NULL) {
    return -1;
  }

  simulation_run_ensure_capacity(simulation_run, simulation_run->event_count + 1);
  size_t insert_at = simulation_run->event_count;

  while (insert_at > 0 && event_time < simulation_run->event_times[insert_at - 1]) {
    simulation_run->events[insert_at] = simulation_run->events[insert_at - 1];
    simulation_run->event_times[insert_at] = simulation_run->event_times[insert_at - 1];
    insert_at--;
  }

  simulation_run->events[insert_at] = event;
  simulation_run->event_times[insert_at] = event_time;
  simulation_run->event_count++;

  return (long) insert_at;
}

void simulation_run_execute_event(Simulation_Run_Ptr simulation_run)
{
  if (simulation_run == NULL || simulation_run->event_count == 0) {
    return;
  }

  Event event = simulation_run->events[0];
  simulation_run->current_time = simulation_run->event_times[0];

  for (size_t i = 1; i < simulation_run->event_count; ++i) {
    simulation_run->events[i - 1] = simulation_run->events[i];
    simulation_run->event_times[i - 1] = simulation_run->event_times[i];
  }

  simulation_run->event_count--;

  if (event.function != NULL) {
    event.function(simulation_run, event.attachment);
  }
}

double simulation_run_get_time(Simulation_Run_Ptr simulation_run)
{
  if (simulation_run == NULL) {
    return 0.0;
  }
  return simulation_run->current_time;
}

Fifoqueue_Ptr fifoqueue_new(void)
{
  Fifoqueue_Ptr queue = (Fifoqueue_Ptr) calloc(1, sizeof(*queue));
  if (queue == NULL) {
    return NULL;
  }
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
  return queue;
}

void fifoqueue_put(Fifoqueue_Ptr queue, void *item)
{
  if (queue == NULL) {
    return;
  }

  Fifoqueue_Node *node = (Fifoqueue_Node *) malloc(sizeof(*node));
  if (node == NULL) {
    return;
  }

  node->item = item;
  node->next = NULL;

  if (queue->tail == NULL) {
    queue->head = node;
    queue->tail = node;
  } else {
    queue->tail->next = node;
    queue->tail = node;
  }

  queue->size++;
}

void *fifoqueue_get(Fifoqueue_Ptr queue)
{
  if (queue == NULL || queue->head == NULL) {
    return NULL;
  }

  Fifoqueue_Node *node = queue->head;
  void *item = node->item;
  queue->head = node->next;
  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  queue->size--;
  free(node);
  return item;
}

size_t fifoqueue_size(Fifoqueue_Ptr queue)
{
  if (queue == NULL) {
    return 0;
  }
  return queue->size;
}

Server_Ptr server_new(void)
{
  Server_Ptr server = (Server_Ptr) calloc(1, sizeof(*server));
  if (server == NULL) {
    return NULL;
  }
  server->item = NULL;
  server->state = FREE;
  return server;
}

void server_put(Server_Ptr server, void *item)
{
  if (server == NULL) {
    return;
  }
  server->item = item;
  server->state = BUSY;
}

void *server_get(Server_Ptr server)
{
  if (server == NULL) {
    return NULL;
  }

  void *item = server->item;
  server->item = NULL;
  server->state = FREE;
  return item;
}

Server_State server_state(Server_Ptr server)
{
  if (server == NULL) {
    return FREE;
  }
  return server->state;
}

void random_generator_initialize(unsigned seed)
{
  srand((unsigned) seed);
}

double exponential_generator(double lambda)
{
  if (lambda <= 0.0) {
    return 0.0;
  }

  double u = (double) rand() / ((double) RAND_MAX + 1.0);
  if (u <= 0.0) {
    u = 1e-12;
  }
  return -lambda * log(u);
}

void *xmalloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "xmalloc: out of memory\n");
    exit(1);
  }
  return ptr;
}

void xfree(void *ptr)
{
  free(ptr);
}
