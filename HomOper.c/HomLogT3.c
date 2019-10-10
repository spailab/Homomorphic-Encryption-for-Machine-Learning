// log base 2 of b

void HomLogT3(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* sf = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mb = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mb2 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mb3 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mbt = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* output = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);


	///////////////// Initialize iteration
	// Subtraction Factor(sf) = 1
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&sf[i], 0, bk);
	}
	bootsCONSTANT(&sf[length/2-1], 1, bk);

	//// Build Output array 
	// Output array : O_2, O_3, O_4, O_5
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&O_2[i], 0, bk);
		bootsCONSTANT(&O_3[i], 0, bk);
	}
	
	//// temporary array : mbt, mbt2, mbt3,  mbt4
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&mbt[i], 0, bk);
		bootsCONSTANT(&mbt2[i], 0, bk);
		bootsCONSTANT(&mbt3[i], 0, bk);
	}

	//// division factors : div2, div3, div4 
	// division factor : div2 
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&div2[i], 0, bk);
	}

	bootsCONSTANT(&div2[length/2], 1, bk);
	// division factor : div3 
	for(int i = 0; i < length; i++){

		bootsCONSTANT(&div3[i], 0, bk);
	}

	bootsCONSTANT(&div3[length/2-1], 1, bk);
	bootsCONSTANT(&div3[length/2], 1, bk);
	

	/////// STEP1 
	// Multiplication basis : mb = b - sf
	HomSubt(mb, b, sf, length, bk);
	

	/////// STEP2
	HomMulti(mb2, mb, mb, length, bk);
		
	for(int i = 0; i < length; i++){
		bootsCOPY(&mbt[i], &mb2[i+length/2], bk);
	}

	// divide by a divisor
	HomDiv(O_2, A, mbt, div2, length, bk);

	
	/////// STEP3
	HomMulti(mb3, mbt, mb, length, bk);
	
	for(int i = 0; i < length; i++){
	bootsCOPY(&mbt2[i], &mb3[i+length/2], bk);
	}
	
	// divide by a divisor
	HomDiv(O_3, A, mbt2, div3, length, bk);


	/////// Summation of all the output : output
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&output[i], 0, bk);
	}
	
	HomSubt(output, mb, O_2, length, bk);
	HomAdd(output, output, O_3, length, bk);	


	for(int i = 0; i < length; i++){
	bootsCOPY(&res[i], &output[i], bk);
	}



	delete_gate_bootstrapping_ciphertext_array(length, sf);
	delete_gate_bootstrapping_ciphertext_array(length, mb);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb2);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb3);
	delete_gate_bootstrapping_ciphertext_array(length, mbt);
	delete_gate_bootstrapping_ciphertext_array(length, mbt2);
	delete_gate_bootstrapping_ciphertext_array(length, mbt3);
	delete_gate_bootstrapping_ciphertext_array(length, div2);
	delete_gate_bootstrapping_ciphertext_array(length, div3);
	delete_gate_bootstrapping_ciphertext_array(length, O_2);
	delete_gate_bootstrapping_ciphertext_array(length, O_3);
	delete_gate_bootstrapping_ciphertext_array(length, output);


}
