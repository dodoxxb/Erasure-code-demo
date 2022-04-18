#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "jerasure.h"	
#include "galois.h"
#include "cauchy.h"

#define BLCOK_NAME_LENGTH 50

void get_sub_matrix(int *matrix, int *sub_matrix, int k, int m, int s)
{
	int i,j,index;
	index = 0;
	for(i = 0; i < m; i++){
		for(j = 0; j < s; j++){
			sub_matrix[index] = matrix[(i+1)*k-s+j];
			index++;
		}
	}
}

int main(int argc, char const *argv[])
{
  	int k1, k2, m, w, s, size; //k1 denotes before scaling, k2 denotes after scaling and others are parameters of encoding matrix
  	int i, j, index;           
  	int stripe;                //stripe number

  	//int packetsize;
  	int *matrix;  			   //encoding matrix
  	int *sub_matrix;           //encoding matrix
  	int *decode_matrix;        //deciding matrix

  	char **data, **coding;     //data block and parity block
  	//char **new_data; 

  	char *fname;               // the file name of block
  	char *parity_after;        //parity block after updating
  	char *parity_temp;         // temporary parity
  	char *delta_parity;        // delta parity block to update

  	char *curdir; // currnet address
 	FILE *fp, *fp2; // file pointers

 	int *erasures;
  	
  	clock_t beg, end;
    beg = clock();
  	
  	k1 = 4;
	k2 = 8;
	m = 3;
	w = 8;
	stripe = 2;
	size = 1024*1024;
	
	s = k2 - k1;
	
	//Allocate space
	matrix = (int *)malloc(m * k2 * sizeof(int));
	sub_matrix = (int *)malloc(m * s * sizeof(int));
	decode_matrix = (int *)malloc(m * k1 * sizeof(int));
	erasures = (int *)malloc((m+1) * sizeof(int));

    parity_after = (char *) malloc(sizeof(char) * size);
    parity_temp = (char *) malloc(sizeof(char) * size);
    delta_parity = (char *)malloc(sizeof(char) * size);

    //new_data = (char **)malloc(sizeof(char*) * s);
	data = (char **)malloc(sizeof(char*) * k1);
	coding = (char **)malloc(sizeof(char*) * m * stripe);

	curdir = (char *)malloc(sizeof(char) * 1000);
	bzero(curdir, strlen(curdir));
	getcwd(curdir, 1000);
	fname = (char *)malloc(sizeof(char) * (BLCOK_NAME_LENGTH + strlen(curdir) + 10));
	
	//Allocate space and read blocks
	for (i = 1; i <= k1; ++i){	
        data[i-1] = (char *)malloc(sizeof(char) * size);
	}
	
	for(i = 1; i <= (k1-m); i++){
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_k%d", curdir, i+k1+m);
		fp =fopen(fname, "rb");
		fread(data[i+m-1], sizeof(char), size, fp);
		fclose(fp);
	}

	for (i = 1; i <= m; ++i){
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_m%d", curdir, i);
		fp = fopen(fname, "rb");
		coding[i-1] = (char *)malloc(sizeof(char) * size);
		fread(coding[i-1], sizeof(char), size, fp);
		fclose(fp);

		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_m%d", curdir, i+m);
		fp = fopen(fname, "rb");
		coding[i+m-1] = (char *)malloc(sizeof(char) * size);
		fread(coding[i+m-1], sizeof(char), size, fp);
		fclose(fp);
	}
	//printf("%s\n",str);

	//Generating encoding cauchy matrix
	matrix = cauchy_original_coding_matrix(k2, m, w);
	jerasure_print_matrix(matrix, m, k2, w);
	printf("\n");
	//printf("%d\n", matrix[k2-1]);

	//Get the submatrix of encoding matrix
	get_sub_matrix(matrix, sub_matrix, k2, m, s);
	jerasure_print_matrix(sub_matrix, m, k1, w);
	printf("\n");

	decode_matrix = cauchy_original_coding_matrix(k1, m, w);
	jerasure_print_matrix(matrix, m, k1, w);
	printf("\n");

	//Mark the busy data block as failed block

	//srand48((int)time(0));
	for (i = 0; i < m; i++){
	    //erasures[i] = lrand48()%(k2+m);
	    erasures[i] = i;
	    //printf("%d ", erasures[i]);
	    //bzero((erasures[i] < k) ? data[erasures[i]] : coding[erasures[i]-k], size);
 	}
 	printf("\n");
	erasures[i] = -1;

	//Decode to get busy data block
	i = jerasure_matrix_decode(k1, m, w, decode_matrix, 0, erasures, data, coding+m, size);

	/*
	for(i = 0; i < m; i++){
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_nk%d", curdir, i+1);
		fp =fopen(fname, "wb");
		fwrite(data[i], sizeof(char), size, fp);
		fclose(fp);
	}
	*/
	
	//Updating the parity block
	for (i = 0; i < m; i++){
		//printf("start\n");
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_n%d", curdir, i+1);
		fp2 = fopen(fname,"wb");
		//bzero(delta_parity, strlen(delta_parity));
		bzero(parity_after, size);
		bzero(parity_temp, size);

		for (j = 0; j < s; j++){
			galois_w08_region_multiply((char *)data[j], sub_matrix[i*s+j], size, (char *)parity_temp, 1);
			//printf("%d\n",sub_matrix[i*s+j]);
			//printf("%s\n",new_data[j]);
			//galois_region_xor((char *)parity_temp, (char *)delta_parity, (char *)delta_parity, size);
			//printf("%s\n",delta_parity);
		}

		galois_region_xor((char *)parity_temp, (char *)coding[i], (char *)parity_after, size);	
		fwrite(parity_after, sizeof(char), size, fp2);
		fclose(fp2);
	}
	//fclose(fp);
	//printf("%s\n", parity_after);

	end = clock();
    printf("Time used=%.5f\n",  (double)(end-beg) / CLOCKS_PER_SEC);

	return 0;
}