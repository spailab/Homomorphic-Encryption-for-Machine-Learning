# Exponential Curve Fit
void HomExpCurFit(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* sumx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumxy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumx2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* b = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* Y = new_gate_bootstrapping_ciphertext_array(length, bk->params);	

////initialization of a0, a1 and b: y = a0 * exp(a1 * x), a0 = exp(b);
	//a0
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a0[i], 0, bk);
	}
	

	//a1
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a1[i], 0, bk);
	}

	//Y
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&Y[i], 0, bk);
	}

	//b
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&b[i], 0, bk);
	}


	LweSample *M1[N], *M2[N];
	for(int i=0;i<N-1;i++)
	{
		M1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		M2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}

	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	
	LweSample* A3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	
	LweSample* A5 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A6 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	HomRealP2C(EN, N, length, bk);

	// for(int i=0;i<length;i++)
	// {
	// 	bootsCONSTANT(&A1[i], 0, bk);
	// 	bootsCONSTANT(&A2[i], 0, bk);
	// 	bootsCONSTANT(&A3[i], 0, bk);
	// 	bootsCONSTANT(&A4[i], 0, bk);
	// 	bootsCONSTANT(&A5[i], 0, bk);
	// 	bootsCONSTANT(&A6[i], 0, bk);
	// 	bootsCONSTANT(&B1[i], 0, bk);
	// 	bootsCONSTANT(&B2[i], 0, bk);
	// 	bootsCONSTANT(&B3[i], 0, bk);
	// }


//Calculation Start///
	for (int i =0;i<N-1;i++)
	{
		HomLog16(Y, y[i], length, bk);
	}


	for(int i =0;i<N-1;i++)
	{
		HomAdd(sumx, sumx, x[i], length, bk);
		HomMulti(M1[i],x[i], x[i], length,bk);
		HomAdd(sumx2, sumx2, M1[i], length, bk);
		HomAdd(sumy, sumy, Y, length, bk);
		HomMulti(M2[i], x[i], Y, length, bk);
		HomAdd(sumxy, sumxy, M2[i], length, bk);		
	}	
	
	HomMulti(A1, sumx2, sumy, length, bk);
	HomMulti(A2, sumx, sumxy, length, bk);
	HomMulti(A3, EN, sumx2, length, bk);
	HomMulti(A4, sumx, sumx, length, bk);
	HomMulti(A5, EN, sumxy, length, bk);
	HomMulti(A6, sumx, sumy, length, bk);
	HomSubt(B1, A1, A2, length, bk);
	HomSubt(B2, A3, A4, length, bk);
	HomSubt(B3, A5, A6, length, bk);
	HomDiv(b, A, B1, B2, length, bk);
	HomDiv(a1, A, B3, B2, length, bk);
	HomBinExpFinal(a0,b,length,bk);
	
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[0][i], &a0[i], bk);
	}

	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[1][i], &a1[i], bk);
	}


	delete_gate_bootstrapping_ciphertext_array(length, sumx);
	delete_gate_bootstrapping_ciphertext_array(length, sumy);
	delete_gate_bootstrapping_ciphertext_array(length, sumxy);
	delete_gate_bootstrapping_ciphertext_array(length, sumx2);
	delete_gate_bootstrapping_ciphertext_array(length, a0);
	delete_gate_bootstrapping_ciphertext_array(length, a1);
	delete_gate_bootstrapping_ciphertext_array(length, b);
	delete_gate_bootstrapping_ciphertext_array(length, B);
	delete_gate_bootstrapping_ciphertext_array(length, Y);
	
	for(int i=0;i<N-1;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, M1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, M2[i]);	
	}	

	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, A1);
	delete_gate_bootstrapping_ciphertext_array(length, A2);
	delete_gate_bootstrapping_ciphertext_array(length, A3);
	delete_gate_bootstrapping_ciphertext_array(length, A4);
	delete_gate_bootstrapping_ciphertext_array(length, A5);
	delete_gate_bootstrapping_ciphertext_array(length, A6);
	delete_gate_bootstrapping_ciphertext_array(length, B1);
	delete_gate_bootstrapping_ciphertext_array(length, B2);
	delete_gate_bootstrapping_ciphertext_array(length, B3);
	delete_gate_bootstrapping_ciphertext_array(length, EN);
}
