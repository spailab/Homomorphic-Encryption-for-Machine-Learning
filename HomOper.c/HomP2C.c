void HomP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);

	
	int8_t plain = num;

	for(int i = 0; i < length; i++)
		bootsCONSTANT(&res[i], (plain>>i)&1, bk);


	delete_gate_bootstrapping_ciphertext_array(1, temp);
}
