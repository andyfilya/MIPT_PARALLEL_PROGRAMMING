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


// MultiplyMatrices - функция умножения двух матриц, которая возвращает
// как результат новую матрицу, являющуюся перемножением двух других.
// Данная функция не является оптимальной - здесь все сделано " в лоб".
Matrix* MultiplyMatrices(Matrix* firstMatrix, Matrix* secondMatrix) {
  // обработка стандартной ошибки, в случае если количество столбцов
  // первой матрицы не совпадает с количеством строк во второй
    if (firstMatrix->cols != secondMatrix->rows) {
        printf("error: matrix multiplication");
        return NULL;
    }
    // результирующая матрица, которая будем отображать результат
    // перемножения двух матриц C = AB
    Matrix* resultMatrix = CreateMatrix(firstMatrix->rows, secondMatrix->cols);
    // первым делом будем делать "в лоб" - не думая о кеше, и так далее:
    for (int i = 0; i < firstMatrix->rows; i++) {
        for (int j = 0; j < secondMatrix->cols; j++) {
            resultMatrix->data[i][j] = 0; // Инициализируем элемент
            for (int k = 0; k < firstMatrix->cols; k++) {
                resultMatrix->data[i][j] += firstMatrix->data[i][k] * secondMatrix->data[k][j];
            }
        }
    }
    // возвраещем результат C=AB
    return resultMatrix;
}

int main(int argc, char **argv) {
  Matrix* a = CreateMatrix(2, 2);
  Matrix* b = CreateMatrix(2, 2);

  FillMatrix(a);
  FillMatrix(b);

  PrintMatrix(a);
  printf("\n\n\n\n");
  PrintMatrix(b);
  printf("\n\n\n\n");
  Matrix* c = MultiplyMatrices(a, b);

  PrintMatrix(c);
  return 0;
}