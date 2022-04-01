#include <stdio.h>
#include <stdlib.h>
#include "mem_region_list.h"

/*
 *
 * Modified from source: https://www.cs.uic.edu/~hnagaraj/articles/linked-list/
 *
 */

struct mem_region {
    char *name;
    int fd;
    int user_count;
    struct list_head list_member;
};

void add_node(char *name, int fd, struct list_head *head)
{
    struct mem_region *ptr = (struct mem_region *)malloc(sizeof(struct mem_region));

    ptr->name = name;
    ptr->fd = fd;
    ptr->user_count = 0;

    INIT_LIST_HEAD(&ptr->list_member);
    list_add(&ptr->list_member, head);
}

void display(struct list_head *head)
{
    struct list_head *iter;
    struct mem_region *objPtr;

    printf("---------List starts here--------\n");

    __list_for_each(iter, head) {
        printf("new object\n");
        objPtr = list_entry(iter, struct mem_region, list_member);
        printf("name: %s\n", objPtr->name);
        printf("fd: %d\n", objPtr->fd);
        printf("user_count: %d\n", objPtr->user_count);
    }
    printf("\n");
}

void delete_all(struct list_head *head)
{
    struct list_head *iter;
    struct mem_region *objPtr;

redo:
    __list_for_each(iter, head) {
        objPtr = list_entry(iter, struct mem_region, list_member);
        list_del(&objPtr->list_member);
        free(objPtr);
        goto redo;
    }
}

int find_first_and_delete(char *arg, struct list_head *head)
{
    struct list_head *iter;
    struct mem_region *objPtr;

    __list_for_each(iter, head) {
        objPtr = list_entry(iter, struct mem_region, list_member);
        if(objPtr->name == arg) {
            list_del(&objPtr->list_member);
            free(objPtr);
            return 1;
        }
    }

    return 0;
}

