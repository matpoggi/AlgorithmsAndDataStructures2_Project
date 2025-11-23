#include <stdio.h>
#include "hasse.h"
#include <stdlib.h>

#include "utils.h"

t_class **arrayClass(adjacency_list *graph, t_partition *partition) {

    /**
    * @brief Creates an array mapping each vertex to its corresponding class.
    * @param graph Pointer to the adjacency list representing the graph.
    * @param partition Pointer to the partition containing the strongly connected components.
    * @return An array of pointers to t_class, where each element corresponds to a vertex
    *         and points to the class it belongs to.
    */

	t_class **verticesClass = malloc(graph->size * sizeof(t_class*));

    for (int i = 0; i < graph->size; i++) {
        verticesClass[i] = NULL;
    }

    for (int i = 0; i < partition->nbClasses; i++) {
    	t_class *class = &partition->classes[i];
    	for (int j = 0; j < class->nbVertices; j++) {
        	int id = class->vertices[j]->id - 1;
            verticesClass[id] = class;
    	}
    }

    return verticesClass;
}

t_link_array *initLinkArray(adjacency_list *graph, t_partition *partition) {

    /**
    * @brief Initializes an array of links between classes based on the graph.
    * @param graph Pointer to the adjacency list representing the graph.
    * @param partition Pointer to the partition containing the strongly connected components.
    * @return Pointer to a t_link_array structure containing all unique links
    *         between different classes.
    * @note Only links between different classes are stored; duplicates are avoided.
    */

	t_class **array = arrayClass(graph,partition);

    t_link_array *linkArray = malloc(sizeof(t_link_array));
    linkArray->links = malloc(NBMAX * sizeof(t_link));
    linkArray->log_size = 0;

    for (int i = 0; i < graph->size; i++) {
		t_class *Ci = array[i];
        t_cell *curr = graph->array[i].head;
        while (curr != NULL) {
        	int j = curr->end - 1;

            t_class *Cj = array[j];

            if (Ci != Cj) {
                int exists = 0;
                for (int k = 0; k < linkArray->log_size; k++) {
                    if (linkArray->links[k].from == Ci && linkArray->links[k].to == Cj) {
                        exists = 1;
                        break;
                    }
                }

                if (!exists) {
                    linkArray->links[linkArray->log_size].from = Ci;
                    linkArray->links[linkArray->log_size].to = Cj;
                    linkArray->log_size++;
                }
            }

            curr = curr->next;
        }
    }

    free(array);
    return linkArray;
}

void textFileHasse(t_partition *partition, t_link_array *linkArray) {

    /**
    * @brief Generates a text file representing the Hasse diagram of a partition.
    * @param partition Pointer to the partition containing the strongly connected components.
    * @param linkArray Pointer to the array of links between classes.
    * @return void
    * @note The function frees the memory of linkArray after writing.
    */

    FILE *file = fopen("../output/textFileHasse.txt", "w");
    fprintf(file, "--- \nconfig: \n   layout: elk \n   theme: neo \n   look: neo \n---\n\nflowchart LR\n");

    for (int i = 0; i < partition->nbClasses; i++) {
        fprintf(file,"%s[\"{",getID(i+1));

        for (int j = 0; j < partition->classes[i].nbVertices - 1; j++) {
        	fprintf(file,"%d,",partition->classes[i].vertices[j]->id);
        }
        fprintf(file,"%d",partition->classes[i].vertices[partition->classes[i].nbVertices-1]->id);

        fprintf(file,"}\"]\n");
    }

    fprintf(file,"\n");

    for (int i = 0; i < linkArray->log_size; i++) {
    	for (int j = 0; j < partition->nbClasses; j++) {
        	if (&partition->classes[j] == linkArray->links[i].from) {
            	fprintf(file,"%s",getID(j+1));
                break;
        	}
    	}
        fprintf(file," --> ");
        for (int j = 0; j < partition->nbClasses; j++) {
        	if (&partition->classes[j] == linkArray->links[i].to) {
            	fprintf(file,"%s\n",getID(j+1));
                break;
        	}
        }
    }
    free(linkArray->links);
    free(linkArray);
	fprintf(file,"\n");
    fclose(file);
}

