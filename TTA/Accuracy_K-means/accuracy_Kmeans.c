#include <stdio.h>
#include <math.h>



int main(int argc, char const *argv[])
{

	int res_number = 3;

	int Res_P[res_number], Res_C[res_number];

	FILE* KmeansPlainResult = fopen("KmeansPlainResult.txt","r");
    fscanf(KmeansPlainResult, "%d %d %d \n", &Res_P[0], &Res_P[1], &Res_P[2]);
    fclose(KmeansPlainResult);

    FILE* KmeansCipherResult = fopen("KmeansCipherResult.txt","r");
    fscanf(KmeansCipherResult, "%d %d %d \n", &Res_C[0], &Res_C[1], &Res_C[2]);
    fclose(KmeansCipherResult);	


    /////////////////////////////////////////////////////////////


    double accuracy = 0;
    double temp1 = 0;
    double temp2 = 0;
    

	for(int i = 0; i < res_number; i++)
	{
		temp1 = fabs((double)Res_P[i] - (double)Res_C[i]);
		temp2 += temp1/(double)Res_P[i];
	}

	accuracy = (1 - temp2)*100;



	/////////////////////////////////////////////////////////////

		printf(" Plain cluster mean : %d %d %d \n", Res_P[0], Res_P[1], Res_P[2]);
		printf("Cipher cluster mean : %d %d %d \n", Res_C[0], Res_C[1], Res_C[2]);
		printf("Accuracy of Kmeans algorithm : %f %% \n", accuracy);

    
	return 0;
}
