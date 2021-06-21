#include <stdio.h>
#include <math.h>
#include <PlainKmeans.c>


int main(int argc, char const *argv[])
{
	int length = 10;
	int res_length = 10;
	int res_number = 3;
	int N = 64;
	int k = 3;

    int plaintext1[N];
    int plaintext2[k];


    FILE* data = fopen("Kmeansdata.txt","r");
    for(int i = 0; i < N; i++)
    {
    	fscanf(data, "%d ", &plaintext1[i]);
    }
    fclose(data);

    FILE* cluster = fopen("Kmeanscluster.txt","r");
    for(int i = 0; i < k; i++)
    {
    	fscanf(cluster, "%d ", &plaintext2[i]);
    }
    fclose(cluster);




    int result[3];


    /////////////////////////////////////////////////////////////

	PlainKmeans(result, plaintext1, plaintext2, length, N, k);

	/////////////////////////////////////////////////////////////

	for(int i = 0; i < res_number; i++)
	{
		printf("%d \n", result[i]);
	}

    FILE* KmeansPlainResult = fopen("KmeansPlainResult.txt","w");
    fprintf(KmeansPlainResult, "%d %d %d \n", result[0], result[1], result[2]);
    fclose(KmeansPlainResult);
    
	return 0;
}