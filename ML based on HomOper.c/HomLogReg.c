# Logistic Regression
void HomLogReg(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* beta0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* beta1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//// Initialization : parameter beta0, beta1 set to binary zero array
	// beta0 = 0, beta1 = 0
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&beta0[i], 0, bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&beta1[i], 0, bk);
	}

	//////Algorithm : Logistic Regression Gradient Descent
	//iteration : 3 times
	int iter = 1;
	// 1 / 1 + exp(-BX) ::: BX = beta0*x1 + beta1*x2
	// dJ/dbeta1 = 1/N * sum(sig(x_i)) * x_i
	// 1/N = (E1)/EN = E1_EN 
	LweSample* EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1_EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//Division supplementary : not used
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomRealP2C(EN, N, length, bk);
	HomRealP2C(E1, 1, length, bk);
	HomDiv(E1_EN, A, E1, EN, length, bk);

	//////gradient descent part 1 : construct dJ/dbeta0 and dJ/ dbeta1
	// 1) dJ/dbeta0 = grad_b0 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b0 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	// 2) dJ/dbeta0 = grad_b1 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b1 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	//// dJ/dbeta1 = (1/N) * summation{sig(-beta0 + beta1 * x_i)} * x_i
	LweSample *L1[N], *L2[N], *L3[N], *L4[N], *L5[N], *L6[N];
	for(int i=0;i<N;i++)
	{
		L1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L3[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L4[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L5[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L6[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}
	LweSample* S1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* S2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	

	/// a-1) S1 = summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] } = L5[0] + L5[1] + ... + L5[N-1]
	/// a-2) S2 = summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] * x1 } = L6[0] + L6[1] + ... + L6[N-1]
	//LR(Learning Rate)= 0.01 (binary) = 0.25 (decimal)
	LweSample* LR = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&LR[i], 0, bk);
	}
	bootsCONSTANT(&LR[(length/2)-3], 1, bk);
	//S1, S2 initializes with elements all 0
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&S1[i], 0, bk);
		bootsCONSTANT(&S2[i], 0, bk);
	}

	int s=1;
	printf("update loop starts\n");
	///iteration cycle; update loop
	for(int k=0;k<iter;k++)
	{
		printf("loop %d initialization\n", s);
		for(int i=0;i<N;i++)
		{
			HomMulti(L1[i], beta1, x[i], length, bk);	// beta1 * x1
			HomAdd(L2[i], L1[i], beta0, length, bk);	// beta0 + beta1 * x1
			HomTwosCompliment(L3[i], L2[i], length, bk);	// -(beta0 + beta1 * x1)
			HomSig(L4[i], L3[i], length, bk);	// 1 (div) 1 + exp(-(beta0 + beta1 * x1))
			HomSubt(L5[i], L4[i], y[i], length, bk);	// {1 (div) 1 + exp(-(beta0 + beta1 * x1))} - y1
			HomMulti(L6[i], L5[i], x[i], length, bk);	// [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) - y1 ] * x1
			HomAdd(S1, S1, L5[i], length, bk);	// summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] } for beta0
			HomAdd(S2, S2, L6[i], length, bk);	// summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] * x1 } for beta1
		}

		/// b-1) (1/N) * summation() = E1_EN * S1
		HomMulti(grad_b0, E1_EN, S1, length, bk);
		/// b-2) -(1/2N) * summation() = E1_EN * S2
		HomMulti(grad_b1, E1_EN, S2, length, bk);

		//////gradient descent part 2 : With learning rate(LR) construct LR*(dJ/dbeta0) and LR*(dJ/dbeta1)
		//update_b0 = LR * (dJ/dbeta0) and update_b1 = LR * (dJ/dbeta1)
		// LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		// LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);

		HomMulti(update_b0, LR, grad_b0, length, bk);
		HomMulti(update_b1, LR, grad_b1, length, bk);

		//////gradient descent part 3 : parameter update phase
		//(update)beta0 = beta0 - update_b0, (update)beta1 = beta1 - update_b1 
		HomAdd(beta0, beta0, update_b0, length, bk);
		HomAdd(beta1, beta1, update_b1, length, bk);
		printf("loop %d termination\n", s);
		s += 1;
	}
	///iteration terminates
	//copy updated result of beta0 to result(res0)
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[0][i], &beta0[i], bk);	
	}
	//copy updated result of beta1 to result(res1)
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[1][i], &beta1[i], bk);	
	}

	//delete bootstrapping ciphertext arrays
	delete_gate_bootstrapping_ciphertext_array(length, beta0);
	delete_gate_bootstrapping_ciphertext_array(length, beta1);
	delete_gate_bootstrapping_ciphertext_array(length, EN);
	delete_gate_bootstrapping_ciphertext_array(length, E1);
	delete_gate_bootstrapping_ciphertext_array(length, E1_EN);
	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(2*length, grad_b0);
	delete_gate_bootstrapping_ciphertext_array(2*length, grad_b1);
	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, L1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L2[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L3[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L4[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L5[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L6[i]);		
	}
	delete_gate_bootstrapping_ciphertext_array(length, S1);
	delete_gate_bootstrapping_ciphertext_array(length, S2);
	delete_gate_bootstrapping_ciphertext_array(length, LR);
	delete_gate_bootstrapping_ciphertext_array(length, update_b0);
	delete_gate_bootstrapping_ciphertext_array(length, update_b1);
}
