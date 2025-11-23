#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "utils.h"

t_cell* createCell(int end, float proba) {

    /**
    * @brief Allocate and initialize a singly linked-list cell representing an outgoing edge.
    * Creates a new t_cell with the given destination vertex index and transition probability. Sets next to NULL. Returns the allocated pointer. Terminates the program if allocation fails.
    * @param end Destination vertex index.
    * @param proba Transition probability to the destination vertex.
    * @return t_cell* Pointer to the newly allocated cell.
    * @post Returned cell has next == NULL.
    * @warning Calls exit(EXIT_FAILURE) if memory allocation fails.
    */

    t_cell *cell = malloc(sizeof(t_cell));
    if (!cell) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    cell->end = end;
    cell->proba = proba;
    cell->next = NULL;
    return cell;
}

t_list createEmptyList() {

    /**
    * @brief Create and return an empty linked list.
    * Initializes a t_list with head set to NULL and returns it by value.
    * @return t_list An empty list (head == NULL).
    * @post Returned list’s head is NULL.
    */

    t_list list;
    list.head = NULL;
    return list;
}

void addCell(t_list *list, int end, float proba) {

    /**
    * @brief Append a new cell (end, proba) to the end of a list.
    * Allocates a cell using createCell and inserts it at the tail. If the list is empty, it becomes the head.
    * @param list Pointer to the target list (must not be NULL).
    * @param end Destination vertex index.
    * @param proba Transition probability to end.
    * @return void
    * @pre list != NULL.
    * @post The list gains an additional element at its tail.
    * @throws Terminates if allocation fails (via createCell).
    */

    t_cell *newcell = createCell(end, proba);

    if (list->head == NULL) {
        list->head = newcell;
    } else {
        t_cell *temp = list->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newcell;
    }
}

void displayList(t_list *list) {

    /**
    * @brief Print the list as arrows with (end, proba) pairs.
    * Iterates through the list, printing each cell to stdout, followed by a newline.
    * @param list Pointer to the list to display (may be empty).
    * @return void
    */

    t_cell *temp = list->head;
    while (temp != NULL) {
        printf("-> (%d, %.2f) ", temp->end, temp->proba);
        temp = temp->next;
    }
    printf("\n");
}

adjacency_list createEmptyAdjacencyList(int size) {

    /**
    * @brief Create an empty adjacency list of given size.
    * Allocates an array of t_list, initializes each entry to empty. Returns the adjacency_list. Terminates the program if allocation fails.
    * @param size Number of vertices (array length).
    * @return adjacency_list The initialized adjacency list.
    * @pre size >= 0.
    * @post Each array[i] is an empty list.
    * @warning Calls exit(EXIT_FAILURE) if allocation fails.
    */

    adjacency_list adjlist;
    adjlist.size = size;
    adjlist.array = malloc(size * sizeof(t_list));
    if (adjlist.array == NULL) {
        perror("Failed to allocate adjacency list");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        adjlist.array[i] = createEmptyList();
    }

    return adjlist;
}

void displayAdjacencyList(adjacency_list adjlist) {

    /**
    * @brief Display the entire adjacency list vertex by vertex.
    * For each vertex, prints its index and the list of outgoing edges.
    * @param adjlist The adjacency list to display.
    * @return void
    */

    printf("\n[AdjaListDisp] Adjacency List:\n");
    for (int i = 0; i < adjlist.size; i++) {
        printf("[AdjaListDisp] Vertex %d ", i + 1);
        displayList(&adjlist.array[i]);
    }
}

adjacency_list readGraph(const char *filename) {

    /**
    * @brief Read a graph from a text file and construct its adjacency list.
    * Expects vertex count in first token. Each subsequent line: "start end proba" (all 1-based indices). Sorts each vertex’s list by ascending probability after reading.
    * @param filename Path to the input file (text mode).
    * @return adjacency_list The loaded and sorted adjacency list.
    * @throws Terminates if file fails to open or if vertex count cannot be read.
    * @warning Stops processing on out-of-bounds start index.
    */

    printf("\n---------------------------------------\n[readGraph---] Opening file: %s\n", filename);

    FILE *file = fopen(filename, "rt");
    if (file == NULL) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        perror("Could not read number of vertices");
        exit(EXIT_FAILURE);
    }
    printf("[readGraph---] Number of vertices: %d\n", nbvert);

    adjacency_list adjlist = createEmptyAdjacencyList(nbvert);
    printf("[readGraph---] Adjacency list created (size=%d)\n", adjlist.size);

    int start, end;
    float proba;
    int line = 0;
    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3) {
        line++;

        if (start < 1 || start > adjlist.size) {
            printf("[ERROR] Start index %d out of bounds!\n", start);
            break;
        }

        addCell(&adjlist.array[start - 1], end, proba);

    }
    for (int i = 0; i < adjlist.size; i++) {
        adjlist.array[i] = sortList(adjlist.array[i]);
    }
    fclose(file);
    printf("[readGraph---] Done reading file.\n---------------------------------------");
    displayAdjacencyList(adjlist);
    printf("---------------------------------------\n[readGraph---] Graph loaded successfully.\n---------------------------------------\n\n");
    return adjlist;
}

