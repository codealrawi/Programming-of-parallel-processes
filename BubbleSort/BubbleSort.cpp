#include <iostream>
#include <omp.h>
#include <ctime>


using namespace std;

void ParallelBubbleSort(int arr[], int arrLen, int numThreads)
{
	double start, end;

	omp_set_num_threads(numThreads);

	start = omp_get_wtime();

	for (int i = 0; i < arrLen; i++)
	{
		if (i % 2 == 0)
		{
#pragma omp parallel for
			for (int j = 0; j < arrLen; j += 2)
				if (j < arrLen - 1)
					if (arr[j] > arr[j + 1])
						swap(arr[j], arr[j + 1]);
		}
		else
		{
#pragma omp parallel for
			for (int j = 1; j < arrLen; j += 2)
				if (j < arrLen - 1)
					if (arr[j] > arr[j + 1])
						swap(arr[j], arr[j + 1]);
		}
	}

	end = omp_get_wtime();

	printf(" %d threads = %lf sec.\n", numThreads, end - start);
}

int main()
{
	const int arrLen = 10000;
	int arr[arrLen] = {};

	srand(time(NULL));
	for (int i = 0; i < arrLen; i++)
		arr[i] = rand() % 100;

	ParallelBubbleSort(arr, arrLen, 2);
	ParallelBubbleSort(arr, arrLen, 5);
	ParallelBubbleSort(arr, arrLen, 9);

	return 0;
}