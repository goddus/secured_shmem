#include "list.h"

//note: commented this out for now just to look at the locking first
/*
struct mem_region {
    char *name;
    int fd;
    int user_count;
    struct list_head list_member;
};
*/

void add_node(char *name, int fd, struct list_head *head);
void display(struct list_head *head);
void delete_all(struct list_head *head);
int find_first_and_delet(char *arg, struct list_head *head);
