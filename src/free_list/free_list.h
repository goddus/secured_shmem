/*
 * free_list.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max_size 10
#define max_users 5
#define max_name_length 15

struct controller {
    struct mem_region *used;
    struct mem_region *free;
    int size;
};

struct mem_region {
    char name[max_name_length];
    int fd;
    void *address;
    size_t size;
    int user_count;
    volatile int lock_state;
    int next_in_queue;
    int queue[max_users];
    int next_open_idx;
    int users[max_users];
    struct mem_region *next;
    struct mem_region *prev;
};

int init(struct controller *list_control, struct mem_region *arr);
struct mem_region *new_node(struct controller *list_control);
int return_to_free(struct controller *list_control, struct mem_region *node);
struct mem_region *search(struct controller *list_control, const char *name);
void print_list(struct controller *list_control);



