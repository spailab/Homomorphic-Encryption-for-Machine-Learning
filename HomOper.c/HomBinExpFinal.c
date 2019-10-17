///////////////////////////////     Final Exponential function      ////////////////////////////
// exponential 2 of b
// binary exponentiation(positive + negative)

void HomBinExpFinal(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	//positive ciphertext array
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
	LweSample* final_pos = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	//negative ciphertext array
	LweSample* avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nenci = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* noutput = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* int_avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nsk = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oan = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* nxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* snxr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* oasr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* final_neg = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	//~x[signed bit] (AND) A + x[signed bit]
	LweSample* nb = new_gate_bootstrapping_ciphertext_array(1, bk->params);
	LweSample* final = new_gate_bootstrapping_ciphertext_array(length, bk->params);


	//////////        Positive Exponentiation        //////////
	/// 1) integer part of x : int_x
	for(int i = (length/2)-1; i < length-1; i++){
		bootsCOPY(&int_x[i-(length/2)+1], &b[i], bk);
	}

	/// 2) decimal part of x : xr
	for(int i = 0; i < (length/2)-1; i++){
		bootsCOPY(&xr[i], &b[i], bk);
	}
	for(int i = (length/2)-1; i < length; i++){
		bootsCONSTANT(&xr[i], 0, bk);
	}

	/// temp, temp2
	for(int i = 0; i < length/2; i++){
		bootsCONSTANT(&temp[i], 0 , bk);
		bootsCONSTANT(&temp2[i], 0 , bk);
	}

	/// 3) k : 1 (shift) [00010000]
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
		HomRShift(sxr, xr, length, i, bk);
		
		for(int j = 0; j < length; j++){
			bootsAND(&oar[j], &output[i], &sxr[j], bk);
		}
		HomAdd(temp2, oar, temp2, length, bk);
}
	HomRShift(stemp, temp, length, (length/2)-1, bk);

	HomAdd(final_pos, stemp, temp2, length, bk);

	//////////        Negative Exponentiation        //////////
	/// absolute value of x
	HomAbs(avx, b, length, bk);

	/// 1) integer part of abs(x) : int_avx
	for(int i = (length/2)-1; i < length-1; i++){
		bootsCOPY(&int_avx[i-(length/2)+1], &avx[i], bk);
	}
	/// 2) decimal part of avx : nxr
	for(int i = 0; i < (length/2)-1; i++){
		bootsCOPY(&nxr[i], &avx[i], bk);
	}
	for(int i = (length/2)-1; i < length; i++){
		bootsCONSTANT(&nxr[i], 0, bk);
	}
	/// temp3, temp4
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp3[i], 0 , bk);
		bootsCONSTANT(&temp4[i], 0 , bk);
	}
	/// 3) nk : 1 (shift)
	for(int i = 0; i < (length/2)-1; i++){
		bootsCONSTANT(&nk[i], 0, bk);
	}
	bootsCONSTANT(&nk[(length/2)-1], 1, bk);

	///////// algorithm 2 : exponential function with integer x < 0 /////////

	for(int i = 0; i < (length/2); i++){
		///// E[i] /////		
		HomP2C(nenci, i, (length/2), bk);

		///// msb part /////
		HomEqui(&noutput[i], nenci, int_avx, (length/2), bk);
		HomLShift(nsk, nk, length/2, i, bk);
		
		for(int j = 0; j < length/2; j++){
			bootsAND(&oan[j], &noutput[i], &nsk[j], bk);
		}
		HomAdd(temp3, oan, temp3, length/2,bk);

		///// next to msb part /////
		HomLShift(snxr, nxr, length, i+1, bk);
		
		for(int j = 0; j < length; j++){
			bootsAND(&oasr[j], &noutput[i], &snxr[j], bk);
		}
		HomAdd(temp4, oasr, temp4, length, bk);
}

	HomSubt(final_neg, temp3, temp4, length, bk);
	
	//////////        Selective Integration        //////////
	///// ~x[signed bit] (AND) A + x[signed bit] (AND) B :: A = positive alg. outcome, B = negative alg. outcome
	//~x (AND) A : positive
	bootsNOT(&nb[0], &b[length-1], bk);
	for(int i = 0; i < length; i++){
		bootsAND(&final_pos[i], &nb[0], &final_pos[i], bk);
	}	
	//x (AND) B : negative
	for(int i = 0; i < length; i++){
		bootsAND(&final_neg[i], &b[length-1], &final_neg[i], bk);
	}	
	// ~x[signed bit] (AND) A + x[signed bit]
	HomAdd(final, final_pos, final_neg, length, bk);	
	//copy final to result
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final[i], bk);
	}

	//positive ciphertext array : delete
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
	//negative ciphertext array : delete
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
	//~x[signed bit] (AND) A + x[signed bit]
	delete_gate_bootstrapping_ciphertext_array(1, nb);
	delete_gate_bootstrapping_ciphertext_array(length, final);
}
