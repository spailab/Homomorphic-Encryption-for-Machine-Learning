void HomRealP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);

	int16_t plain = num;

	for(int i = 0; i < (length/2)-1; i++)
		bootsCONSTANT(&res[i], 0, bk);
	for(int i = 0; i < (length/2)+1; i++)
		bootsCONSTANT(&res[i+(length/2)-1], (plain>>i)&1, bk);

	delete_gate_bootstrapping_ciphertext_array(1, temp);
}
