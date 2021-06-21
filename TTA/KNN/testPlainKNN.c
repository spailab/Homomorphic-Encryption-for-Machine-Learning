#include <stdio.h>
#include <math.h>
#include <PlainKNN.c>


int main(int argc, char const *argv[])
{
	int length = 10;
	int N = 32;
	int k = 3;
	int d = 2;
    int result_length = 1;
    int loop = 10;

    int plaintext1[N*d];
    int plaintext2[N];
    int plaintext3[loop*d];

    FILE* data = fopen("KNNdata.txt","r");
    for(int i = 0; i < N*d; i++)
    {
    	fscanf(data, "%d ", &plaintext1[i]);
    }
    fclose(data);

    FILE* label = fopen("KNNlabel.txt","r");
    for(int i = 0; i < N; i++)
    {
    	fscanf(label, "%d ", &plaintext2[i]);
    }
    fclose(label);

    FILE* newdata = fopen("KNNnewdata.txt","r");
    for(int i = 0; i < loop*d; i++)
    {
    	fscanf(newdata, "%d ", &plaintext3[i]);
    }
    fclose(newdata);


    int result[loop][1];
    int ndata[d];


    /////////////////////////////////////////////////////////////
    for(int i = 0; i < loop; i++){
    	for(int j = 0; j < d; j++){
    		ndata[j] = plaintext3[d*i+j];
            //printf("%d ",ndata[j]);
    	}
        
    	PlainKNN(result[i], plaintext1, plaintext2, ndata, length, result_length, N, d, k);	
    }
	
	
	/////////////////////////////////////////////////////////////

	
    FILE* KNNPlainResult = fopen("KNNPlainResult.txt","w");
    for(int i = 0; i < loop; i++){
    	fprintf(KNNPlainResult, "%d \n", result[i][0]);	
    }
    fclose(KNNPlainResult);


	for(int i = 0; i < loop; i++){
    	printf("%d ",result[i][0]);
	}	
    printf("\n ");



    
	return 0;
}