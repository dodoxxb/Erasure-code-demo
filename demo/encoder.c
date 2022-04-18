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
    FILE *fp, *fp2;                 //file pointer

    int k, m, w, packetsize;        // parameters
    int i, j;                       // loop control variables
    int blocksize;                  // size of k+m files
    int stripe;

    char **data, **coding;          //data block and parity block   
    //char **data_scaling, **coding_scaling;
    int *matrix;                    //coding cauchy matrix
    //int *bitmatrix;
    //int **schedule;

    char *fname;
    char *curdir;
    
    clock_t beg, end;
    beg = clock();

    matrix = NULL;
    //bitmatrix = NULL;
    //schedule = NULL;

    //Initialize the encoding parameters
    k = 4;
    m = 3;
    w = 8;
    stripe = 2;
    
    blocksize = 1024*1024;
    packetsize = 1024;

    //get current address
    curdir = (char*)malloc(sizeof(char)*1000);  
    bzero(curdir, strlen(curdir));
    getcwd(curdir, 1000);
    fname = (char *)malloc(sizeof(char) * (BLCOK_NAME_LENGTH + strlen(curdir) + 10));

    //Allocate space
    matrix = (int *)malloc(m * k * sizeof(int));
    data = (char **)malloc(sizeof(char*) * k * stripe); 
    coding = (char **)malloc(sizeof(char*) * m * stripe);
    //data_scaling = (char **)malloc(sizeof(char*) * k); 
    //coding_scaling = (char **)malloc(sizeof(char*) * m);

    //Read data block
    for (i = 1; i <= k; i++){
        bzero(fname, strlen(fname));
        sprintf(fname, "%s/Coding/block_k%d", curdir, i);
        fp = fopen(fname, "rb");
        data[i-1] = (char *)malloc(sizeof(char) * blocksize);
        fread(data[i-1], sizeof(char), blocksize, fp);
        fclose(fp);

        bzero(fname, strlen(fname));    
        sprintf(fname, "%s/Coding/block_k%d", curdir, i+k);
        fp = fopen(fname, "rb");
        data[i+k-1] = (char *)malloc(sizeof(char) * blocksize);
        fread(data[i+k-1], sizeof(char), blocksize, fp);
        fclose(fp);
       
        /*
        bzero(fname, strlen(fname));
        sprintf(fname, "%s/Coding/block_k%d", curdir, i+k);
        fp = fopen(fname, "rb");
        data_scaling[i-1] = (char *)malloc(sizeof(char) * blocksize);
        fread(data_scaling[i-1], sizeof(char), blocksize, fp);
        fclose(fp);
        */
    }

    //Allocate parity block space
    for (i = 0; i < m; i++) {
        coding[i] = (char *)malloc(sizeof(char)*blocksize);
        coding[i+m] = (char *)malloc(sizeof(char)*blocksize);
    }

    //Generate coding matrix
    matrix = cauchy_original_coding_matrix(k, m, w);
    jerasure_print_matrix(matrix, m, k, w);
    printf("\n");
    //bitmatrix = jerasure_matrix_to_bitmatrix(k, m, w, matrix);
    //schedule = jerasure_smart_bitmatrix_to_schedule(k, m, w, bitmatrix);
    
    /*
    jerasure_schedule_encode(k, m, w, schedule, data, coding, blocksize, packetsize);

    for (i = 0; i < m; i++) {
        bzero(fname, strlen(fname));
        sprintf(fname, "%s/Coding/block_s_m%d", curdir, i+1);
        fp = fopen(fname, "wb");
        fwrite(coding[i], sizeof(char), blocksize, fp);
        fclose(fp);
    }

    for (i = 0; i < m; i++) {
        bzero(coding[i], strlen(coding[i]));
    }

    jerasure_bitmatrix_encode(k, m, w, bitmatrix, data, coding, blocksize, packetsize);

    for (i = 0; i < m; i++) {
        bzero(fname, strlen(fname));
        
        sprintf(fname, "%s/Coding/block_b_m%d", curdir, i+1);
        fp = fopen(fname, "wb");
        fwrite(coding[i], sizeof(char), blocksize, fp);
        fclose(fp);
    }

    for (i = 0; i < m; i++) {
        bzero(coding[i], strlen(coding[i]));
    }*/

    //Execute encode operation
    for (i = 0; i < stripe; i++){
         jerasure_matrix_encode(k, m, w, matrix, data+k*i, coding+m*i, blocksize);
         
    }
    
    //Save parity block file
    for (i = 0; i < m; i++) {
        bzero(fname, strlen(fname));
        sprintf(fname, "%s/Coding/block_m%d", curdir, i+1);
        fp = fopen(fname, "wb");
        fwrite(coding[i], sizeof(char), blocksize, fp);
        fclose(fp);

        bzero(fname, strlen(fname));
        sprintf(fname, "%s/Coding/block_m%d", curdir, i+m+1);
        fp = fopen(fname, "wb");
        fwrite(coding[i+m], sizeof(char), blocksize, fp);
        fclose(fp);
    }

    end = clock();
    printf("Time used=%.5f\n",  (double)(end-beg) / CLOCKS_PER_SEC);

    return 0;
}
