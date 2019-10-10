// log base 2 of b
// binary logarithm

void HomBinLog(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);
	LweSample* power2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* power2S = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* Output = new_gate_bootstrapping_ciphertext_array(length-1, bk->params);
	LweSample* int_sum = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* dec_sum = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp_sum = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//// find digit of 'b'
	// fill power2comp with zeroes
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&power2[i], 0, bk);}

	for(int i = 0; i < length; i++){
		bootsCONSTANT(&int_sum[i], 0 , bk);
		bootsCONSTANT(&dec_sum[i], 0 , bk);
		bootsCONSTANT(&temp_sum[i], 0 , bk);
	}

	for(int i = 0; i < length-2; i++){			//compare Setting with the b
		bootsCONSTANT(&power2[i], 1, bk);
		HomCompLE(&temp[0], b, power2, length, bk);		//store outcomes in Output array
		HomRShift(power2S, power2, length, 1, bk);
		HomCompS(&temp[1], b, power2S, length, bk);
		bootsAND(&Output[i], &temp[0], &temp[1], bk);
		bootsCONSTANT(&power2[i], 0, bk);
	}
	bootsCONSTANT(&power2[length-2], 1, bk);
	HomCompLE(&Output[length-2], b, power2, length, bk);

	int ll = log2(length);

	for(int i = 0; i < length-1; i++){
		HomP2C(temp_sum, i-(length/2)+1, ll+1, bk);

		for(int j = 0; j < ll+1; j++){
			bootsAND(&temp_sum[j], &temp_sum[j], &Output[i], bk);}
		HomAdd(int_sum, int_sum, temp_sum, ll+1, bk);

		if (i-(length/2)+1 < 0){
			HomRShift(res, b, length, abs(i-(length/2)+1), bk);
			for(int j = 0; j < (length/2)-1; j++){
				bootsAND(&temp_sum[j], &temp_sum[j], &Output[i], bk);}
			HomAdd(dec_sum, dec_sum, temp_sum, (length/2)-1, bk);}

		else if (i-(length/2)+1 == 0){
			for(int j = 0; j < (length/2)-1; j++){
				bootsAND(&temp_sum[j], &temp_sum[j], &Output[i], bk);}
			HomAdd(dec_sum, dec_sum, temp_sum, (length/2)-1, bk);}
						
		else {
			HomLShift(temp_sum, b, length, i-(length/2)+1, bk);
			for(int j = 0; j < (length/2)-1; j++){
				bootsAND(&temp_sum[j], &temp_sum[j], &Output[i], bk);}
			HomAdd(dec_sum, dec_sum, temp_sum, (length/2)-1, bk);}
	}

	for(int i = 0; i < (length/2)-1; i++)
		bootsCOPY(&res[i], &dec_sum[i], bk);

	for(int i = (length/2)-1; i < (length/2) + ll; i++)
		bootsCOPY(&res[i], &int_sum[i-(length/2)+1], bk);

	for(int i = (length/2) + ll; i < length; i++)
		bootsCOPY(&res[i], &int_sum[ll], bk);



	delete_gate_bootstrapping_ciphertext_array(2, temp);
	delete_gate_bootstrapping_ciphertext_array(length, power2);
	delete_gate_bootstrapping_ciphertext_array(length, power2S);
	delete_gate_bootstrapping_ciphertext_array(length-1, Output);
	delete_gate_bootstrapping_ciphertext_array(length/2, int_sum);
	delete_gate_bootstrapping_ciphertext_array(length/2, dec_sum);
	delete_gate_bootstrapping_ciphertext_array(length, temp_sum);
}
