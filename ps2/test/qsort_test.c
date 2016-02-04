#include "qsort.h"
#include "point.h"
#include "scan_test.h"
#include "util.h"

#include "assert.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "string.h"

#include "omp.h"

void scale_test(int procs, int size)
{
	for (int i = 1; i < procs; i *= 2)
	{
		printf("Number of threads: %d\n", i);
		omp_set_num_threads(i);
		basic_test(10, size, size);
	}
}

int main(int argv, char** argc) 
{
	if (argv < 4)
	{
		printf("Usage: ./test_qsort [iterations] [max_problem_size] [increment_step]\n");
		return 1;
	}
	scale_test(32, atoi(argc[2]));
	basic_test(atoi(argc[1]), atoi(argc[2]), atoi(argc[3]));
	//_CrtDumpMemoryLeaks();
	return 0;
}

void basic_test(size_t times, size_t size, size_t increment) 
{
	
	int i, j;
	int* rand_int; double* rand_double; float* rand_float; long* rand_long; Point* rand_point;
	double int_time = 0, double_time = 0, float_time = 0, long_time = 0, point_time = 0;
	Point int_time_single, double_time_single, long_time_single, point_time_single, float_time_single;
	double int_time_std = 0, double_time_std = 0, float_time_std = 0, long_time_std = 0, point_time_std = 0;
	for (j = increment; j <= size; j += increment)
	{
		for (i = 0; i < times; ++i)
		{
			rand_int = random_int_array(j);
			rand_double = random_double_array(j);
			rand_float = random_float_array(j);
			rand_long = random_long_array(j);
			rand_point = random_point_array(j);
			int_time_single = test_array(rand_int, j, sizeof(int), *compar_int);
			double_time_single = test_array(rand_double, j, sizeof(double), *compar_double);
			float_time_single = test_array(rand_float, j, sizeof(float), *compar_float);
			long_time_single = test_array(rand_long, j, sizeof(long), *compar_long);
			point_time_single = test_array(rand_point, j, sizeof(Point), *compar_point);

			int_time += int_time_single.x;
			double_time += double_time_single.x;
			long_time += long_time_single.x;
			float_time += float_time_single.x;
			point_time += point_time_single.x;

			int_time_std += int_time_single.y;
			double_time_std += double_time_single.y;
			long_time_std += long_time_single.y;
			float_time_std += float_time_single.y;
			point_time_std += point_time_single.y;

		}
		printf("(int) array_size: %d, sort_time (ms): %g\n", j, int_time/times);
		printf("(double) array_size: %d, sort_time (ms): %g\n", j, double_time/times);
		printf("(float) array_size: %d, sort_time (ms): %g\n", j, float_time/times);
		printf("(long) array_size: %d, sort_time (ms): %g\n", j, long_time/times);
		printf("(point) array_size: %d, sort_time (ms): %g\n", j, point_time/times);
		printf("(int) array_size: %d, sort_time_std (ms): %g\n", j, int_time_std/times);
		printf("(double) array_size: %d, sort_time_std (ms): %g\n", j, double_time_std/times);
		printf("(float) array_size: %d, sort_time_std (ms): %g\n", j, float_time_std/times);
		printf("(long) array_size: %d, sort_time_std (ms): %g\n", j, long_time_std/times);
		printf("(point) array_size: %d, sort_time_std (ms): %g\n", j, point_time_std/times);
		printf("\n");
		int_time = double_time = float_time = long_time = point_time = 0;
		int_time_std = double_time_std = long_time_std = float_time_std = point_time_std = 0;
	}
}

Point test_array(void* arri_1, size_t num, size_t size, int (*compar) (const void*, const void*)) 
{
	void* arri_2 = duplicate_array(arri_1, num, size);
	assert(are_equal(arri_1, arri_2, num, size, compar));
	clock_t difference;	
	clock_t start = clock();
	my_qsort(arri_1, num, size, compar);
	difference = clock() - start;	
	clock_t difference_std;
	start = clock();
	qsort(arri_2, num, size, compar);	
	difference_std = clock() - start;
	assert(are_equal(arri_1, arri_2, num, size, compar));
	Point p = { .x = difference * 1000/CLOCKS_PER_SEC, .y = difference_std * 1000/CLOCKS_PER_SEC};
	free(arri_1); free(arri_2);
	return p;
}

