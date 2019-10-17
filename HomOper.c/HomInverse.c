void HomInverse(LweSample* res[10][10], LweSample* x[10][10], const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* ratio = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* ratio2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* a = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	
 	printf("00");
 	for (int i = 0; i < N; i++){
 		for (int j = N; j <2*N;j++){
 			if (i==(j-N))
 				for(int s = 0; s < length; s++){
 					for(int t = 0; t < length; t++){
		 				bootsCONSTANT(&x[s][t], 0, bk);
		 			}
 						bootsCONSTANT(&x[(length/2)-1][(length/2)-1], 1, bk);
 				}		
 				//x[i][j] = 1;
 			else
 				for(int s = 0; s < length; s++){
 					for(int t = 0; t < length; t++){
		 				bootsCONSTANT(&x[s][t], 0, bk);
		 			}
 				}
 				//x[i][j] = 0;
 		}
 	}

 	printf("1");

 	for (int i=0; i<N; i++){
 		for(int j=0;j<N;j++){
 			if(i!=j){
 			HomDiv(ratio, A, x[j][i], x[i][i],length, bk);
 			printf("11");
 			for (int k=0;k<2*N;k++){
 				HomMultiReal(ratio2, ratio, x[i][k], length, bk);
 				HomSubt(x[j][k], x[j][k], ratio2, length, bk);
 				printf("22");
 				}
 			}
 		}
	}
	printf("2");
	
	for(int i=0;i<N;i++){
		bootsCOPY(&a, &x[i][i], bk);
		for (int j=0;j<2*N;j++){
		HomDiv(x[i][j], A, x[i][j], a, length, bk);
		}
	}	

	for(int i=0;i<length;i++)
	{
		for(int j=N;j<2*length;j++)
		{
		bootsCOPY(&res[i][j], &x[i][j], bk);	
		}
	}

	delete_gate_bootstrapping_ciphertext_array(length, ratio);
 	delete_gate_bootstrapping_ciphertext_array(length, ratio2);
 	delete_gate_bootstrapping_ciphertext_array(length, a);
 	delete_gate_bootstrapping_ciphertext_array(length, A);
 }	
