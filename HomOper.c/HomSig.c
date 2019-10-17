void HomSig(LweSample* res, const LweSample* x, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* null = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* exp_mx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* one = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* dec = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* outcome = new_gate_bootstrapping_ciphertext_array(length, bk->params);


	/////for sigmoid function, we need -x for 1/(1+exp(-x))
	////x converted to -x by 0-x
	//make 0 in a binary array
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&null[i], 0, bk);
	}
	//0-x
	HomSubt(mx, null, x, length, bk);
	////exp(-x) using HomBinExp
	HomBinExpP(exp_mx, mx, length, bk);
	////1+exp(-x)
	//make 1 for addition
	for(int i=1;i<length;i++)
	{
		bootsCONSTANT(&one[i], 0, bk);
	}
	bootsCONSTANT(&one[0], 1, bk);
	//add 1 to exp(-x) that is one + exp_mx and store it in dec
	HomAdd(dec, one, exp_mx, length, bk);
	////Division of dec by 1 is our result 
	//remainder not needed : A
	HomDiv(outcome, A, one, dec, length, bk);
	//copy updated result of outcome to res
	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[i], &outcome[i], bk);	
	}

	delete_gate_bootstrapping_ciphertext_array(length, null);
	delete_gate_bootstrapping_ciphertext_array(length, mx);
	delete_gate_bootstrapping_ciphertext_array(length, exp_mx);
	delete_gate_bootstrapping_ciphertext_array(length, one);
	delete_gate_bootstrapping_ciphertext_array(length, dec);
	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, outcome);

}
