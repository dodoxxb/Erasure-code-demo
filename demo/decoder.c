#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "jerasure.h"	
#include "galois.h"
#include "cauchy.h"

#define BLCOK_NAME_LENGTH 50

int main(int argc, char const *argv[])
{
	int k, m, w, size;              //parameters
 	int i, j;

	int *matrix;       
	char **data, **coding;
	char *fname;
	char *curdir;                   // currnet address
	int *erasures;

	FILE *fp, *fp2;

	clock_t beg, end;
    beg = clock();

	//Initialize the encoding parameters
	k = 8;
	m = 3;
	w = 8;
	size = 1024*1024;
	
	//get current address
	curdir = (char *)malloc(sizeof(char) * 1000);
	bzero(curdir, strlen(curdir));
	getcwd(curdir, 1000);
	fname = (char *)malloc(sizeof(char) * (BLCOK_NAME_LENGTH + strlen(curdir) + 10));
	
	matrix = (int *)malloc(m * k * sizeof(int));
	erasures = (int *)malloc((m+1) * sizeof(int));
	data = (char **)malloc(sizeof(char*) * k); 
	coding = (char **)malloc(sizeof(char*) * m);

	//Read blocks and prepare to decode
	//srand48(0);
  	for (i = 1; i <= k; i++){
  		bzero(fname, strlen(fname));
  		sprintf(fname, "%s/Coding/block_k%d", curdir, i);
  		fp = fopen(fname, "rb");
    	data[i-1] = (char *)malloc(sizeof(char) * size);
    	fread(data[i-1], sizeof(char), size, fp);
 		fclose(fp);
 	}
  	 
  	for (i = 1; i <= m; i++){ 
  		bzero(fname, strlen(fname)); 
  		sprintf(fname, "%s/Coding/block_n%d", curdir, i); 
  		fp = fopen(fname, "rb"); 
  		coding[i-1] = (char *)malloc(sizeof(char) * size); 
  		fread(coding[i-1], sizeof(char), size, fp); 
  		fclose(fp);
  	}	

  	//Mark failed block
  	srand48((int)time(0));
  	for (i = 0; i < m; i++){
	    erasures[i] = lrand48()%(k+m);
	    //erasures[i] = i+2;
	    printf("%d ", erasures[i]);
	    bzero((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], size);
 	}
 	printf("\n");
	erasures[i] = -1;

	matrix = cauchy_original_coding_matrix(k, m, w);
	jerasure_print_matrix(matrix, m, k, w);
	printf("\n");

	//sbitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);
	//i = jerasure_schedule_decode_lazy(k2, m, w, bitmatrix, erasures, data, coding, size, 1024, 1);
	i = jerasure_matrix_decode(k, m, w, matrix, 0, erasures, data, coding, size);

	for(i = 0; i < m; i++){
		if(erasures[i] < k){
			bzero(fname, strlen(fname));
	  		sprintf(fname, "%s/Coding/block_K%d", curdir, erasures[i]+1);
			fp2 = fopen(fname, "wb");
			fwrite(data[erasures[i]], sizeof(char), size, fp2);
			fclose(fp2);
		}
		else{
			bzero(fname, strlen(fname));
	  		sprintf(fname, "%s/Coding/block_M%d", curdir, erasures[i]-k+1);
			fp2 = fopen(fname, "wb");
			fwrite(coding[erasures[i]-k], sizeof(char), size, fp2);
			fclose(fp2);
		}
	}

	end = clock();
    printf("Time used=%.5f\n",  (double)(end-beg) / CLOCKS_PER_SEC);


	return 0;
}
