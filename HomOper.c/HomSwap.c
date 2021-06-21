# Swap
void HomSwap(const LweSample* c, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	
	LweSample* temp =  new_gate_bootstrapping_ciphertext_array(1, bk->params);
	LweSample* D1 =  new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* D2 =  new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1 =  new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E2 =  new_gate_bootstrapping_ciphertext_array(length, bk->params);

	bootsNOT(&temp[0], &c[0], bk);


	for(int i = 0; i < length; i++){
		bootsAND(&D1[i], &a[i], &c[0], bk);
		bootsAND(&D2[i], &a[i], &temp[0], bk);
		bootsAND(&E1[i], &b[i], &c[0], bk);
		bootsAND(&E2[i], &b[i], &temp[0], bk);}

	HomAdd(a, D2, E1, length, bk);
	HomAdd(b, D1, E2, length, bk);


	delete_gate_bootstrapping_ciphertext_array(1, temp);
	delete_gate_bootstrapping_ciphertext_array(length, D1);
	delete_gate_bootstrapping_ciphertext_array(length, D2);
	delete_gate_bootstrapping_ciphertext_array(length, E1);
	delete_gate_bootstrapping_ciphertext_array(length, E2);
}

