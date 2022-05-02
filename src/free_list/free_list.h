/*
 * free_list.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max_regions 10
#define max_users 5
#define max_name_length 15

#define regions_list_name "/regions_list"
#define regions_list_lock_name "/regions_lock"

enum return_values {
    SUCCESS,
    REMOVE_NONEXISTENT_NODE_ERROR,
    MEM_REGION_CREATION_ERROR,
    UNLINKING_ERROR
};

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
    int next_open_idx;
    pid_t users[max_users];
    struct mem_region *next;
    struct mem_region *prev;
};

struct mem_regions_list {
    struct controller control;
    struct mem_region regions[max_regions];
};

int initialize_list(struct controller *list_control, struct mem_region *arr);
struct mem_region *new_node(struct controller *list_control);
int return_to_free(struct controller *list_control, struct mem_region *node);
struct mem_region *search(struct controller *list_control, const char *name);
void print_list(struct controller *list_control);



