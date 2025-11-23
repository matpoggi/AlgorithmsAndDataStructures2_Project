#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "matrix.h"

t_matrix newMatrix(int row, int col) {
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
    t_matrix matrix = newMatrix(row, col);

    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.col; j++) {
            matrix.value[i][j] = 0.0;
        }
    }
    return matrix;
}

t_matrix adjMatrix(adjacency_list *adjlist) {
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
    t_matrix c = newMatrix(matrix->row, matrix->col);
    for (int i = 0; i < matrix->row; i++) {
        for (int j = 0; j < matrix->col; j++) {
            c.value[i][j] = matrix->value[i][j];
        }
    }
    return c;
}

t_matrix multiplyMatrix(t_matrix *matrix1, t_matrix *matrix2) {
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

double diffmatrix(t_matrix *M, t_matrix *N) {
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
    if (matrix == NULL || matrix->value == NULL) return;

    for (int i = 0; i < matrix->row; i++) {
        free(matrix->value[i]);
    }
    free(matrix->value);
    matrix->value = NULL;
}

t_matrix powerMatrix(t_matrix M, int p) {
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
    t_matrix current = copyMatrix(&M);
    t_matrix next;
    double diff = 1.0;
    double epsilon = 0.01;
    int step = 1;
    int max_steps = 1000;

    printf("\n[Stationary] Starting convergence analysis (epsilon = %.2f)...\n", epsilon);

    while (diff > epsilon && step < max_steps) {

        next = multiplyMatrix(&current, &M);

        diff = diffmatrix(&next, &current);

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
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int gcd_array(int *vals, int nbvals) {
    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; i++) {
        result = gcd(result, vals[i]);
        if (result == 1) return 1;
    }
    return result;
}

int getPeriod(t_matrix sub_matrix) {
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