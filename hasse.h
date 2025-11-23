#ifndef __HASSE_H__
#define __HASSE_H__

#include "tarjan.h"
#include "list.h"

// Creation of the link type
typedef struct s_link {
    t_class *from;  // Pointer to the source class (SCC) of the link
    t_class *to;    // Pointer to the target class (SCC) of the link
} t_link;

// Creation of the link_array type
typedef struct s_link_array {
    t_link *links;  // Dynamic array of links
    int log_size;   // Number of links currently stored in the array
} t_link_array;

// Function declarations
t_link_array *initLinkArray(adjacency_list *graph, t_partition *partition);

void textFileHasse(t_partition *partition, t_link_array *linkArray);
void removeTransitiveLinks(t_link_array *p_link_array);
void displayCharacteristics(t_partition *partition, t_link_array *linkArray);

#endif