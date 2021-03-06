// log base 2 of b

void HomLogT5(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* sf = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mb = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mb2 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mb3 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mb4 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mb5 = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* mbt = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* mbt5 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* div5 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* O_5 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
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
		bootsCONSTANT(&O_4[i], 0, bk);
		bootsCONSTANT(&O_5[i], 0, bk);
	}
	
	//// temporary array : mbt, mbt2, mbt3,  mbt4
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&mbt[i], 0, bk);
		bootsCONSTANT(&mbt2[i], 0, bk);
		bootsCONSTANT(&mbt3[i], 0, bk);
		bootsCONSTANT(&mbt4[i], 0, bk);
	}

	//// division factors : div2, div3, div4, div5 
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
	// division factor : div4 
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&div4[i], 0, bk);
	}
	bootsCONSTANT(&div4[length/2+1], 1, bk);
	// division factor : div5 
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&div5[i], 0, bk);
	}
	bootsCONSTANT(&div5[length/2-1], 1, bk);
	bootsCONSTANT(&div5[length/2+1], 1, bk);
	
	
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


	/////// STEP4
	HomMulti(mb4, mbt2, mb, length, bk);
	for(int i = 0; i < length; i++){
		bootsCOPY(&mbt3[i], &mb4[i+length/2], bk);
	}
	
	
	// divide by a divisor
	HomDiv(O_4, A, mbt3, div4, length, bk);


	/////// STEP5
	HomMulti(mb5, mbt3, mb, length, bk);
	for(int i = 0; i < length; i++){
		bootsCOPY(&mbt5[i], &mb5[i+length/2], bk);
	}

	
	// divide by a divisor
	HomDiv(O_5, A, mbt3, div5, length, bk);

	/////// Summation of all the output : output
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&output[i], 0, bk);
	}
	
	HomSubt(output, mb, O_2, length, bk);
	HomAdd(output, output, O_3, length, bk);	
	HomSubt(output, output, O_4, length, bk);
	HomAdd(output, output, O_5, length, bk);	

	for(int i = 0; i < length; i++){
	bootsCOPY(&res[i], &output[i], bk);
	}



	delete_gate_bootstrapping_ciphertext_array(length, sf);
	delete_gate_bootstrapping_ciphertext_array(length, mb);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb2);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb3);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb4);
	delete_gate_bootstrapping_ciphertext_array(2*length, mb5);
	delete_gate_bootstrapping_ciphertext_array(length, mbt);
	delete_gate_bootstrapping_ciphertext_array(length, mbt2);
	delete_gate_bootstrapping_ciphertext_array(length, mbt3);
	delete_gate_bootstrapping_ciphertext_array(length, div2);
	delete_gate_bootstrapping_ciphertext_array(length, div3);
	delete_gate_bootstrapping_ciphertext_array(length, div4);
	delete_gate_bootstrapping_ciphertext_array(length, div5);
	delete_gate_bootstrapping_ciphertext_array(length, O_2);
	delete_gate_bootstrapping_ciphertext_array(length, O_3);
	delete_gate_bootstrapping_ciphertext_array(length, O_4);
	delete_gate_bootstrapping_ciphertext_array(length, O_5);
	delete_gate_bootstrapping_ciphertext_array(length, output);


}
