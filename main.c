#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


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

// OptimizeMultiplyMatrices - функция умножения двух матриц, которая возвращает
// как результат новую матрицу, являющуюся перемножением двух других.
// Данная функция является оптимальной - здесь все сделано таким образом,
// что мы пользуется знаниями о кеш линии, или Cache Lines и Cache Hits.
// что не так? В предыдущем, неоптимизированном подходе, мы знали о том, что
// матрицы хранятся в виде сложенных друг на друга плиток, но не использовали этого
// знания и получали много кеш-миссов, которые обращаются к информации, которая хранится
// не в кеше - это медленно! Будем делать таким образом, чтобы обращаться к информации, которая
// уже есть в кеше - так будет намного быстрее, потому что мы не обращаемся к памяти
// напрямую - мы работаем через кеш.
Matrix* OptimizeMultiplyMatrices(Matrix* firstMatrix, Matrix* secondMatrix) {
    if (firstMatrix->cols != secondMatrix->rows) {
        printf("error: matrix multiplication");
        return NULL;
    }
    // результирующая матрица, которая будем отображать результат
    // перемножения двух матриц C = AB
    Matrix* resultMatrix = CreateMatrix(firstMatrix->rows, secondMatrix->cols);
		// теперь, заботясь о кеше - перемножаем матрицы.
    for (int i = 0; i < firstMatrix->rows; i++) {
        for (int k = 0; k < secondMatrix->cols; k++) {
            for (int j = 0; j < firstMatrix->cols; j++) {
                resultMatrix->data[i][j] += firstMatrix->data[i][k] * secondMatrix->data[k][j];
            }
        }
    }
    // возвраещем результат C=AB
    return resultMatrix;
}

void RunProgram() {
	FILE* file = fopen("times.csv", "w");
	fprintf(file, "Size,Time\n");

	for (int size = 1; size <= 100; size++) {
		Matrix* a = CreateMatrix(size, size);
		Matrix* b = CreateMatrix(size, size);

		FillMatrix(a);
		FillMatrix(b);

		clock_t start = clock();
    Matrix* c = OptimizeMultiplyMatrices(a, b);
    clock_t end = clock();

		double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
		fprintf(file, "%d,%f\n", size, time_spent);
		printf("Size: %d, Time: %f seconds\n", size, time_spent);

		FreeMatrix(a);
		FreeMatrix(b);
		FreeMatrix(c);
	}

	fclose(file);
}
typedef struct {
  int thread_id;
  Matrix *A;
  Matrix *B;
  Matrix *C;
  int start_row;
  int end_row;
  int tile_size;
} ThreadArgs;

// MultiplyParallelBlocks - функция умножения матриц блочного типа
// не оптимизированный способ (опять используем много кеш-миссов)
void* MultiplyParallelBlocks(void* args) {
	const int tile_size = 2;
  ThreadArgs* data = (ThreadArgs*)args;
  int thread_id = data->thread_id;
  Matrix *A = data->A;
  Matrix *B = data->B;
  Matrix *C = data->C;

	for (int i = data->start_row; i < data->end_row; i += tile_size) {
		for (int j = 0; j < B->cols; j += tile_size) {
			for (int k = 0; k < A->cols; k += tile_size) {
				for (int ii = i; ii < i + tile_size && ii < A->rows; ++ii) {
					for (int jj = j; jj < j + tile_size && jj < B->cols; ++jj) {
							double sum = 0.0;
							for (int kk = k; kk < k + tile_size && kk < A->cols; ++kk) {
									sum += A->data[ii][kk] * B->data[kk][jj];
							}
							C->data[ii][jj] += sum;
					}
				}
			}
		}
	}

    return NULL;

  pthread_exit(NULL);
}

// MultiplyParallelOptimizeBlocks - функция умножения матриц блочного типа
// оптимизированный способ (используем свойтва кеша, а не долбимся в память напрямую).
void* MultiplyOtimizeParallelBlocks(void* args) {
	const int tile_size = 2;
  ThreadArgs* data = (ThreadArgs*)args;
  int thread_id = data->thread_id;
  Matrix *A = data->A;
  Matrix *B = data->B;
  Matrix *C = data->C;

	for (int ih = data->start_row; ih < data->end_row; ih += tile_size) {
		for (int jh = 0; jh < B->cols; jh += tile_size) {
			for (int kh = 0; kh < A->cols; kh += tile_size) {
				for (int il = 0; il < tile_size ; ++il) {
					for (int jl= 0; jl< tile_size ; ++jl) {
							for (int kl = 0; kl < tile_size ; ++kl) {
								C->data[ih+il][jh+jl] += A->data[ih+il][kh+kl] + B->data[kh+kl][jh+jl];
							}
					}
				}
			}
		}
	}

    return NULL;

  pthread_exit(NULL);
}