void removeTransitiveLinks(t_link_array *p_link_array){

    /**
    * @brief Removes transitive links from a link array to simplify the Hasse diagram.
    * @param p_link_array Pointer to the link array to process.
    * @return void
    * @note A link from A to C is removed if there exists a vertex B such that
    *       links A → B and B → C exist, preserving only the direct relationships.
    */

    int i = 0;
    while (i < p_link_array->log_size)
    {
        t_link link1 = p_link_array->links[i];
        int j = 0;
        int to_remove = 0;
        while (j < p_link_array->log_size && !to_remove)
        {
            if (j != i)
            {
                t_link link2 = p_link_array->links[j];
                if (link1.from == link2.from)
                {
                    int k = 0;
                    while (k < p_link_array->log_size && !to_remove)
                    {
                        if (k != j && k != i)
                        {
                            t_link link3 = p_link_array->links[k];
                            if ((link3.from == link2.to) && (link3.to == link1.to))
                            {
                                to_remove = 1;
                            }
                        }
                        k++;
                    }
                }
            }
            j++;
        }
        if (to_remove)
        {
            p_link_array->links[i] = p_link_array->links[p_link_array->log_size - 1];
            p_link_array->log_size--;
        }
        else
        {
            i++;
        }
    }
}

void displayCharacteristics(t_partition *partition, t_link_array *linkArray) {

    /**
    * @brief Displays the characteristics of each class in a partition.
    * @param partition Pointer to the partition containing the strongly connected components.
    * @param linkArray Pointer to the array of links between classes.
    * @return void
    * @note Prints each class, identifies whether it is transient or persistent,
    *       and indicates whether the overall Markov graph is irreducible.
    */

    for (int i = 0; i < partition->nbClasses; i++) {
        printf("C%d : {",i+1);

        for (int j = 0; j < partition->classes[i].nbVertices - 1; j++) {
            printf("%d,",partition->classes[i].vertices[j]->id);
        }
        printf("%d",partition->classes[i].vertices[partition->classes[i].nbVertices-1]->id);

        printf("}\n");
    }

    for (int i = 0; i < partition->nbClasses; i++) {
        int transient = 0;
        for (int j = 0; j < linkArray->log_size; j++) {
            if (&partition->classes[i] == linkArray->links[j].from) {
                transient = 1;
                printf("The class %s is transient - ",partition->classes[i].name);
                if (partition->classes[i].nbVertices == 1) {
                    printf("state %d is transient.\n",partition->classes[i].vertices[0]->id);
                    break;
                }
                printf("states ");
                for (int k = 0; k < partition->classes[i].nbVertices-1; k++) {
                    printf("%d,",partition->classes[i].vertices[k]->id);
                }
                printf("%d are transient.\n",partition->classes[i].vertices[partition->classes[i].nbVertices-1]->id);
            }
        }

        if (!transient) {
            printf("The class %s is persistent - ",partition->classes[i].name);
            if (partition->classes[i].nbVertices == 1) {
                printf("state %d is persistent - state %d is absorbing.\n",partition->classes[i].vertices[0]->id, partition->classes[i].vertices[0]->id);
            } else {
                printf("states ");
                for (int k = 0; k < partition->classes[i].nbVertices-1; k++) {
                    printf("%d,",partition->classes[i].vertices[k]->id);
                }
                printf("%d are persistent.\n",partition->classes[i].vertices[partition->classes[i].nbVertices-1]->id);
            }
        }
    }

    if (partition->nbClasses == 1) {
        printf("The Markov Graph is irreducible.\n");
    } else {
        printf("The Markov Graph is not irreducible.\n");
    }
}