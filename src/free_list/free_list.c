/*
 * free_list.c: implementation of a free list (specific to secure shared memory regions)
 */

#include "free_list.h"


/*
 *  init: sets the next and previous pointers for the free and used list
 *          and set the list size to 0
 */
int initialize_list(struct controller *list_control, struct mem_region *arr){
    int i;
    list_control->used = NULL;
    list_control->free = arr;

    //set next and previous pointers
    arr[0].prev = NULL;
    arr[0].next = &arr[1];
    for(i = 1; i < max_regions-1; i++){
        arr[i].next = &arr[i+1];
        arr[i].prev = &arr[i-1];
    }
    arr[max_regions-1].prev = &arr[max_regions-2];
    arr[max_regions-1].next = NULL;

    //set size
    list_control->size = 0;

    return SUCCESS;
}

/*
 *  new_node: adds a node to the used list
 */
struct mem_region *new_node(struct controller *list_control){

    //check that there is space
    if(list_control->size >= max_regions){
        printf("Error: no more space in list\n");
        return NULL;
    }
    
    //adjust free list
    struct mem_region *new_region = list_control->free;
    list_control->free = new_region->next;

    //TODO: memset the region before setting pointers

    if(new_region->next != NULL){
        new_region->next->prev = NULL;
    }

    //adjust used list
    new_region->prev = NULL;
    new_region->next = list_control->used;
    if(list_control->used != NULL){
        list_control->used->prev = new_region;
    }
    list_control->used = new_region; 

    //increment size
    list_control->size++;

    return new_region;   
}

/*
 *  return_to_free: delete the mem_region node (adds it to free list)
 *       *ASSUMES THAT THE POINTER PASSED IN IS THE EXACT POINTER*
 *       *need to call search first and get the region before deleting*
 */
int return_to_free(struct controller *list_control, struct mem_region *node){

    //check node
    if(node == NULL){
        printf("Error: node does not exist in list\n");
        return REMOVE_NONEXISTENT_NODE_ERROR;
    }
   
    //adjust used list
    struct mem_region *curr_prev = node->prev;
    struct mem_region *curr_next = node->next;

    if(curr_prev != NULL){
        curr_prev->next = curr_next;
    }
    if(curr_next != NULL){
        curr_next->prev = curr_prev;
    }

    if(node == list_control->used){
        list_control->used = curr_next;
    }

    //adjust free list
    if(list_control->free == NULL){
        node->prev = NULL;
        node->next = NULL;
        list_control->free = node;
    }
    else {
        list_control->free->prev = node;
        node->prev = NULL;
        node->next = list_control->free;
        list_control->free = node;
    }

    list_control->size--;

    return SUCCESS;
}

/*
 *  search: returns a pointer to the mem_region with the name provided
 *          as a parameter
 */
struct mem_region *search(struct controller *list_control, const char *name){

   //search for struct using memcpy on "name"
   struct mem_region *head;
   char name_buffer[max_name_length];
   memset(name_buffer, 0, sizeof(name_buffer));
   memcpy(name_buffer, name, strlen(name)+1);
   for(head = list_control->used; head != NULL; head=head->next){
        if(memcmp(head->name, name_buffer, sizeof(name_buffer)) == 0){
            return head;
        }
   }
   return NULL;
}

/*
 *  print_list: prints the contents of the used list
 *              *used for testing purposes*
 */
void print_list(struct controller *list_control){
    
    //print used list
    struct mem_region *head;
    int i;
    int count = 0;
    for(head = list_control->used; head != NULL; head=head->next){
        printf("node%d: %s\n", count, head->name);
        printf("fd: %d\n", head->fd);
        printf("size: %zu\n", head->size);
        printf("address: %p\n", head->address);
	    printf("users: ");
	    for(i = 0; i < max_users; i++){
		    printf("%d ", head->users[i]);
	    }
	    printf("\n");
        printf("-------------------------------\n");
        count++;
    }
}
