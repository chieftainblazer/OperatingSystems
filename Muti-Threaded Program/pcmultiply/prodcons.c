/*
 *  prodcons module
 *  Producer Consumer module
 *
 *  Implements routines for the producer consumer module based on
 *  chapter 30, section 2 of Operating Systems: Three Easy Pieces
 *
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 */

// Include only libraries for this module
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "counter.h"
#include "matrix.h"
#include "pcmatrix.h"
#include "prodcons.h"


// Define Locks and Condition variables here
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int bufferStart = 0;
int bufferEnd = 0;
int count = 0;
int done = 0;
Matrix* M1 = NULL;
Matrix* M2 = NULL;
Matrix* M3 = NULL;

// Producer consumer data structures

// Bounded buffer bigmatrix defined in prodcons.h
Matrix * bigmatrix[MAX];

// Bounded buffer put() get()
int put(Matrix * value)
{
	*(bigmatrix + bufferEnd) = value;
	bufferEnd = (bufferEnd + 1) % MAX;
	count = count + 1;
	return 0;		
}

Matrix * get()
{
	Matrix* matrixRetrieved = NULL;
	matrixRetrieved = *(bigmatrix + bufferStart);
	bufferStart = (bufferStart + 1) % MAX;
	count = count - 1;
	return matrixRetrieved;
}

// Matrix PRODUCER worker thread
void *prod_worker(void *arg)
{
	ProdConsStats * prod_count = (ProdConsStats*) arg;
	for (int i = 0; i < LOOPS; i++) {
		pthread_mutex_lock(&lock);
		while (count == MAX) {
			pthread_cond_wait(&condition, &lock);
		}
		Matrix* M1 = GenMatrixRandom();
		put(M1);
		prod_count -> matrixtotal++;
		prod_count -> sumtotal += SumMatrix(M1);
		pthread_cond_signal(&condition);
		pthread_mutex_unlock(&lock);
	}
	return prod_count;
}

// Matrix CONSUMER worker thread
void *cons_worker(void *arg)
{
	ProdConsStats * con_count = (ProdConsStats*) arg;
	for (int i = 0; i < LOOPS; i++) {
		pthread_mutex_lock(&lock);
		while (count == 0) {
			pthread_cond_wait(&condition, &lock);
		}
		if (M1 == NULL && M2 == NULL) {
			M1 = get();
			con_count -> matrixtotal++;
			con_count -> sumtotal += SumMatrix(M1);
		} else if (M1 != NULL && M2 == NULL) {
			M2 = get();
			con_count -> matrixtotal++;
			con_count -> sumtotal += SumMatrix(M2);
			M3 = MatrixMultiply(M1, M2);
			if (M3 != NULL) {
				con_count -> multtotal++;
				DisplayMatrix(M1, stdout);
				printf("	X\n");
				DisplayMatrix(M2, stdout);
				printf("	=\n");
				DisplayMatrix(M3, stdout);
				printf("\n");
				FreeMatrix(M1);
				FreeMatrix(M2);
				FreeMatrix(M3);
				M1 = NULL;
				M2 = NULL;
				M3 = NULL;
			} else {
				FreeMatrix(M2);
				M2 = NULL;
			}
		}
		if (con_count -> matrixtotal == LOOPS) {
			if (M1 != NULL) {
				FreeMatrix(M1);
				M1 = NULL;
			}	
		} else {
			pthread_cond_signal(&condition);
		}	
		pthread_mutex_unlock(&lock);
	}	
	return con_count;
}
