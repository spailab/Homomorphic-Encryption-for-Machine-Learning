# Normal Equation
void HomLinRegNE(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* sumx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumxy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumx2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	

////initialization of intercept(a0) and slope(a1)
	//a0 is an intercept
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a0[i], 0, bk);
		bootsCONSTANT(&sumx[i], 0, bk);
		bootsCONSTANT(&sumy[i], 0, bk);
		bootsCONSTANT(&sumxy[i], 0, bk);
		bootsCONSTANT(&sumx2[i], 0, bk);
	}
	

	//a1 is a slope 
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a1[i], 0, bk);
	}
	

	LweSample *M1[N], *M2[N];
	for(int i=0;i<N;i++)
	{
		M1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		M2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}

	// LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
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
	
	for(int i=0; i<length; i++){
		bootsCONSTANT(&A1[i], 0, bk);
		bootsCONSTANT(&A2[i], 0, bk);
		bootsCONSTANT(&A3[i], 0, bk);
		bootsCONSTANT(&A4[i], 0, bk);
		bootsCONSTANT(&A5[i], 0, bk);
		bootsCONSTANT(&A6[i], 0, bk);
		bootsCONSTANT(&B1[i], 0, bk);
		bootsCONSTANT(&B2[i], 0, bk);
		bootsCONSTANT(&B3[i], 0, bk);
		bootsCONSTANT(&EN[i], 0, bk);
	}

	HomRealP2C(EN, N, length, bk);

// //Calculation Start///
	for(int i =0;i<N;i++)
	{
		HomAdd(sumx, sumx, x[i], length, bk); //sumx = sumx + x(i)
		HomMultiReal(M1[i],x[i], x[i], length,bk); //x(i)*x(i)
		HomAdd(sumx2, sumx2, M1[i], length, bk); //sumx^2 = sumx^2+x(i)*x(i)
		HomAdd(sumy, sumy, y[i], length, bk); //sumy = sumy+y(i)
		HomMultiReal(M2[i], x[i], y[i], length, bk); //x(i)*y(i)
		HomAdd(sumxy, sumxy, M2[i], length, bk); //sumxy = sumxy+x(i)*y(i)		
	}	
	

	HomMultiReal(A1, sumx2, sumy, length, bk); //sumx^2*sumy
	HomMultiReal(A2, sumx, sumxy, length, bk); //sumx*sumxy
	HomMultiReal(A3, EN, sumx2, length, bk); //N*sumx^2
	HomMultiReal(A4, sumx, sumx, length, bk); //sumx*sumx
	HomMultiReal(A5, EN, sumxy, length, bk); //N*sumxy
	HomMultiReal(A6, sumx, sumy, length, bk); //sumx*sumy
	HomSubt(B1, A1, A2, length, bk); //sumx^2*sumy-sumx*sumxy
	HomSubt(B2, A3, A4, length, bk); //N*sumx^2-sumx*sumx
	HomSubt(B3, A5, A6, length, bk); //N*sumxy-sumx*sumy
	HomRealDiv(a0, B1, B2, length, bk); //sumx^2*sumy-sumx*sumxy/N*sumx^2-sumx*sumx
	HomRealDiv(a1, B3, B2, length, bk); //N*sumxy-sumx*sumy/N*sumx^2-sumx*sumx

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
	
	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, M1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, M2[i]);	
	}	

	// delete_gate_bootstrapping_ciphertext_array(length, A);
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
