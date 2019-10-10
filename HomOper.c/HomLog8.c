// log base a of b

void HomLog8(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* Setting = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* Output = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* out_sum = new_gate_bootstrapping_ciphertext_array(5, bk->params);
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);
	LweSample* p = new_gate_bootstrapping_ciphertext_array(5, bk->params);
	LweSample* S = new_gate_bootstrapping_ciphertext_array(5, bk->params);
	LweSample* sf = new_gate_bootstrapping_ciphertext_array(5, bk->params);
	LweSample* E = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* C = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* final = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* MC = new_gate_bootstrapping_ciphertext_array(2*length+1, bk->params);
	LweSample* lb = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* x= new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nMC = new_gate_bootstrapping_ciphertext_array(1, bk->params);

	//// find digit of 'b'
	// fill Setting with zeroes
	for(int j = 0; j < length; j++){
		bootsCONSTANT(&Setting[j], 0, bk);
	}

	for(int i = 0; i < length-1; i++){			//compare Setting with the b
		bootsCONSTANT(&Setting[i], 1, bk);
		HomCompLE(&Output[i], b, Setting, length, bk);		//store outcomes in Output array
		bootsCONSTANT(&Setting[i], 0, bk);
	}
	// Sum of the Output
	for(int i = 0; i < 5; i++){
		bootsCONSTANT(&out_sum[i], 0 , bk);
		bootsCONSTANT(&temp[i], 0 , bk);
	}

	// Initialize
	for(int i = 0; i < length-1; i++){
		bootsCOPY(&temp[0], &Output[i] , bk);	
		HomAdd(out_sum, out_sum, temp, 5, bk);
	}

	//// Position of b
	// make 8 : subtraction factor
	for(int i = 0; i < 5; i++){
		bootsCONSTANT(&p[i], 0 , bk);}
	
	bootsCONSTANT(&p[2], 1 , bk);
	

	// subtraction
		HomSubt(out_sum, out_sum, p, 5, bk);

	// Shift variable array : S = 3
	for(int i = 0; i < 2; i++){
		bootsCONSTANT(&S[i], 1, bk);}

	for(int i = 2; i < 5; i++){
		bootsCONSTANT(&S[i], 0, bk);}

	// Subtracting factor array : sf = 1
	for(int i = 1; i < 5; i++){
		bootsCONSTANT(&sf[i], 0, bk);} 

		bootsCONSTANT(&sf[0], 1, bk);

	// find a position of b
	for(int i = 0; i < length; i++)
		bootsCONSTANT(&C[i], 0, bk);

	for(int i = 0; i < (length/2)-1; i++){

		HomSubt(S, S, sf, 5, bk);

		HomEqui(&E[i], S, out_sum, 5, bk); 
		
		HomLShift(A, b, length, i+1, bk);

		for(int j = 0; j < length; j++)
			bootsAND(&A[j], &A[j], &E[i], bk);

		HomAdd(C, A, C, length, bk);

		}



	HomSubt(S, S, sf, 5, bk);
/*
	for(int i = 0; i < 5; i++)
		bootsCOPY(&res[i], &S[i], bk);
*/
	HomEqui(&E[2], S, out_sum, 5, bk); 

	for(int j = 0; j < length; j++)
		bootsAND(&A[j], &b[j], &E[2], bk);

	HomAdd(C, A, C, length, bk);

	for(int i = 0; i < 5; i++)
		bootsCOPY(&res[i], &S[i], bk);

	for(int i = 0; i < (length/2)-1; i++){

		HomSubt(S, S, sf, 5, bk);
		HomEqui(&E[i+3], S, out_sum, 5, bk); 
		
		HomRShift(A, b, length, i+1, bk);

		for(int j = 0; j < length; j++)
			bootsAND(&A[j], &A[j], &E[i+3], bk);

		HomAdd(C, A, C, length, bk);

	}	

	////logarithm of shifted 'b'
	// build up last array : final	
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&final[i], 0, bk);
}
	for(int j = 0; j < 3; j++){
	HomMulti(MC, C, C, length, bk);
	
	bootsCOPY(&final[2-j], &MC[4], bk);

	HomLShift(lb, C, length, 1, bk);
	for(int i = 0; i < length; i++){
		bootsAND(&x[i], &MC[4], &lb[i], bk); 
}
	// not mc[4]
	bootsNOT(&nMC[0], &MC[4], bk);
	for(int i = 0; i < length; i++){
		bootsAND(&nx[i], &nMC[0], &C[i], bk);
	}

	HomAdd(C, x, nx, length, bk);
}
	// integer part of log 2 of b
	for(int i = 0; i < 4; i++){
	bootsCOPY(&final[i+3], &out_sum[i], bk);
}
	bootsCOPY(&final[7], &out_sum[4], bk);

	// result of log base 2 of b 
	for(int i = 0; i < length; i++){
	bootsCOPY(&res[i], &final[i], bk);
}

	delete_gate_bootstrapping_ciphertext_array(length, Setting);
	delete_gate_bootstrapping_ciphertext_array(length, Output);
	delete_gate_bootstrapping_ciphertext_array(5, out_sum);
	delete_gate_bootstrapping_ciphertext_array(5, temp);
	delete_gate_bootstrapping_ciphertext_array(5, p);
	delete_gate_bootstrapping_ciphertext_array(5, S);
	delete_gate_bootstrapping_ciphertext_array(5, sf);
	delete_gate_bootstrapping_ciphertext_array(length, E);
	delete_gate_bootstrapping_ciphertext_array(length, A);	
	delete_gate_bootstrapping_ciphertext_array(length, C);
	delete_gate_bootstrapping_ciphertext_array(length, final);
	delete_gate_bootstrapping_ciphertext_array(2*length+1, MC);
	delete_gate_bootstrapping_ciphertext_array(length, lb);
	delete_gate_bootstrapping_ciphertext_array(length, x);
	delete_gate_bootstrapping_ciphertext_array(length, nx);
	delete_gate_bootstrapping_ciphertext_array(1, nMC);
}
