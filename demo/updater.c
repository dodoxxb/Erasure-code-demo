#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#include "jerasure.h"	
#include "galois.h"
#include "cauchy.h"

#define BLCOK_NAME_LENGTH 50

int main(int argc, char const *argv[])
{
  	int k1, k2, m, w, s, size;     //k1 denotes before scaling, k2 denotes after scaling and others are parameters of encoding matrix
  	int i, j, index; 
  	//int packetsize;
  	int *matrix; 				   //encoding matrix
  	int *sub_matrix; 			   //encoding matrix

  	char **data, **coding;         //data block and parity block
  	char **new_data;               // new data block 

  	char *fname;                   // the file name of block
  	char *parity_after;            //parity block before updating and after updating
  	char *parity_temp;             // temporary parity
  	char *delta_parity;            // delta parity block to update

  	char *curdir; // currnet address
 	FILE *fp, *fp2; // file pointers
  	
  	k1 = 4;
	k2 = 6;
	m = 3;
	w = 8;
	size = 1024*1024;
	
	s = k2 - k1;
	
	//Allocate space
	matrix = (int *)malloc(m * k2 * sizeof(int));
	sub_matrix = (int *)malloc(m * s * sizeof(int));

    parity_after = (char *) malloc(sizeof(char) * size);
    parity_temp = (char *) malloc(sizeof(char) * size);
    delta_parity = (char *)malloc(sizeof(char) * size);

    new_data = (char **)malloc(sizeof(char*) * s);
	data = (char **)malloc(sizeof(char*) * k1);
	coding = (char **)malloc(sizeof(char*) * m);

	curdir = (char *)malloc(sizeof(char) * 1000);
	bzero(curdir, strlen(curdir));
	getcwd(curdir, 1000);
	fname = (char *)malloc(sizeof(char) * (BLCOK_NAME_LENGTH + strlen(curdir) + 10));

	//Read blocks 
	for (i = 1; i <= k1; ++i){	
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_k%d", curdir, i);
		fp =fopen(fname, "rb");
		data[i-1] = (char *)malloc(sizeof(char) * size);
		fread(data[i-1], sizeof(char), size, fp);
		fclose(fp);
	}

	for (i = 1; i <= m; ++i){
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_m%d", curdir, i);
		fp = fopen(fname, "rb");
		coding[i-1] = (char *)malloc(sizeof(char) * size);
		fread(coding[i-1], sizeof(char), size, fp);
		fclose(fp);
	}

	for(i = 1; i <= s; i++){
		bzero(fname, strlen(fname));
		sprintf(fname, "%s/Coding/block_k%d", curdir, i+4);
		//printf("%d\n", i);
		fp =fopen(fname, "rb");
		new_data[i-1] = (char *)malloc(sizeof(char) * size);
		fread(new_data[i-1], sizeof(char), size, fp);
		fclose(fp);
	}	
	//printf("%s\n",str);

	//Generating encoding cauchy matrix
	matrix = cauchy_original_coding_matrix(k2, m, w);
	jerasure_print_matrix(matrix, m, k2, w);
	printf("\n");
	//printf("%d\n", matrix[k2-1]);

	index = 0;
	for(i = 0; i < m; i++){
		for(j = 0; j < s; j++){
			sub_matrix[index] = matrix[(i+1)*k2-s+j];
			index++;
		}
	}
	jerasure_print_matrix(sub_matrix, m, s, w);
	printf("\n");

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


	return 0;
}
