# Linear Regression
void HomLinRegGD(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* beta0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* beta1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//// Initialization : parameter beta0, beta1 set to binary zero array
	// beta0(intercept) = 0, beta1(slope) = 0
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&beta0[i], 0, bk);
	}
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&beta1[i], 0, bk);
	}

	//////Algorithm : Linear Regression Gradient Descent
	//iteration : 5 times
	int iter = 5;
	// dJ/dbeta1 = 1/N * sum(x_i) * x_i
	// 1/N = (E1)/EN = E1_EN 
	LweSample* EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1_EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//Division supplementary : not used
	// LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomRealP2C(EN, N, length, bk);

	HomRealP2C(E1, 1, length, bk);

	HomRealDiv(E1_EN, E1, EN, length, bk);

	//////gradient descent part 1 : construct dJ/dbeta0 and dJ/ dbeta1

	// 1) dJ/dbeta0 = grad_b0 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	// 2) dJ/dbeta1 = grad_b1 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	//// dJ/dbeta1 = (1/N) * summation{(-beta0 + beta1 * x_i)} * x_i
	LweSample *L1[N], *L2[N], *L3[N], *L4[N];
	for(int i=0;i<N;i++)
	{
		L1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L3[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L4[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}
	LweSample* S1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* S2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	

	//LR(Learning Rate)= 0.01 (binary) = 0.25 (decimal) Learning Rate: 1/128
	LweSample* LR = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&LR[i], 0, bk);
	}
	bootsCONSTANT(&LR[(length/2)-7], 1, bk);


	int s=1;
	printf("update loop starts\n");
	///iteration cycle; update loop
	for(int k=0;k<iter;k++)
	{
		printf("loop %d initialization\n", s);
		//S1, S2 initializes with elements all 0
		for(int i=0;i<length;i++)
		{
			bootsCONSTANT(&S1[i], 0, bk);
			bootsCONSTANT(&S2[i], 0, bk);
		}

		for(int i=0;i<N;i++)
		{
			HomMultiReal(L1[i], beta1, x[i], length, bk);	// beta1 * x1
			HomAdd(L2[i], L1[i], beta0, length, bk);	// beta0 + beta1 * x1
			HomSubt(L3[i], L2[i], y[i], length, bk);	// [(beta0 + beta1 * x1) - y1]
			HomMultiReal(L4[i], L3[i], x[i], length, bk);	// {[(beta0 + beta1 * x1) - y1]} * x1
			HomAdd(S1, S1, L3[i], length, bk);	// sum [{[(beta0 + beta1 * x1) - y1]} for beta0
			HomAdd(S2, S2, L4[i], length, bk);	// sum [{[(beta0 + beta1 * x1) - y1]} * x1} for beta1
		}

		/// b-1) (1/N) * sum() = E1_EN * S1
		HomMultiReal(grad_b0, E1_EN, S1, length, bk);
		/// b-2) -(1/2N) * sum() = E1_EN * S2
		HomMultiReal(grad_b1, E1_EN, S2, length, bk);

		//////gradient descent part 2 : With learning rate(LR) construct LR*(dJ/dbeta0) and LR*(dJ/dbeta1)

		//update_b0 = LR * (dJ/dbeta0) and update_b1 = LR * (dJ/dbeta1)
		LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		for(int i=0;i<length;i++)
		{
			bootsCONSTANT(&update_b0[i], 0, bk);
		}
		for(int i=0;i<length;i++)
		{
			bootsCONSTANT(&update_b1[i], 0, bk);
		}
		// HomMultiReal(update_b0, LR, grad_b0, length, bk);
		// HomMultiReal(update_b1, LR, grad_b1, length, bk);

		// HomMultiReal(update_b0, LR, grad_b0, length, bk);
		HomLShift(update_b0, grad_b0, length, 7, bk);
		// HomMultiReal(update_b1, LR, grad_b1, length, bk);
		HomLShift(update_b1, grad_b1, length, 7, bk);


		//////gradient descent part 3 : parameter update phase
		//(update)beta0 = beta0 - update_b0, (update)beta1 = beta1 - update_b1 
		HomSubt(beta0, beta0, update_b0, length, bk);
		HomSubt(beta1, beta1, update_b1, length, bk);
		printf("loop %d termination\n", s);
			///iteration terminates
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
	// delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, grad_b0);
	delete_gate_bootstrapping_ciphertext_array(length, grad_b1);
	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, L1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L2[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L3[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L4[i]);	
	}
	delete_gate_bootstrapping_ciphertext_array(length, S1);
	delete_gate_bootstrapping_ciphertext_array(length, S2);
	delete_gate_bootstrapping_ciphertext_array(length, LR);
	delete_gate_bootstrapping_ciphertext_array(length, update_b0);
	delete_gate_bootstrapping_ciphertext_array(length, update_b1);
}
