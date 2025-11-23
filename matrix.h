#ifndef MATRIX_H
#define MATRIX_H

#include "list.h"
#include "tarjan.h"

// Creation of the matrix type
typedef struct {
  int row;  // number of rows of the matrix
  int col;  // number of columns of the matrix
  double **value; // value at the given column and row
} t_matrix;

// Function declarations
t_matrix newMatrix(int row, int col);
t_matrix zeroMatrix(int row, int col);
t_matrix adjMatrix(adjacency_list *adjlist);
t_matrix copyMatrix(t_matrix *matrix);
t_matrix multiplyMatrix(t_matrix *matrix1, t_matrix *matrix2);
t_matrix powerMatrix(t_matrix M, int p);
t_matrix subMatrix(t_matrix matrix, t_partition *part, int class_index);

double diffMatrix(t_matrix *M, t_matrix *N);

void printMatrix(t_matrix M);
void freeMatrix(t_matrix *matrix);
void findStationaryDistribution(t_matrix M);

int getPeriod(t_matrix sub_matrix);

#endif // MATRIX_H