// MultiplyMatricesParallel - распараллеленное блочное умножение матриц 
// здесь также использовано оптмизированное умножение матриц, дружелюбное
// к кешу.
Matrix* MultiplyMatricesParallel(Matrix* A, Matrix* B, int numThreads) {
  if (A->cols != B->rows) {
    fprintf(stderr, "Error: Incompatible matrix dimensions for multiplication.\n");
    exit(1);
  }

  Matrix* C = CreateMatrix(A->rows, B->cols);

  pthread_t threads[numThreads];
  ThreadArgs thread_data[numThreads];
	int rows_per_thread = A->rows / numThreads;
  for (int i = 0; i < numThreads; i++) {
    thread_data[i].thread_id = i;
    thread_data[i].A = A;
    thread_data[i].B = B;
    thread_data[i].C = C;
		thread_data[i].start_row = i * rows_per_thread;
    thread_data[i].end_row = (i + 1) * rows_per_thread;
    pthread_create(&threads[i], NULL, MultiplyParallelBlocks, (void*)&thread_data[i]);
  }

  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return C;
}

void RunParallelProgram() {
	Matrix* A = CreateMatrix(32, 32);
  Matrix* B = CreateMatrix(32, 32);

	FillMatrix(A);
	FillMatrix(B);

	PrintMatrix(A);
	PrintMatrix(B);

	FILE* file = fopen("parallel.csv", "w");
	fprintf(file, "Thread,Time\n");

	for (int numTheads = 1; numTheads <= 16; numTheads++) {
		time_t start = clock();
    Matrix* C = MultiplyMatricesParallel(A, B, numTheads);
    clock_t end = clock();

		double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
		fprintf(file, "%d,%f\n", numTheads, time_spent);
    printf("Number of threads: %d\n", numTheads);

		PrintMatrix(C);
    FreeMatrix(C);
  }

	FreeMatrix(A);
	FreeMatrix(B);
	fclose(file);
}

Matrix* MultiplyMatricesOptimizeParallel(Matrix* A, Matrix* B, int numThreads) {
  if (A->cols != B->rows) {
    fprintf(stderr, "Error: Incompatible matrix dimensions for multiplication.\n");
    exit(1);
  }

  Matrix* C = CreateMatrix(A->rows, B->cols);

  pthread_t threads[numThreads];
  ThreadArgs thread_data[numThreads];
	int rows_per_thread = A->rows / numThreads;
  for (int i = 0; i < numThreads; i++) {
    thread_data[i].thread_id = i;
    thread_data[i].A = A;
    thread_data[i].B = B;
    thread_data[i].C = C;
		thread_data[i].start_row = i * rows_per_thread;
    thread_data[i].end_row = (i + 1) * rows_per_thread;
    pthread_create(&threads[i], NULL, MultiplyOtimizeParallelBlocks, (void*)&thread_data[i]);
  }

  for (int i = 0; i < numThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  return C;
}

void RunOptimizeParallelProgram() {
	Matrix* A = CreateMatrix(32, 32);
  Matrix* B = CreateMatrix(32, 32);

	FillMatrix(A);
	FillMatrix(B);

	PrintMatrix(A);
	PrintMatrix(B);

	FILE* file = fopen("parallel_optimize.csv", "w");
	fprintf(file, "Thread,Time\n");

	for (int numTheads = 1; numTheads <= 16; numTheads++) {
		time_t start = clock();
    Matrix* C = MultiplyMatricesOptimizeParallel(A, B, numTheads);
    clock_t end = clock();

		double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
		fprintf(file, "%d,%f\n", numTheads, time_spent);
    printf("Number of threads: %d\n", numTheads);

		PrintMatrix(C);
    FreeMatrix(C);
  }

	FreeMatrix(A);
	FreeMatrix(B);
	fclose(file);
}


int main(int argc, char **argv) {
	RunOptimizeParallelProgram();

  return 0;
}