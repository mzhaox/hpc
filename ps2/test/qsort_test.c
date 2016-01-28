#include "qsort.h"
#include "assert.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "string.h"

void print_int_array(int* base, size_t num) 
{
	size_t i;
	for (i = 0; i < num; ++i) 
	{
		printf("%d ", base[i]);
	}
	printf("\n");
}

int main(int argv, char** argc) 
{
	if (argv < 4)
	{
		printf("Usage: ./test_qsort [iterations] [max_problem_size] [increment_step]\n");
		return 1;
	}
	//test_swap(atoi(argc[1]));
	basic_test(atoi(argc[1]), atoi(argc[2]), atoi(argc[3]));
	return 0;
}


bool is_ordered(void* base, size_t num, size_t size, int (*compar)(const void*, const void*))
{
	size_t i;
	for (i = 0; i < num - 1; ++i)
 	{
		if (compar(((char*)base) + size*i, ((char*)base) + size*(i+1)) > 0)
			return false;
  }	
	return true;
}

//void test_swap(size_t times) 
//{
//	time_t t;
//	srand((unsigned) time(&t));
//	int i;
//	#pragma omp parallel for
//	for (i = 0; i < times; ++i)
//	{
//		int a = rand();
//		int b = rand();
//		int a_copy = a;
//		int b_copy = b;
//		int* aa = &a;
//		int* bb = &b;	
//		swap(aa, bb, sizeof(int));
//		assert((*aa) == b_copy);
//		assert((*bb) == a_copy);
//	}
//}

void basic_test(size_t times, size_t size, size_t increment) 
{
	
	int i, j;
	double int_time = 0, double_time = 0, float_time = 0, long_time = 0, point_time = 0;
	Point int_time_single, double_time_single, long_time_single, point_time_single, float_time_single;
	double int_time_std = 0, double_time_std = 0, float_time_std = 0, long_time_std = 0, point_time_std = 0;
	for (j = 0; j < size; j += increment)
	{
//		#pragma omp parallel for default(none) \
				private(int_time_single, double_time_single, float_time_single, long_time_single, point_time_single) \
				shared(int_time, double_time, long_time, float_time, point_time) \
				shared(i, times, increment) private(j)
		#pragma omp parallel for private(int_time_single, double_time_single, float_time_single, long_time_single, point_time_single)
		for (i = 0; i < times; ++i)
		{
			int_time_single = test_array(random_int_array(j), j, sizeof(int), *compar_int);
			double_time_single = test_array(random_double_array(j), j, sizeof(double), *compar_double);
			float_time_single = test_array(random_float_array(j), j, sizeof(float), *compar_float);
			long_time_single = test_array(random_long_array(j), j, sizeof(long), *compar_long);
			point_time_single = test_array(random_point_array(j), j, sizeof(Point), *compar_point);

			#pragma omp atomic
			int_time += int_time_single.x;
			#pragma omp atomic
			double_time += double_time_single.x;
			#pragma omp atomic
			long_time += long_time_single.x;
			#pragma omp atomic
			float_time += float_time_single.x;
			#pragma omp atomic
			point_time += point_time_single.x;
			#pragma omp atomic
			int_time_std += int_time_single.y;
			#pragma omp atomic
			double_time_std += double_time_single.y;
			#pragma omp atomic
			long_time_std += long_time_single.y;
			#pragma omp atomic
			float_time_std += float_time_single.y;
			#pragma omp atomic
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
	free(arri_2);
	Point p = { .x = difference * 1000/CLOCKS_PER_SEC, .y = difference_std * 1000/CLOCKS_PER_SEC};
	return p;
}

bool are_equal(void* arr1, void* arr2, size_t num, size_t size, int (*compar)(const void*, const void*))
{
	size_t i;
	for (i = 0; i < num; ++i) 
	{
		if (compar(((char*)arr1) + size*i, ((char*)arr2) + size*i) != 0) 
		{
			return false;
		}
	}
	return true;
}

void* duplicate_array(void* arr, size_t num, size_t size) 
{
	void* arr2 = calloc(num, size); 
	memcpy(arr2, arr, num*size);
	return arr2;
}

int compar_int(const int* a, const int* b) 
{
	if (*a > *b)
		return 1;
	else if (*a == *b)
		return 0;
	else 
		return -1;
}

int compar_long(const long* a, const long* b)
{
	if (*a > *b)
		return 1;
	else if (*a == *b)
		return 0;
	else 
		return -1;
}

int compar_double(const double* a, const double* b)
{
	if (*a > *b)
		return 1;
	else if (*a == *b)
		return 0;
	else 
		return -1;
}

int compar_float(const float* a, const float* b) 
{
	if (*a > *b)
		return 1;
	else if (*a == *b)
		return 0;
	else 
		return -1;
}

int compar_point(const Point* a, const Point* b)
{
	if (a->y > b->y)
		return 1;
	else if (a->y < b->y)
		return -1;
	else
		return compar_double(&a->x, &b->x);
}

int* random_int_array(size_t num) 
{
	time_t t;
	srand((unsigned) time(&t));
	int* arr = (int*) calloc(num, sizeof(int));
	size_t i;
	for (i = 0; i < num; ++i)
	{
		arr[i] = rand() % 10000;
	}
	return arr;
}

long* random_long_array(size_t num)
{
	time_t t;
	srand((unsigned) time(&t));
	long* arr = (long*) calloc(num, sizeof(long));
	size_t i;
	for (i = 0; i < num; ++i)
	{
		int random = rand();
		long longrand = (( (long) random ) << 16) + rand();
		arr[i] = longrand;
	}
	return arr;
}

double* random_double_array(size_t size) 
{
	time_t t;
	srand((unsigned) time(&t));
	double* arr = (double*) calloc(size, sizeof(double));
	size_t i;
	for (i = 0; i < size; ++i)
	{
		int random = rand() % 25;
		arr[i] = random + (random/(double)(1 + (rand() % 100)));
	}
	return arr;
}

float* random_float_array(size_t size) 
{
	return (float*) random_double_array(size);
}

Point* random_point_array(size_t size)
{
	Point* arr = (Point*) calloc(size, sizeof(Point));
	Point* p = calloc(1, sizeof(Point));
	time_t t;
	srand((unsigned) time(&t));
	size_t i;
	for (i = 0; i < size; ++i) 
	{
		int random = rand() % 100000;
		p->x = random + (random/(double)(1 + (rand() % 100)));
		p->y = random + (random/(double)(1 + (rand() % 100)));
		arr[i] = *p; 
	}
	free(p);
	return arr;
}