// exponential 2 of b
// binary exponentiation

void HomBinExpN(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	/// absolute value of x
	LweSample* avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomAbs(avx, b, length, bk);

	/// 1) integer part of abs(x) : int_avx
	LweSample* int_avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = (length/2); i < length; i++) // xxxs 0000
		bootsCOPY(&int_avx[i - length/2], &avx[i], bk);
	for(int i = (length/2); i < length; i++) 
		bootsCONSTANT(&int_avx[i], 0, bk);
		
	/// 2) decimal part of avx : nxr -> xxxx 0000
	LweSample* nxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length/2; i++){
		bootsCOPY(&nxr[i], &avx[i], bk);
	}
	for(int i = length/2; i < length; i++){
		bootsCONSTANT(&nxr[i], 0, bk);
	}
	/// temp3, temp4
	LweSample* temp3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp3[i], 0 , bk);
		bootsCONSTANT(&temp4[i], 0 , bk);
	}
	/// 3) nk : 1 (shift) -> 0000 1000
	LweSample* nk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++)
		bootsCONSTANT(&nk[i], 0, bk);
	bootsCONSTANT(&nk[length/2], 1, bk);

	///////// algorithm 2 : exponential function with integer x < 0 /////////
	//// lwe samples
	LweSample* nenci = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* noutput = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++)
		bootsCONSTANT(&noutput[i], 0, bk); // noutput init 0
	LweSample* nsk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oan = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* snxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oasr = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//// LOOP
	for(int i = 0; i < length/2; i++){
		///// E[i] /////		
		HomP2C(nenci, i, length, bk);

		///// msb part /////
		HomEqui(&noutput[i], nenci, int_avx, length, bk);
		HomLShift(nsk, nk, length, i, bk);
		
		for(int j = 0; j < length; j++)
			bootsAND(&oan[j], &noutput[i], &nsk[j], bk);
		
		HomAdd(temp3, oan, temp3, length, bk);

		///// next to msb part /////
		HomLShift(snxr, nxr, length, i+1, bk);
		
		for(int j = 0; j < length; j++)
			bootsAND(&oasr[j], &noutput[i], &snxr[j], bk);
		
		HomAdd(temp4, oasr, temp4, length, bk);
	}
	//// LOOP ENDS

	LweSample* final_neg = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomSubt(final_neg, temp3, temp4, length, bk);
	
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final_neg[i], bk);
	}

	delete_gate_bootstrapping_ciphertext_array(length, avx);
	delete_gate_bootstrapping_ciphertext_array(length, nenci);
	delete_gate_bootstrapping_ciphertext_array(length, noutput);
	delete_gate_bootstrapping_ciphertext_array(length, int_avx);
	delete_gate_bootstrapping_ciphertext_array(length, nk);
	delete_gate_bootstrapping_ciphertext_array(length, nsk);
	delete_gate_bootstrapping_ciphertext_array(length, oan);
	delete_gate_bootstrapping_ciphertext_array(length, temp3);
	delete_gate_bootstrapping_ciphertext_array(length, nxr);
	delete_gate_bootstrapping_ciphertext_array(length, snxr);
	delete_gate_bootstrapping_ciphertext_array(length, oasr);
	delete_gate_bootstrapping_ciphertext_array(length, temp4);
	delete_gate_bootstrapping_ciphertext_array(length, final_neg);
}
