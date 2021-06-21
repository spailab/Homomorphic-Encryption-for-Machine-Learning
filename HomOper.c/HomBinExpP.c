// exponential 2 of b
// binary exponentiation

void HomBinExpP(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	/// 1) integer part of x : int_x -> 3 out of 8 xxxs 0000
	LweSample* int_x = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = (length/2); i < length; i++){ 
		bootsCOPY(&int_x[i-(length/2)], &b[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&int_x[i], 0, bk);
	}

	/// 2) decimal part of x : xr -> xxxx 0000
	LweSample* xr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < (length/2); i++){ // 4 out of 8
		bootsCOPY(&xr[i], &b[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&xr[i], 0, bk);
	}

	/// temp, temp2
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp[i], 0 , bk); // 3 out of 8, but 0000 0000
		bootsCONSTANT(&temp2[i], 0 , bk); // 4 out of 8, but 0000 0000
	}

	/// 3) k : 1 (shift) 1000 0000
	LweSample* k = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	bootsCONSTANT(&k[0], 1, bk); // 3 out of 8
	for(int i = 1; i < length; i++){
		bootsCONSTANT(&k[i], 0, bk);
	}

	// lwe samples
	LweSample* enci = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* output = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++){ // initialize output -> 0
		bootsCONSTANT(&output[i], 0, bk);
	}

	LweSample* msk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oam = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oar = new_gate_bootstrapping_ciphertext_array(length, bk->params);


	///////// algorithm 1 : exponential function with integer x >=0 /////////
	
	/// LOOP
	for(int i = 0; i < length/2 - 1; i++){
		///// E[i] /////		
		HomP2C(enci, i, length, bk); 

		///// msb part /////
		HomEqui(&output[i], enci, int_x, length, bk); 
		HomRShift(msk, k, length, i, bk); // 3 out of 8
				
		for(int j = 0; j < length/2 - 1; j++){
			bootsAND(&oam[j], &output[i], &msk[j], bk);  
		}
		HomAdd(temp, oam, temp, length, bk); // 3 out of 8 (?)

		///// next to msb part /////
		HomRShift(sxr, xr, length, i, bk); // 8
		
		for(int j = 0; j < length; j++){
			bootsAND(&oar[j], &output[i], &sxr[j], bk); // 8
		}
		HomAdd(temp2, oar, temp2, length, bk); // 8
	}
	/// LOOP ENDS
	
	LweSample* stemp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomRShift(stemp, temp, length, (length/2), bk);

	
	LweSample* final_pos = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomAdd(final_pos, stemp, temp2, length, bk);
	
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final_pos[i], bk);
	}
	

	delete_gate_bootstrapping_ciphertext_array(length, enci);
	delete_gate_bootstrapping_ciphertext_array(length, output);
	delete_gate_bootstrapping_ciphertext_array(length, int_x);
	delete_gate_bootstrapping_ciphertext_array(length, k);
	delete_gate_bootstrapping_ciphertext_array(length, msk);
	delete_gate_bootstrapping_ciphertext_array(length, oam);
	delete_gate_bootstrapping_ciphertext_array(length, temp);
	delete_gate_bootstrapping_ciphertext_array(length, xr);
	delete_gate_bootstrapping_ciphertext_array(length, sxr);
	delete_gate_bootstrapping_ciphertext_array(length, oar);
	delete_gate_bootstrapping_ciphertext_array(length, temp2);
	delete_gate_bootstrapping_ciphertext_array(length, stemp);
	delete_gate_bootstrapping_ciphertext_array(length, final_pos);
	
}
