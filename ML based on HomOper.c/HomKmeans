void HomKmeans(LweSample* res[10], const LweSample* a[100], const LweSample* u[10], const int length, const int N, const int k, const TFheGateBootstrappingCloudKeySet* bk){


 	LweSample *L[N], *M[N], *U[N], *w[k-1], *ww[k-1], *v[k-1];


 	for(int i = 0; i < N; i++)
 	{
 		L[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 		M[i] = new_gate_bootstrapping_ciphertext_array(k, bk->params);
 		U[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	}



 	for(int i = 0; i < k-1; i++)
 	{
 		w[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 		ww[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 		v[i] = new_gate_bootstrapping_ciphertext_array(2, bk->params);
	}



 	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* B = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* C = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* D = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* UU = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* t = new_gate_bootstrapping_ciphertext_array(k, bk->params);
 	LweSample* E = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	LweSample* temp = new_gate_bootstrapping_ciphertext_array(3, bk->params);

 	for(int i = 0; i < N; i++)
	{
 		for(int j = 0; j < length; j++)
 		{
 			bootsCOPY(&L[i][j], &a[i][j], bk);
 		}
 	}

 	printf("start algorithm!\n");
 	int s = 0;
 	while(s < 1){
 		printf("means\n");
 		for(int i = 0; i < k-1; i++)
 		{
 			printf("%d-th means\n", i+1);
 			HomAdd(ww[i], u[i], u[i+1], length, bk);
 			printf("%d-th means\n", i+1);
 			HomRShift(w[i], ww[i], length, 1, bk);
 			printf("%d-th means\n", i+1);
 		}
 		printf("%d - calculating means of near cluster clear!\n", s+1);

 		for(int i = 0; i < N; i++)
 		{
 			for(int j = 0; j < k-1; j++)
 			{
 				HomCompB(&v[j][1], L[i], w[j], length, bk);
 				bootsNOT(&v[j][0], &v[j][1], bk);
 			}

 			bootsCONSTANT(&t[0], 1, bk);
 			for(int j = 0; j < k; j++)
 			{
 				for(int m = 0; m < k-1; m++)
 				{
 					int e;
 					if(m < j)
 						e = 1;
 					else
 						e = 0;

 					bootsAND(&t[m+1], &v[m][e], &t[m], bk);
 				}

 				M[i][j] = t[k-1];
 			}

 		}

 		printf("%d - label setting clear\n", s+1);



 		bootsCONSTANT(&E[0], 0, bk);
 		for(int i = 0; i < length-1; i++)
 			bootsAND(&E[i+1], &E[i], &E[0], bk);

 		for(int r = 0; r < k; r++)
 		{
 			for(int i = 0; i < N; i++)
 			{
 				for(int j = 0; j < length; j++)
 				{
 					bootsAND(&U[i][j], &L[i][j], &M[i][r], bk);
 				}
 			}


 			int m = 0; 
 			int NN = N;
 			while(m <  log2(N))
			{
 				for(int i = 0; i < NN/2; i++)
 				{
 					int ii = 2*i;
 					HomEqui(&temp[0], U[ii], E, length, bk);
 					HomEqui(&temp[1], U[ii+1], E, length, bk);
 					for(int j = 0; j < length; j++)
 					{
						bootsAND(&A[j], &U[ii][j], &temp[1], bk);
 						bootsAND(&B[j], &U[ii+1][j], &temp[0], bk);
 					}

 					HomAdd(C, U[ii], A, length, bk);
 					HomAdd(D, C, U[ii+1], length, bk);
 					HomAdd(UU, D, B, length, bk);
 					HomRShift(U[i], UU, length, 1, bk);
 				}
 				NN = NN/2;
 				m++;
 			}

 			bootsCONSTANT(&temp[2], 1, bk);

 			for(int i = 0; i < length; i++)
 			{ 
 				bootsAND(&u[r][i], &U[0][i], &temp[2], bk);
 			}

 			printf("%d - %d - cluster fixed\n", s+1, r+1);

 		}
 		s++;

 		printf("%d - loop clear\n", s);
 	}


 	for(int i = 0; i < k; i++)
 	{
 		for(int j = 0; j < length; j++)
 			res[i][j] = u[i][j];
 	}

 	for(int i = 0; i < N; i++)
 	{
 		delete_gate_bootstrapping_ciphertext_array(length, L[i]);
 		delete_gate_bootstrapping_ciphertext_array(k, M[i]);
 		delete_gate_bootstrapping_ciphertext_array(length, U[i]);
 	}

 	for(int i = 0; i < k-1; i++)
 	{
 		delete_gate_bootstrapping_ciphertext_array(length, w[i]);
 		delete_gate_bootstrapping_ciphertext_array(length, ww[i]);
 		delete_gate_bootstrapping_ciphertext_array(2, v[i]);
 	}

 	delete_gate_bootstrapping_ciphertext_array(length, A);
 	delete_gate_bootstrapping_ciphertext_array(length, B);
 	delete_gate_bootstrapping_ciphertext_array(length, C);
 	delete_gate_bootstrapping_ciphertext_array(length, D);
 	delete_gate_bootstrapping_ciphertext_array(length, UU);
 	delete_gate_bootstrapping_ciphertext_array(k, t);
 	delete_gate_bootstrapping_ciphertext_array(length, E);
 	delete_gate_bootstrapping_ciphertext_array(3, temp);

 }
