#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int rows;
  int cols;
  double** data;
} Matrix;

Matrix* CreateMatrix(int rows, int cols) {
  Matrix* m = (Matrix*)malloc(sizeof(Matrix));

  m->rows = rows;
  m->cols = cols;

  m->data = (double*)malloc(sizeof(double*) * m->rows);

  for (int i = 0; i < rows; i++) {
    double* row = (double*)malloc(cols * sizeof(double));

    m->data[i] = row;
  }

  return m;
}

void FreeMatrix(Matrix* m) {
  for (int i = 0; i < m->rows; i++) {
    free(m->data[i]);
  }
  free(m->data);
  free(m);
}

void FillMatrix(Matrix* m) {
  const MinValue = 1.0;
  const MaxValue = 20.0;

  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->data[i][j] = MinValue + (rand() % MaxValue - MinValue + 1.0);
    }
  }
}

void PrintMatrix(Matrix* m) {
  for(int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      printf("%lf ", m->data[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {

  Matrix* m = CreateMatrix(15, 15);
  FillMatrix(m);
  PrintMatrix(m);
  FreeMatrix(m);

  return 0;
}