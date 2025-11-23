#ifndef MATRIX_H
#define MATRIX_H

#include "list.h"
#include "tarjan.h"
typedef struct {
  int row;
  int col;
  double **value;
} t_matrix;

t_matrix newMatrix(int row, int col);
t_matrix zeroMatrix(int row, int col);
t_matrix adjMatrix(adjacency_list *adjlist);
t_matrix copyMatrix(t_matrix *matrix);
t_matrix multiplyMatrix(t_matrix *matrix1, t_matrix *matrix2);
double diffmatrix(t_matrix *M, t_matrix *N);
void printMatrix(t_matrix M);
t_matrix powerMatrix(t_matrix M, int p);
void freeMatrix(t_matrix *matrix);
void findStationaryDistribution(t_matrix M);
t_matrix subMatrix(t_matrix matrix, t_partition *part, int class_index);
int getPeriod(t_matrix sub_matrix);

#endif // MATRIX_H