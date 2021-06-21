 void swap(int* a, int* b){
 	int temp = *a;
 	*a = *b;
 	*b = temp;
 }

 void PlainKNN(int res[1], int *a, int *l, int *b, const int length, const int label_length, const int N, const int d, const int k){


	int A[N*d];
 	int B[N*d];
 	int C[N], L[N];
 	int D, E, temp;


	///// copy data /////
 	for(int i = 0; i < N*d; i++)
	{
 		A[i] = a[i];
 	}


 	// printf("A = ");
 	// for(int i = 0; i < N*d; i++){
 	// 	printf("%d ", A[i]);
 	// }
 	// printf(" \n");


 	for(int i = 0; i < N; i++)
	{
 		L[i] = l[i];
 	}


 	// printf("L = ");
 	// for(int i = 0; i < N; i++){
 	// 	printf("%d ", L[i]);
 	// }
 	// printf(" \n");


 	for(int i = 0; i < N; i++)
 	{
 		C[i] = 0;
 	}
 	
	////// sum(abs(data - New data)) /////
 	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < d; j++)
		{
			B[i*d+j] =  abs(A[i*d+j] - b[j]);
			C[i] += B[i*d+j];
		}
 	}


	// printf("b = ");
	// for(int i = 0; i < d; i++){
	// 	printf("%d ", b[i]);
	// }
	// printf(" \n");

	// printf("B = ");
	// for(int i = 0; i < N*d; i++){
	// 	printf("%d ", B[i]);
	// }
	// printf(" \n");

	// printf("C = ");
	// for(int i = 0; i < N; i++){
	// 	printf("%d ", C[i]);
	// }
	// printf(" \n");
 	




	// find k small distance //
	for(int i = 0; i < k; i ++)
	{
		for(int j = i+1; j < N; j++)
		{
			if (C[i] > C[j])
			{
				swap(&C[i], &C[j]);
				swap(&L[i], &L[j]);

			}
		}
	}

	// printf("C = ");
	// for(int i = 0; i < N; i++){
	// 	printf("%d ", C[i]);
	// }
	// printf(" \n");
	// printf("L = ");
	// for(int i = 0; i < N; i++){
	// 	printf("%d ", L[i]);
	// }
	// printf(" \n");



	// find mode of sorted label //
	int freq, count = 1;

	for(int i = 0; i < k; i++)
	{
		freq = 1;
		for(int j = i+1; j < k; j++)
		{
			if(L[i] == L[j]){
				freq += 1;
			}
		}
		if(freq >= count)
		{
			res[0] = L[i];
			count = freq;
		}
	}
	// printf("loop clear /////////////// %d \n", res[0]);
 	return res[0];
}

