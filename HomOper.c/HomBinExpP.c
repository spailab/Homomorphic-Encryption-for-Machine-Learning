// exponential 2 of b
// binary exponentiation

void HomBinExpP(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* enci = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* output = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* int_x = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* k = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* msk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oam = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* xr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oar = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* stemp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* final = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	
	/// integer part of x : int_x
	// for(int i = 0; i < (length/2); i++){
	// 	bootsCONSTANT(&int_x[i], 0 , bk);
	// }
	for(int i = (length/2)-1; i < length-1; i++){
		bootsCOPY(&int_x[i-(length/2)+1], &b[i], bk);
	}

	/// decimal part of x : xr
	// for(int i = 0; i < length; i++){
	// 	bootsCONSTANT(&xr[i], 0 , bk);
	// }
	for(int i = 0; i < (length/2)-1; i++){
		bootsCOPY(&xr[i], &b[i], bk);
	}

	/// temp, temp2
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp[i], 0 , bk);
		bootsCONSTANT(&temp2[i], 0 , bk);
	}

	/// k : 1 (shift)
	bootsCONSTANT(&k[0], 1, bk);
	for(int i = 1; i < (length/2); i++){
		bootsCONSTANT(&k[i], 0, bk);
	}

	///////// algorithm 1 : exponential function with integer x >=0 /////////
	for(int i = 0; i < (length/2); i++){
		///// E[i] /////		
		HomP2C(enci, i, (length/2), bk);

		///// msb part /////
		HomEqui(&output[i], enci, int_x, (length/2), bk);
		HomRShift(msk, k, length/2, i, bk);
		

		for(int j = 0; j < length/2; j++){
			bootsAND(&oam[j], &output[i], &msk[j], bk);
		}
		HomAdd(temp, oam, temp, length/2,bk);

		///// next to msb part /////
		HomRShift(sxr, xr, length-1, i, bk);
		
		for(int j = 0; j < length-1; j++){
			bootsAND(&oar[j], &output[i], &sxr[j], bk);
		}
		HomAdd(temp2, oar, temp2, length-2, bk);
}
	HomRShift(stemp, temp, length-1, (length/2)-1, bk);

	HomAdd(final, stemp, temp2, length, bk);
	
	for(int i = 0; i < length-2; i++){
		bootsCOPY(&res[i], &final[i], bk);
	}


	// for(int i = (length/2)-1; i < length; i++){
	// 	bootsCOPY(&res[i], &temp[i-(length/2)+1], bk);
	// }

	// for(int i = 0; i < length-2; i++){
	// 	bootsCOPY(&res[i], &temp2[i], bk);
	// }

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
	delete_gate_bootstrapping_ciphertext_array(length, final);
	
}
