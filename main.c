#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>


static const long Num_To_Sort = 10000000;

void swap(int *xp, int *yp)
{
int temp = *xp;
*xp = *yp;
*yp = temp;
}

// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
int partition (int arr[], int l, int h)
{
int pivot = arr[h];
int i = (l - 1);

for (int j = l; j <= h - 1; j++)
{
// If current element is smaller than the pivot
if (arr[j] < pivot)
{
i++; // increment index of smaller element
swap(&arr[i], &arr[j]);
}
}
swap(&arr[i + 1], &arr[h]);
return (i + 1);
}

void sort_s(int *arr,int l, int h) {
if (l < h)
{
/* pit is partitioning index, arr[p] is now
at right place */
int pit = partition(arr, l, h);

// Separately sort elements before
// partition and after partition
sort_s(arr, l, pit - 1);
sort_s(arr, pit + 1, h);
}
}

// Parallel version of your sort
void sort_p(int *arr,int l,int h) {
int n=sizeof(arr);
long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
{
int p = omp_get_thread_num();
unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
long chunk_start = p * chunk_size;
long chunk_end = chunk_start + chunk_size;
if (l < h)
{
/* pit is partitioning index, arr[p] is now
at right place */
int pit = partition(arr, l, h);

// Separately sort elements before
// partition and after partition
sort_p(arr, l, pit - 1);
sort_p(arr, pit + 1, h);
}
}



}

int main()
{
int *arr_s = malloc(sizeof(int) * Num_To_Sort);
long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
{
int p = omp_get_thread_num();
unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
long chunk_start = p * chunk_size;
long chunk_end = chunk_start + chunk_size;
for (long i = chunk_start; i < chunk_end; i++) {
arr_s[i] = rand_r(&seed);
}
}

// Copy the array so that the sorting function can operate on it directly.
// Note that this doubles the memory usage.
// You may wish to test with slightly smaller arrays if you're running out of memory.
int *arr_p = malloc(sizeof(int) * Num_To_Sort);
memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

struct timeval start, end;

printf("Timing sequential...\n");
gettimeofday(&start, NULL);
sort_s(arr_s,0,sizeof(arr_s)-1);
gettimeofday(&end, NULL);
printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

free(arr_s);

printf("Timing parallel...\n");
gettimeofday(&start, NULL);
sort_p(arr_p,0,sizeof(arr_p));
gettimeofday(&end, NULL);
printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

free(arr_p);

return 0;
}
