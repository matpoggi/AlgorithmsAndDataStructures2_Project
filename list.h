#ifndef LIST_H
#define LIST_H

// Creation of the cell type
typedef struct s_cell {
    int end;                // destination vertex
    float proba;            // probability
    struct s_cell *next;    // destination cell
} t_cell;

// Creation of the list type
typedef struct {
    t_cell *head;   // first cell of the list
} t_list;

// Creation of the adjacency_list type
typedef struct {
    int size;       // number of vertices
    t_list *array;  // array of adjacency lists
} adjacency_list;

// Function declarations
adjacency_list createEmptyAdjacencyList(int size);
adjacency_list readGraph(const char *filename);

t_cell* createCell(int end, float proba);

t_list sortList(t_list list);
t_list createEmptyList();

void displayAdjacencyList(adjacency_list adjlist);
void addCell(t_list *list, int end, float proba);
void isMarkov (adjacency_list adjlist);
void drawGraph(adjacency_list adjlist);
void displayList(t_list *list);

// Optional Function Declaration
void removeCell(t_cell *cell, t_list *list);


#endif
