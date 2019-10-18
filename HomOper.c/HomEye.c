void HomEye(LweSample* res, const LweSample* a[10][10], const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* a[N][N];

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
		a[i][j] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		}
	}
		 
 	
	for (int i=0;i<N;i++)
	 	{
	 	for (int j=0;j<N;j++)
	 		{
	 		if (i==j)
	 		{
 				bootsCOPY(&a[i][j],1,bk);
	 			printf("%d",a[i][j]);
	 		}
	 		else
	 		{
	 			bootsCOPY(&a[i][j],0, bk);
	 			printf("%d",a[i][j]);
	 		}
	      	}
	      	printf("\n");
     	}
    bootsCOPY(&res[0][0], &a[N][N], bk);

	delete_gate_bootstrapping_ciphertext_array(length, a[N][N]);
}     
