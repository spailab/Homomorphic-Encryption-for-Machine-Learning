void HomModSel(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* sy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sxy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sx2y = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* syy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sxxy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	
	// initialize all elements set to binary zero array

	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&sy[i],0,bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&sxy[i],0,bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&sx2y[i],0,bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&syy[i],0,bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&sxxy[i],0,bk);
	}	

	// initialize a(result) to 0

	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a[i], 0, bk);
	}

	LweSample *A1[N], *A2[N], *A3[N], *A4[N];
	for(int i=0;i<N-1;i++)
	{
		A1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		A2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		A3[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		A4[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}


	//calculation start

	for(int i = 0; i<N; i++)
		{
		HomAdd(A1, A1, x[i], length, bk);
		HomMulti(x2[i], x[i], x[i], length, bk);
		HomAdd(A2, A2, x2[i], length, bk);
		HomMulti(x3[i], x2[i], x[i], length, bk);
		HomAdd(A3, A3, x3[i], length, bk);
		HomMulti(x4[i], x3[i], x[i], length, bk);
		HomAdd(A4, A4, x4[i], length, bk);

		HomAdd(sy, sy, y[i], length, bk);
		HomMulti(xy[i], x[i], y[i], length, bk);
		HomAdd(sxy, sxy, sxy[i], length, bk);
		HomMulti(x2y[i], x[i], y[i], length, bk);
		HomAdd(sx2y, sx2y, x2y[i], length, bk);

		HomAdd(syy, syy, y[i], length, bk);
		HomAdd(sxxy, sxxy, xy[i], length, bk);
		}

	A[3][3] = ({N, A1, A2}, {A1, A2, A3}, {A2, A3, A4});
	B[1][3] = (sy, sxy, sx2y);

	for(i=0;i<length;i++)
	{
		HomInverse(AA, A[3][3], length, bk);
		HomMulti(a, AA, b, length, bk);
	}

	// copy updated result of a
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[0][i], &a[0][i], bk);	
	}
	//copy updated result of beta1 to result(res1)
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[1][i], &beta[1][i], bk);	
	}

	//delete bootstrapping ciphertext arrays
	delete_gate_bootstrapping_ciphertext_array(length, sy);
	delete_gate_bootstrapping_ciphertext_array(length, sxy);
	delete_gate_bootstrapping_ciphertext_array(length, sx2y);
	delete_gate_bootstrapping_ciphertext_array(length, sxxy);
	delete_gate_bootstrapping_ciphertext_array(length, syy);
	delete_gate_bootstrapping_ciphertext_array(length, a);
	delete_gate_bootstrapping_ciphertext_array(2*length, a);

	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, A1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, A2[i]);
		delete_gate_bootstrapping_ciphertext_array(length, A3[i]);
		delete_gate_bootstrapping_ciphertext_array(length, A4[i]);	
	}
	delete_gate_bootstrapping_ciphertext_array(length, AA);
}