void isMarkov (adjacency_list adjlist) {

    /**
    * @brief Check whether the graph is Markovian (sum of outgoing probabilities ≈ 1 for each vertex).
    * Sums outgoing probabilities for each vertex and checks if sum is in [0.99, 1.00]. Confirms if the graph passes or not.
    * @param adjlist The adjacency list.
    * @return void
    * @note Accepts sums ≤ 1.00 and ≥ 0.99; otherwise, rejects.
    */

    int truemarkov = 1;
    for (int i = 0; i < adjlist.size; i++) {
        printf("[markov------] Vertex %d :\n", i + 1);
        printf("[markov------] -- Checking the sum of the probabilities%d\n", i + 1);
        float proba = 0.0;
        t_cell *curr = adjlist.array[i].head;
        while (curr != NULL) {
            proba += curr->proba;
            curr = curr->next;
        }
        if (proba <0.99 || proba > 1.00) {
            truemarkov = 0;
            printf("[markov------] -- Sum of probability = %.2f \n", proba);
            printf ("---------------------------------------\n");
            break;
        }
        printf("[markov------] -- Sum of probability = 1\n");
    }
    if (truemarkov == 1) {
        printf ("---------------------------------------\n[markov------] The graph is a Markov graph\n---------------------------------------\n\n");
    }else{
        printf ("[markov------] The graph is not a Markov graph\n---------------------------------------\n\n");
    }
}

void drawGraph(adjacency_list adjlist) {

    /**
    * @brief Generate a Mermaid flowchart file describing the graph for visualization.
     * Writes to "../output/graph.txt" a diagram with layout and theme information, nodes, and edges labeled by probability. Uses getID(int) to label vertices.
    * @param adjlist The graph to render.
    * @return void
    * @warning Output path is relative: "../output/graph.txt" must exist.
    */


    FILE *file = fopen("../output/graph.txt", "w");
    if (file == NULL) {
        perror("[drawGraph---] Could not open file for writing");
    }
    printf("[drawGraph---] Writing the config...\n");
    fprintf(file, "--- \nconfig: \n   layout: elk \n   theme: neo \n   look: neo \n---\n\nflowchart LR");

    for (int i = 0; i < adjlist.size; i++) {
        int vertex_index = i + 1;
        char * vertex_letter;
        vertex_letter = getID(vertex_index);
        fprintf(file,"\n%s((%d))",vertex_letter, vertex_index);
    }

    printf("[drawGraph---] Edge creation : Done\n");

    fprintf(file,"\n\n");

    for (int vertex_index = 1; vertex_index <= adjlist.size; vertex_index++) {

        t_list *edge_list = &adjlist.array[vertex_index - 1];
        t_cell *edge_curr = edge_list->head;
        while (edge_curr != NULL) {

            char * vertex_letter = getID(vertex_index);
            fprintf(file,"%s -->|",vertex_letter);

            char * edge_letter = getID(edge_curr->end);
            fprintf(file,"%.2f|%s\n", edge_curr->proba, edge_letter);

            edge_curr = edge_curr->next;

        }
        printf("[drawGraph---] Vertex %d : Done\n",vertex_index);
    }
    printf("---------------------------------------\n[drawGraph---] Graph draw file create successfully.\n---------------------------------------\n\n");
}

void removeCell(t_cell *cell, t_list *list) {

    /**
    * @brief removes a given cell from a given list
    * @param cell : pointer to the cell to be removed
    * @param list : pointer to the list from wich the cell is to be removed
    * @return void
    */

    if (!list || !list->head || !cell) return;

    if (cell == list->head) {
        list->head = list->head->next;
        free(cell);
        return;
    }

    t_cell *prev = list->head;
    t_cell *cur  = list->head->next;

    while (cur && cur != cell) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == cell) {
        prev->next = cur->next;
        free(cur);
    }
}

t_list sortList(t_list list){

    /**
    * @brief sorts the given list in a increasing order
    * @param list : the given list to be sorted
    * @return t_list the sorted list : sorted_list
    */

    t_list sorted_list = createEmptyList();

    while (list.head != NULL) {
        t_cell *lowest_cell = list.head;
        t_cell *current_cell = list.head;

        while (current_cell != NULL) {
            if (current_cell->proba < lowest_cell->proba) {
                lowest_cell = current_cell;
            }
            current_cell = current_cell->next;
        }

        addCell(&sorted_list, lowest_cell->end, lowest_cell->proba);

        removeCell(lowest_cell, &list);
    }

    return sorted_list;
}

