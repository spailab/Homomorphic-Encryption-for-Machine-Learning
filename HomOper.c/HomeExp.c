///////////////////////////////     Exponential function base 'e'      ////////////////////////////
// exponential e of b

void HomExp(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	//////////       1. Preprocessing        //////////
	///// use log2(e) * b as an input
	/// 1) build up log2(e)=110.10000 when 8 bit
	/// for ref : log2(e)=1.01110001010101001101 binary
	
	////////////////    8-bit, 16-bit, 32-bit
	
	LweSample* pre = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* ni = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	// 	/// 16bit: 10001 110.10 00000 0
	for(int i = 0; i < length; i++)
		bootsCONSTANT(&pre[i], 0, bk);
	bootsCONSTANT(&pre[0], 1, bk);
	bootsCONSTANT(&pre[4], 1, bk);
	bootsCONSTANT(&pre[5], 1, bk);
	bootsCONSTANT(&pre[6], 1, bk);
	bootsCONSTANT(&pre[8], 1, bk);

	/// 2) new_input(ni) = log2(e)*b = pre (MULT) b 
	HomMultiReal(ni, pre, b, length, bk);	

	/// 3) absolute value operation for both
	LweSample* avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomAbs(avx, ni, length, bk);

	
	// //////////       2. Positive Exponentiation        //////////
	// ///// start with avx
		/// 1) integer part of x : int_x -> 3 out of 8 xxxs 0000
	LweSample* int_x = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = (length/2); i < length; i++){ 
		bootsCOPY(&int_x[i-(length/2)], &avx[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&int_x[i], 0, bk); 
	}

	// /// 2) decimal part of x : xr -> xxxx 0000
	LweSample* xr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < (length/2); i++){ // 4 out of 8
		bootsCOPY(&xr[i], &avx[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&xr[i], 0, bk);
	}

	// /// temp, temp2
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp[i], 0 , bk); // 3 out of 8, but 0000 0000
		bootsCONSTANT(&temp2[i], 0 , bk); // 4 out of 8, but 0000 0000
	}

	// /// 3) k : 1 (shift) 1000 0000
	LweSample* k = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	bootsCONSTANT(&k[0], 1, bk); // 3 out of 8
	for(int i = 1; i < length; i++){
		bootsCONSTANT(&k[i], 0, bk);
	} // k = 1000 0000 = 0.003906


	// // lwe samples
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
		/// E[i] /////		
		HomP2C(enci, i, length, bk); 

		///// msb part /////
		HomEqui(&output[i], enci, int_x, length, bk); 
		HomRShift(msk, k, length, i, bk); // 3 out of 8
				
		// for(int j = 0; j < length/2 - 1; j++){ // wrong for loop
		for(int j = 0; j < length; j++){
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

	//////////       3. Negative Exponentiation        //////////
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
	
	//////////       4. Selective Integration        //////////
	///// ~x[signed bit] (AND) A + x[signed bit] (AND) B :: A = positive alg. outcome, B = negative alg. outcome
	//~ni (AND) A : positive
	LweSample* nb = new_gate_bootstrapping_ciphertext_array(1, bk->params);
	LweSample* final = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	bootsNOT(&nb[0], &ni[length-1], bk);
	for(int i = 0; i < length; i++){
		bootsAND(&final_pos[i], &nb[0], &final_pos[i], bk);
	}	
	//x (AND) B : negative
	for(int i = 0; i < length; i++){
		bootsAND(&final_neg[i], &ni[length-1], &final_neg[i], bk);
	}	
	// ~x[signed bit] (AND) A + x[signed bit]
	HomAdd(final, final_pos, final_neg, length, bk);	
	//copy final to result
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final[i], bk);
	}

	//preprocessing
	delete_gate_bootstrapping_ciphertext_array(length, pre);
	delete_gate_bootstrapping_ciphertext_array(length, ni);
	delete_gate_bootstrapping_ciphertext_array(length, avx);
	//positive ciphertext array : delete
	delete_gate_bootstrapping_ciphertext_array(length, int_x);
	delete_gate_bootstrapping_ciphertext_array(length, xr);
	delete_gate_bootstrapping_ciphertext_array(length, temp);
	delete_gate_bootstrapping_ciphertext_array(length, temp2);
	delete_gate_bootstrapping_ciphertext_array(length, k);
	delete_gate_bootstrapping_ciphertext_array(length, enci);
	delete_gate_bootstrapping_ciphertext_array(length, output);
	delete_gate_bootstrapping_ciphertext_array(length, msk);
	delete_gate_bootstrapping_ciphertext_array(length, oam);
	delete_gate_bootstrapping_ciphertext_array(length, sxr);
	delete_gate_bootstrapping_ciphertext_array(length, oar);
	delete_gate_bootstrapping_ciphertext_array(length, stemp);
	delete_gate_bootstrapping_ciphertext_array(length, final_pos);	
	//negative ciphertext array : delete
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
