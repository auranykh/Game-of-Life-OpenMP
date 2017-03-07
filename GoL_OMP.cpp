/*
	Name: Michael Pitts
	BlazerId: mchlpitt
	Course Section: CS 432
	Homework #: 4
*/
//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <omp.h>

using namespace std;

//define global variables
int M_SIZE;
int ITERATIONS;
int THREAD_COUNT;
int **matrix;
int **previous_matrix;

double getTime()
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return((double)tval.tv_sec + (double)tval.tv_usec / 1000000.0);
}

void validateConditions(int argc)
{
	if (argc != 4)
	{
		cout << argc << "\n";
		cout << "Invalid arguments, call program with arguments for a matrix size, number of iterations, and thread count";
	}
}

void initializeMatrix()
{
	//initialize ghost cells
	for (int i = 0; i < M_SIZE; i++)
	{
		matrix[i][0] = 0;
		matrix[0][i] = 0;
		matrix[i][M_SIZE - 1] = 0;
		matrix[M_SIZE - 1][i] = 0;
	}

	//initialize board where each cell has a 20% chance of being alive
	for (int i = 1; i < M_SIZE - 1; i++)
	{
		for (int j = 1; j < M_SIZE - 1; j++)
		{
			if (rand() % 100 <= 50)
			{
				matrix[i][j] = 1;
			}
			else
			{
				matrix[i][j] = 0;
			}
		}
	}
}

void setPrevMatrix()
{
	//set previous matrix = matrix
	for (int i = 1; i < M_SIZE - 1; i++)
	{
		for (int j = 1; j < M_SIZE - 1; j++)
		{
			previous_matrix[i][j] = matrix[i][j];
		}
	}
}

void advanceGenerations()
{
	//iterate generations
	#pragma omp parallel for num_threads(THREAD_COUNT)
	for (int k = 0; k < ITERATIONS; k++)
	{
		setPrevMatrix(); //set previous matrix
		//advance generation
		for (int i = 1; i < M_SIZE - 1; i++)
		{
			for (int j = 1; j < M_SIZE - 1; j++)
			{
				//if cell is dead
				if (previous_matrix[i][j] == 0)
				{
					int neighbors = 0;
					if (previous_matrix[i - 1][j - 1] == 1) neighbors++;
					if (previous_matrix[i][j - 1] == 1) neighbors++;
					if (previous_matrix[i + 1][j - 1] == 1) neighbors++;
					if (previous_matrix[i - 1][j] == 1) neighbors++;
					if (previous_matrix[i + 1][j] == 1) neighbors++;
					if (previous_matrix[i - 1][j + 1] == 1) neighbors++;
					if (previous_matrix[i][j + 1] == 1) neighbors++;
					if (previous_matrix[i + 1][j + 1] == 1) neighbors++;
					if (neighbors == 3)
					{
						matrix[i][j] = 1;
					}
					else matrix[i][j] = 0;
					neighbors = 0;
				}

				//if cell is alive
				else
				{
					int neighbors = 0;
					if (previous_matrix[i - 1][j - 1] == 1) neighbors++;
					if (previous_matrix[i][j - 1] == 1) neighbors++;
					if (previous_matrix[i + 1][j - 1] == 1) neighbors++;
					if (previous_matrix[i - 1][j] == 1) neighbors++;
					if (previous_matrix[i + 1][j] == 1) neighbors++;
					if (previous_matrix[i - 1][j + 1] == 1) neighbors++;
					if (previous_matrix[i][j + 1] == 1) neighbors++;
					if (previous_matrix[i + 1][j + 1] == 1) neighbors++;
					if (neighbors <= 1 || neighbors >= 4)
					{
						matrix[i][j] = 0;
					}
					else matrix[i][j] = 1;
					neighbors = 0;
				}
			}
		}
		//for (int i = 0; i < M_SIZE; i++) {
		//	for (int j = 0; j < M_SIZE; j++) {
		//		cout << matrix[i][j];
		//	}
		//	cout << endl;
		//}
		//cout << endl;
	}
}

void initializePrevMatrix()
{
	for (int i = 0; i < M_SIZE; i++)
	{
		previous_matrix[i] = new int[M_SIZE];
	}
	for (int i = 0; i < M_SIZE; i++)
	{
		previous_matrix[i][0] = 0;
		matrix[0][i] = 0;
		matrix[i][M_SIZE - 1] = 0;
		matrix[M_SIZE - 1][i] = 0;
	}
}

void calculateTime(double stop_time, double start_time)
{
	//output time to a file
	ofstream output_time;
	output_time.open("execution_times.txt", ios::app);
	output_time << "Execution Time: " << stop_time - start_time << " s for " << M_SIZE - 2 << " size and " << ITERATIONS << " generations with " << THREAD_COUNT << " thread(s).\n";
	output_time.close();
	//cout << "Execution Time: " << double(stop_time - start_time) / (double)(CLOCKS_PER_SEC) << " s for " << M_SIZE - 2 << " size and " << ITERATIONS << " generations." << endl;
}

void flush()
{
	//flush
	for (int i = 0; i < M_SIZE; i++)
	{
		delete[] matrix[i];
		delete[] previous_matrix[i];
	}

	delete[] matrix;
	delete[] previous_matrix;
}

int main(int argc, char * argv[])
{
	validateConditions(argc);

	M_SIZE = std::atoi(argv[1]) + 2;
	ITERATIONS = std::atoi(argv[2]);
	THREAD_COUNT = std::atoi(argv[3]);

	matrix = new int*[M_SIZE];
	for (int i = 0; i < M_SIZE; i++)
	{
		matrix[i] = new int[M_SIZE];
	}

	//start clock for determining execution time
	double start_time = getTime();

	initializeMatrix();

	//define previous_matrix for determining new generations
	previous_matrix = new int*[M_SIZE]; //int **matrix = new int*[M_SIZE];
	initializePrevMatrix();

	//print initial board
	//for (int i = 0; i < M_SIZE; i++) {
	//	for (int j = 0; j < M_SIZE; j++) {
	//		cout << matrix[i][j];
	//	}
	//	cout << endl;
	//}
	//cout << endl;

	advanceGenerations();

	//iterate generations
	//for (int i = 0; i < ITERATIONS; i++)
	//{
	//	setPrevMatrix();
	//	for (thread = 0; thread < THREAD_COUNT; thread++)
	//	{
	//		pthread_create(&thread_handles[thread], NULL, threadFunction, (void*)thread);
	//	}
	//	//print (should be commented out to improve execution)
		//for (int i = 0; i < M_SIZE; i++) {
		//	for (int j = 0; j < M_SIZE; j++) {
		//		cout << matrix[i][j];
		//	}
		//	cout << endl;
		//}
		//cout << endl;
	//	for (thread = 0; thread < THREAD_COUNT; thread++)
	//	{
	//		pthread_join(thread_handles[thread], NULL);
	//	}
	//}
	//print
	//for (int i = 0; i < M_SIZE; i++) {
	//	for (int j = 0; j < M_SIZE; j++) {
	//		cout << matrix[i][j];
	//	}
	//	cout << endl;
	//}

	//end clock for determining exeuction time
	double stop_time = getTime();

	calculateTime(stop_time, start_time);

	flush();

	return 0;
}
