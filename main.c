#include <stdio.h>
#include <string.h>

#include "list.h"
#include "tarjan.h"
#include "hasse.h"
#include "matrix.h"

#define PRINT_SECTION(title) printf("\n=======================================\n   %s\n=======================================\n", title)
#define PRINT_STEP(msg) printf("[Step] %s\n", msg)

int main() {
    // ---------------------------------------------------------
    // INITIALISATION & INPUT
    // ---------------------------------------------------------
    PRINT_SECTION("INITIALIZATION");

    char filepath[200];
    char prefix[] = "..\\data\\";
    char fullpath[250];

    printf("Please enter the file name (e.g., exemple_meteo.txt):\n-> ");

    if (fgets(filepath, sizeof(filepath), stdin) != NULL) {
        filepath[strcspn(filepath, "\n")] = 0;

        for (int i = 0; filepath[i] != '\0'; i++) {
            if (filepath[i] == '/') {
                filepath[i] = '\\';
            }
        }
    }

    strcpy(fullpath, prefix);
    strcat(fullpath, filepath);
    printf("Loading file: %s\n", fullpath);


    // ---------------------------------------------------------
    // PART 1 : LOADING & MARKOV
    // ---------------------------------------------------------
    PRINT_SECTION("PART 1: GRAPH LOADING & CHECKS");

    // 1. Reading of the graph
    PRINT_STEP("Reading graph from file...");
    adjacency_list G = readGraph(fullpath);

    // 2. Vérification if it's a Markov graph or not
    PRINT_STEP("Checking Markov property...");
    isMarkov(G);

    // 3. "Drawing" of the graph
    PRINT_STEP("Generating graph visualization file...");
    drawGraph(G);
    printf("-> File '../output/graph.txt' created.\n");


    // ---------------------------------------------------------
    // PART2 : TOPOLOGY (TARJAN & HASSE)
    // ---------------------------------------------------------
    PRINT_SECTION("PART 2: CONNECTED COMPONENTS & HASSE");

    // 1. Tarjan algorithm (Partition of class)
    PRINT_STEP("Running Tarjan's algorithm to find components...");
    t_partition *partition = tarjan(&G);
    displayPartition(partition);

    // 2. Création between classes (Hasse)
    PRINT_STEP("Building Hasse diagram structure...");
    t_link_array *links = initLinkArray(&G, partition);

    // 3. Generation of file Hasse for Mermaid
    PRINT_STEP("Generating Hasse diagram file...");
    textFileHasse(partition, links);
    printf("-> File 'textFileHasse.txt' created.\n");

    // 4. Analysis of properties (Transient vs Persistent)
    PRINT_STEP("Analyzing class characteristics (Transient/Persistent):");
    displayCharacteristics(partition, links);


    // ---------------------------------------------------------
    // PART 3 : MATRIX CALCULATIONS
    // ---------------------------------------------------------
    PRINT_SECTION("PART 3: MATRIX CALCULATIONS");

    PRINT_STEP("Adjacency Matrix M (Transition Probabilities):");
    t_matrix M = adjMatrix(&G);
    printMatrix(M);

    PRINT_STEP("Calculating M^3 (Weather in 3 days):");
    t_matrix M3 = powerMatrix(M, 3);
    printMatrix(M3);

    PRINT_STEP("Calculating M^7 (Weather in 7 days):");
    t_matrix M7 = powerMatrix(M, 7);
    printMatrix(M7);

    PRINT_STEP("Checking for Stationary Distribution (Convergence):");
    findStationaryDistribution(M);

    PRINT_STEP("Extracting Sub-matrices for each Class:");
    for (int i = 0; i < partition->nbClasses; i++) {
        if (partition->classes[i].nbVertices > 0) {
            printf("\n--- Sub-matrix for Component C%d ---\n", i + 1);
            t_matrix subM = subMatrix(M, partition, i);
            printMatrix(subM);
            freeMatrix(&subM);
        }
    }

    // ---------------------------------------------------------
    // PART 3 (BONUS) : PÉRIODICITY
    // ---------------------------------------------------------
    PRINT_SECTION("PART 3 (BONUS): PERIODICITY ANALYSIS");

    for (int i = 0; i < partition->nbClasses; i++) {
        if (partition->classes[i].nbVertices > 0) {

            t_matrix subM = subMatrix(M, partition, i);
            int d = getPeriod(subM);

            printf("Component C%d (Size: %d) -> Period d = %d\n", i + 1, partition->classes[i].nbVertices, d);

            if (d == 1) {
                printf("   -> This class is Aperiodic (Regular).\n");
            } else {
                printf("   -> This class is Periodic.\n");
            }
            freeMatrix(&subM);
        }
    }

    PRINT_SECTION("END OF PROGRAM");

    freeMatrix(&M);
    freeMatrix(&M3);
    freeMatrix(&M7);


    return 0;
}