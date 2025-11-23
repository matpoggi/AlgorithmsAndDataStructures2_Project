#ifndef TARJAN_H
#define TARJAN_H
#define NBMAX 50
#include "list.h"

// Creation of the tarjanVertex type
typedef struct s_tarjanVertex {
    int id;                 // Unique identifier of the vertex (1-based)
    int index;              // Discovery index used in Tarjan's algorithm
    int accessibleNumber;   // Low-link value: smallest index reachable from this vertex
    int onStack;            // Boolean flag: 1 if vertex is currently on the stack, 0 otherwise
} t_tarjanVertex;

// Creation of the class type
typedef struct s_class {
    char name[10];                      // Name of the class
    t_tarjanVertex *vertices[NBMAX];    // Array of pointers to vertices in this class
    int nbVertices;                     // Number of vertices currently in this class
} t_class;

// Creation of the partition type
typedef struct s_partition {
    t_class *classes;   // Pointer to an array of classes (SCCs)
    int nbClasses;      // Number of classes in the partition
} t_partition;

// Creation of the stack type
typedef struct s_stack {
    t_tarjanVertex *values[NBMAX];  // Array of pointers to vertices on the stack
    int nbValues;                   // Current number of vertices in the stack
} t_stack;

// Function declarations
t_tarjanVertex* initTarjanVertices(adjacency_list*);
t_tarjanVertex* pop(t_stack *stack);

t_partition *tarjan(adjacency_list *graph);

void displayPartition(t_partition *partition);
void parcours(int v, adjacency_list *graph, t_tarjanVertex *vertices, t_stack *stack, int *indexPtr, t_partition *partition);
void push(t_stack *stack, t_tarjanVertex* vertex);

#endif //TARJAN_H
