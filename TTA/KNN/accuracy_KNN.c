#include <stdio.h>
#include <math.h>



int main(int argc, char const *argv[])
{

	int loop = 10;
	int res_number = loop;
	
	int Res_P[res_number], Res_C[res_number];

	FILE* KNNPlainResult = fopen("KNNPlainResult.txt","r");
	for(int i = 0; i < res_number; i++){
    	fscanf(KNNPlainResult, "%d ", &Res_P[i]);
	}
    fclose(KNNPlainResult);

    FILE* KNNCipherResult = fopen("KNNCipherResult.txt","r");
    for(int i = 0; i < res_number; i++){
    	fscanf(KNNPlainResult, "%d ", &Res_C[i]);
	}
    fclose(KNNCipherResult);	

	printf(" Plain label : ");
	for(int i = 0; i < loop; i++){
		printf("%d ", Res_P[i]);}
	printf("\n");

	printf("Cipher label : ");
	for(int i = 0; i < loop; i++){
		printf("%d ", Res_C[i]);}
	printf("\n");

    /////////////////////////////////////////////////////////////
    double accuracy = 0;
    int temp1 = 0;
    
	for(int i = 0; i < res_number; i++)
	{
		temp1 += fabs(Res_P[i] - Res_C[i]);
	}

	accuracy = (1 - ((double) temp1 / (double) res_number))*100;



	/////////////////////////////////////////////////////////////


	printf("Accuracy of Kmeans algorithm : %f %% \n", accuracy);
    
	return 0;
}
