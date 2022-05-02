/*
 *  free_list_test.c: extensively tests the free_list functions
 */

#include "free_list.h"

int main(){

    int i;

    //create an array of int_holder structs and a controller
    struct mem_region *arr = malloc(max_size*sizeof(struct mem_region));
    struct controller control;

    //initialize the list
    if(init(&control, arr) != 0){
        printf("Error during init_holder\n");
    }
    printf("successfully initialized list\n");

    //add a few nodes
    struct mem_region *node1 = new_node(&control);
    memset(node1->name, 0, sizeof(node1->name));
    strcpy(node1->name, "one");

    struct mem_region *node2= new_node(&control);
    memset(node2->name, 0, sizeof(node2->name));
    strcpy(node2->name, "two");

    struct mem_region *node3 = new_node(&control);
    memset(node3->name, 0, sizeof(node3->name));
    strcpy(node3->name, "three");

    //print the list
    print_list(&control);

    //try and add too many nodes
    struct mem_region *node4 = new_node(&control);
    memset(node4->name, 0, sizeof(node4->name));
    strcpy(node4->name, "four");

    struct mem_region *node5= new_node(&control);
    memset(node5->name, 0, sizeof(node5->name));
    strcpy(node5->name, "five");

    struct mem_region *node6 = new_node(&control);
    memset(node6->name, 0, sizeof(node6->name));
    strcpy(node6->name, "six");

    struct mem_region *node7 = new_node(&control);
    memset(node7->name, 0, sizeof(node7->name));
    strcpy(node7->name, "seven");

    struct mem_region *node8 = new_node(&control);
    memset(node8->name, 0, sizeof(node8->name));
    strcpy(node8->name, "eight");

    struct mem_region *node9 = new_node(&control);
    memset(node9->name, 0, sizeof(node9->name));
    strcpy(node9->name, "nine");

    struct mem_region *node10 = new_node(&control);
    memset(node10->name, 0, sizeof(node10->name));
    strcpy(node10->name, "ten");

    //should be fine up to here
    printf("Added 10 nodes (list is filled)\n");
    print_list(&control);

    printf("Adding another node should throw an error\n");
    struct mem_region *node11 = new_node(&control); //this should throw an error

    //search for node and remove
    struct mem_region *to_remove;
    to_remove = search(&control, "three");

    //verify search
    printf("to remove: %s\n", to_remove->name);

    //remove 3
    return_to_free(&control, to_remove);
    
    printf("Removed 'three' from list\n"); 
    print_list(&control);

    //remove first node
    to_remove = search(&control, "ten");
    return_to_free(&control, to_remove);

    printf("Removed 'ten' from list\n");
    print_list(&control);

    //remove last node
    to_remove = search(&control, "one");
    return_to_free(&control, to_remove);

    printf("Removed 'one' from list\n");
    print_list(&control);

    //add another node
    node11 = new_node(&control);
    memset(node11->name, 0, sizeof(node11->name));
    strcpy(node11->name, "eleven");

    printf("Added 'eleven' to list\n");
    print_list(&control);

    //remove all the nodes
    char *regions_left[8] = {"eleven", "nine", "eight", "seven", "six", "five", "four", "two"};
    for(i = 0; i < 8; i++){
        to_remove = search(&control, regions_left[i]);
        return_to_free(&control, to_remove);
    }

    printf("Removed all nodes\n");
    print_list(&control);

    //try and remove an extra node
    printf("Trying to remove node that doesn't exist should throw error\n");
    to_remove = search(&control, "twelve"); //this should return null
    return_to_free(&control, to_remove); //this should throw an error

    //add another node
    struct mem_region *node12 = new_node(&control);
    memset(node12->name, 0, sizeof(node12->name));
    strcpy(node12->name, "twelve");

    printf("Added 'twelve' to the list\n");
    print_list(&control);

    //search for and update 12
    struct mem_region *to_update = search(&control, "twelve");
    memset(to_update->name, 0, sizeof(to_update->name));
    strcpy(to_update->name, "thirteen");

    printf("Changed 'twelve' to 'thirteen'\n");
    print_list(&control);

    return 0;

}
