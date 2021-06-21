 void HomKNN(LweSample* res[100], LweSample* a[200], LweSample* l[100], LweSample* b[5], const int length, const int label_length, const int N, const int d, const int k, const TFheGateBootstrappingCloudKeySet* bk){



 	LweSample *A[N*d], *B[N*d], *C[N], *L[N];

	// A = data copy // 
	// B = dimensional distance with data and new_data // 
	// C = distance with data and new_data // 
	for(int i = 0; i < N*d; i++)
 	{
 		A[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 		B[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 	}

 	for(int i = 0; i < N; i++){
 		C[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
 		L[i] = new_gate_bootstrapping_ciphertext_array(label_length, bk->params);}

	// variable for labelling //
	LweSample* D = new_gate_bootstrapping_ciphertext_array(label_length + k, bk->params);
 	LweSample* E = new_gate_bootstrapping_ciphertext_array(label_length + k, bk->params);
 	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);





	///// copy data /////
	// a[N*d][length] to A[N*d][length] //
	printf("// data copy\n");
 	for(int i = 0; i < N*d; i++)
	{
 		for(int j = 0; j < length; j++)
 		{
 			bootsCOPY(&A[i][j], &a[i][j], bk);
 		}
 	}

 	for(int i = 0; i < N; i++)
	{
 		for(int j = 0; j < label_length; j++)
 		{
 			bootsCOPY(&L[i][j], &l[i][j], bk);
 		}
 	}




	printf("// data - new_data\n");
	////// data - New data /////
 	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < d; j++)
		{
			HomSubt(B[i*d+j], A[i*d+j], b[j], length, bk);
		}
 	}



	printf("// calculate distance_with absolute value\n");
	///// calculate distance /////	
 	for(int i = 0; i < N*d; i++)
	{
//		HomMulti(B[i], B[i], B[i], length, bk);
		HomAbs(B[i], B[i], length, bk);
 	}



	printf("// calculate distance sum\n");
	for(int i = 0; i < N; i++)
	{
		for(int j = 1; j < d; j++)
		{
			HomAdd(B[i*d], B[i*d], B[i*d+j], length, bk);
		}
 	}





 	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < length; j++){
			bootsCOPY(&C[i][j], &B[i*d][j], bk);
		}
 	}



	printf("// small k - sorting algorithm\n");
	// find k small distance //
	for(int i = 0; i < k; i ++)
	{
		for(int j = i + 1; j < N; j++)
		{
			HomCompB(&temp[0], C[i], C[j], length, bk);
			HomSwap(&temp[0], C[i], C[j], length, bk);
			HomSwap(&temp[0], L[i], L[j], label_length, bk);
		}
	}



	printf("// find mode of sorted label\n");
	// find mode of sorted label //
	for(int i = 0; i < label_length + k; i++)
	{
		bootsCONSTANT(&D[i], 0, bk);
		bootsCONSTANT(&E[i], 0, bk);
	}
	

	for(int i = 0; i < k; i++)
	{
		for(int j = 0; j < label_length; j++)
		{
			bootsCOPY(&D[j], &L[i][j], bk);
		}
		HomAdd(E, E, D, label_length + k, bk);
	}






 	//////////////////////////////////////////////////////////
 	int res_num = 1;
 	int res_length = 1;


 	for(int i = 0; i < res_num; i++)
 	{
 		for(int j = 0; j < res_length; j++)
			bootsCOPY(&res[i][j], &E[1], bk);
 	}
 	//////////////////////////////////////////////////////////


/*
	// result //
	bootsCOPY(&res[0], &E[1], bk);

*/
 	//clean up all pointers //
 	for(int i = 0; i < N*d; i++)
 	{
 		delete_gate_bootstrapping_ciphertext_array(length, A[i]);
 		delete_gate_bootstrapping_ciphertext_array(length, B[i]);
	}
	
 	for(int i = 0; i < N; i++){
 		delete_gate_bootstrapping_ciphertext_array(length, C[i]);
 		delete_gate_bootstrapping_ciphertext_array(label_length, L[i]);}

 	delete_gate_bootstrapping_ciphertext_array(label_length + k, D);
 	delete_gate_bootstrapping_ciphertext_array(label_length + k, E);
 	delete_gate_bootstrapping_ciphertext_array(2, temp);
 }

