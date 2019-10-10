// exponential 2 of b
// binary exponentiation

void HomBinExpN(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* enci = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* output = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* int_avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nsk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oan = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* snxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oasr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* final = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	/// absolute value of x
	HomAbs(avx, b, length, bk);

	for(int i = (length/2)-1; i < length-1; i++){
		bootsCOPY(&int_avx[i-(length/2)+1], &avx[i], bk);
	}

	/// nk : 1 (shift)
	for(int i = 0; i < (length/2)-1; i++){
		bootsCONSTANT(&nk[i], 0, bk);
	}
	bootsCONSTANT(&nk[(length/2)-1], 1, bk);
	
	/// temp3, temp4
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp3[i], 0 , bk);
		bootsCONSTANT(&temp4[i], 0 , bk);
	}

	/// decimal part of x
	for(int i = 0; i < (length/2)-1; i++){
		bootsCOPY(&nxr[i], &avx[i], bk);
	}

	///////// algorithm 2 : exponential function with integer x < 0 /////////

	for(int i = 0; i < (length/2); i++){
		///// E[i] /////		
		HomP2C(enci, i, (length/2), bk);

		///// msb part /////
		HomEqui(&output[i], enci, int_avx, (length/2), bk);
		HomLShift(nsk, nk, length/2, i, bk);
		
		for(int j = 0; j < length/2; j++){
			bootsAND(&oan[j], &output[i], &nsk[j], bk);
		}
		HomAdd(temp3, oan, temp3, length/2,bk);

		///// next to msb part /////
		HomLShift(snxr, nxr, length-1, i+1, bk);
		
		for(int j = 0; j < length-1; j++){
			bootsAND(&oasr[j], &output[i], &snxr[j], bk);
		}
		HomAdd(temp4, oasr, temp4, (length/2)-1, bk);
}
	
	HomSubt(final, temp3, temp4, length, bk);
	
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final[i], bk);
	}

	delete_gate_bootstrapping_ciphertext_array(length, avx);
	delete_gate_bootstrapping_ciphertext_array(length, enci);
	delete_gate_bootstrapping_ciphertext_array(length, output);
	delete_gate_bootstrapping_ciphertext_array(length, int_avx);
	delete_gate_bootstrapping_ciphertext_array(length, nk);
	delete_gate_bootstrapping_ciphertext_array(length, nsk);
	delete_gate_bootstrapping_ciphertext_array(length, oan);
	delete_gate_bootstrapping_ciphertext_array(length, temp3);
	delete_gate_bootstrapping_ciphertext_array(length, nxr);
	delete_gate_bootstrapping_ciphertext_array(length, snxr);
	delete_gate_bootstrapping_ciphertext_array(length, oasr);
	delete_gate_bootstrapping_ciphertext_array(length, temp4);
	delete_gate_bootstrapping_ciphertext_array(length, final);
}
