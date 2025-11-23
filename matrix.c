#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "matrix.h"

t_matrix newMatrix(int row, int col) {

    /**
    * @brief Creates a new matrix with specified dimensions and allocates memory.
    * @param row Number of rows in the matrix.
    * @param col Number of columns in the matrix.
    * @return t_matrix Structure with allocated memory for the values.
    */

    t_matrix matrix;
    matrix.row = row;
    matrix.col = col;

    matrix.value = (double **)malloc(row * sizeof(double *));
    if (matrix.value == NULL) {
        perror("Allocation error in newMatrix (rows)");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < matrix.row; i++) {
        matrix.value[i] = (double *)malloc(col * sizeof(double));
        if (matrix.value[i] == NULL) {
            perror("Allocation error in newMatrix (cols)");
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

t_matrix zeroMatrix(int row, int col) {

    /**
    * @brief Creates a new matrix with all elements initialized to zero.
    * @param row Number of rows in the matrix.
    * @param col Number of columns in the matrix.
    * @return t_matrix Structure with all values set to 0.0.
    */

    t_matrix matrix = newMatrix(row, col);

    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.col; j++) {
            matrix.value[i][j] = 0.0;
        }
    }
    return matrix;
}

t_matrix adjMatrix(adjacency_list *adjlist) {

    /**
    * @brief Converts an adjacency list into an adjacency matrix with edge probabilities.
    * @param adjlist Pointer to the adjacency list representing the graph.
    * @return A t_matrix structure representing the adjacency matrix of the graph,
    *         where each element contains the probability of the corresponding edge.
    */

    int n = adjlist->size;

    t_matrix adjmatrix = zeroMatrix(n, n);

    for (int i = 0; i < n; i++) {
        t_cell *curr = adjlist->array[i].head;
        while (curr != NULL) {
            int j = curr->end - 1;
            if (j >= 0 && j < n) {
                adjmatrix.value[i][j] = curr->proba;
            }

            curr = curr->next;
        }
    }
    return adjmatrix;
}

t_matrix copyMatrix(t_matrix *matrix) {

    /**
    * @brief Creates a copy of the given matrix.
    * @param matrix Pointer to the matrix to copy.
    * @return c New t_matrix structure containing the same values as the original matrix.
    */

    t_matrix c = newMatrix(matrix->row, matrix->col);
    for (int i = 0; i < matrix->row; i++) {
        for (int j = 0; j < matrix->col; j++) {
            c.value[i][j] = matrix->value[i][j];
        }
    }
    return c;
}

t_matrix multiplyMatrix(t_matrix *matrix1, t_matrix *matrix2) {

    /**
    * @brief Multiplies two matrices if their dimensions are compatible.
    * @param matrix1 Pointer to the first matrix.
    * @param matrix2 Pointer to the second matrix.
    * @return A new t_matrix structure containing the result of the multiplication.
    * @note Returns an empty matrix if dimensions are incompatible.
    */

    if (matrix1->col != matrix2->row) {
        printf("Error: Matrix dimensions incompatible for multiplication\n");
        t_matrix empty = {0, 0, NULL};
        return empty;
    }

    t_matrix m = newMatrix(matrix1->row, matrix2->col);

    for (int i = 0; i < matrix1->row; i++) {
        for (int j = 0; j < matrix2->col; j++) {
            double sum = 0.0;
            for (int k = 0; k < matrix1->col; k++) {
                sum += matrix1->value[i][k] * matrix2->value[k][j];
            }
            m.value[i][j] = sum;
        }
    }
    return m;
}

double diffMatrix(t_matrix *M, t_matrix *N) {

    /**
    * @brief Computes the sum of absolute differences between two matrices.
    * @param M Pointer to the first matrix.
    * @param N Pointer to the second matrix.
    * @return The sum of absolute differences between corresponding elements of M and N.
    * @note Returns -1.0 if the dimensions of the matrices are incompatible.
    */

    if (M->row != N->row || M->col != N->col) {
        printf("Error: Matrix dimensions incompatible for diff\n");
        return -1.0;
    }

    double sum = 0.0;
    for (int i = 0; i < M->row; i++) {
        for (int j = 0; j < M->col; j++) {
            sum += fabs(M->value[i][j] - N->value[i][j]);
        }
    }
    return sum;
}

void printMatrix(t_matrix M) {

    /**
    * @brief Prints the matrix to the standard output in a formatted style.
    * @param M The matrix to print.
    * @return void
    */

    for (int i = 0; i < M.row; i++) {
        printf("| ");
        for (int j = 0; j < M.col; j++) {
            printf("%5.2f ", M.value[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

void freeMatrix(t_matrix *matrix) {

    /**
    * @brief Frees the memory allocated for a matrix.
    * @param matrix Pointer to the matrix to be freed.
    * @return void
    * @note Sets matrix->value to NULL after freeing to avoid dangling pointers.
    */

    if (matrix == NULL || matrix->value == NULL) return;

    for (int i = 0; i < matrix->row; i++) {
        free(matrix->value[i]);
    }
    free(matrix->value);
    matrix->value = NULL;
}

t_matrix powerMatrix(t_matrix M, int p) {

    /**
    * @brief Computes the power of a square matrix.
    * @param M The matrix to raise to a power.
    * @param p The exponent (positive integer).
    * @return A new t_matrix representing M raised to the power p.
    * @note If p is 1, returns a copy of the original matrix. The function
    *       repeatedly multiplies matrices for p > 1.
    */

    if (p == 1) return copyMatrix(&M);

    t_matrix res = copyMatrix(&M);
    t_matrix temp;

    for (int i = 1; i < p; i++) {
        temp = multiplyMatrix(&res, &M);

        freeMatrix(&res);
        res = temp;
    }
    return res;
}

void findStationaryDistribution(t_matrix M) {

    /**
    * @brief Computes the stationary distribution of a Markov chain represented by a matrix.
    * @param M The transition matrix of the Markov chain.
    * @return void
    * @note Iterates until convergence (diff < 0.01) or 1000 steps; warns if not reached.
    */

    t_matrix current = copyMatrix(&M);
    t_matrix next;
    double diff = 1.0;
    double epsilon = 0.01;
    int step = 1;
    int max_steps = 1000;

    printf("\n[Stationary] Starting convergence analysis (epsilon = %.2f)...\n", epsilon);

    while (diff > epsilon && step < max_steps) {

        next = multiplyMatrix(&current, &M);

        diff = diffMatrix(&next, &current);

        free(current.value);
        current = next;

        step++;
    }

    if (step >= max_steps) {
        printf("[Stationary] Convergence NOT reached after %d steps (Periodic graph?).\n", max_steps);
    } else {
        printf("[Stationary] Convergence reached at step %d (Diff = %.4f)\n", step, diff);
        printf("Stationary Distribution Matrix:\n");
        printMatrix(current);
    }
}

t_matrix subMatrix(t_matrix matrix, t_partition *part, int class_index) {

    /**
    * @brief Extracts a submatrix corresponding to a class of vertices.
    * @param matrix The original matrix.
    * @param part Pointer to the partition containing vertex classes.
    * @param class_index Index of the class to extract.
    * @return A t_matrix representing the submatrix of the specified class.
    * @note Returns an empty matrix if the class_index is invalid.
    */

    if (class_index < 0 || class_index >= part->nbClasses) {
        printf("Error: Invalid class index.\n");
        return newMatrix(0, 0);
    }

    t_class targetClass = part->classes[class_index];
    int size = targetClass.nbVertices;

    t_matrix sub = newMatrix(size, size);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int global_row = targetClass.vertices[i]->id - 1;
            int global_col = targetClass.vertices[j]->id - 1;

            sub.value[i][j] = matrix.value[global_row][global_col];
        }
    }

    return sub;
}

int gcd(int a, int b) {

    /**
    * @brief Computes the greatest common divisor (GCD) of two integers.
    * @param a First integer.
    * @param b Second integer.
    * @return The greatest common divisor of a and b.
    */

    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int gcd_array(int *vals, int nbvals) {

    /**
    * @brief Computes the greatest common divisor (GCD) of an array of integers.
    * @param vals Pointer to the array of integers.
    * @param nbvals Number of elements in the array.
    * @return The greatest common divisor of all elements in the array.
    * @note Returns 0 if the array is empty.
    */

    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; i++) {
        result = gcd(result, vals[i]);
        if (result == 1) return 1;
    }
    return result;
}

int getPeriod(t_matrix sub_matrix) {

    /**
    * @brief Computes the period of a submatrix (used for Markov chain analysis).
    * @param sub_matrix The square submatrix for which the period is computed.
    * @return The period of the submatrix, defined as the GCD of the lengths of
    *         cycles detected on the diagonal.
    * @note Uses matrix powers to identify cycles and then computes the GCD
    *       of the lengths where diagonal elements are non-zero.
    */

    int n = sub_matrix.row;

    int *periods = malloc(n * n * sizeof(int));
    int period_count = 0;

    t_matrix power_matrix = copyMatrix(&sub_matrix);
    t_matrix temp;

    for (int cpt = 1; cpt <= n; cpt++) {

        int diag_nonzero = 0;
        for (int i = 0; i < n; i++) {
            if (power_matrix.value[i][i] > 0.00001) {
                diag_nonzero = 1;
                break;
            }
        }
        if (diag_nonzero) {
            periods[period_count++] = cpt;
        }

        temp = multiplyMatrix(&power_matrix, &sub_matrix);

        freeMatrix(&power_matrix);
        power_matrix = temp;
    }

    freeMatrix(&power_matrix);

    int result = gcd_array(periods, period_count);

    free(periods);
    return result;
}