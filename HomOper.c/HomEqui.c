// if a = b then res = E(1)
// else res = E(0)
//

void HomEqui(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);	    

	bootsCONSTANT(&temp[0], 1, bk);
	for(int i = 0; i < length; i++){		
		bootsXNOR(&temp[1], &a[i], &b[i], bk);
		bootsAND(&temp[0], &temp[0], &temp[1], bk);
		
	}
	bootsCOPY(&res[0], &temp[0], bk);

	delete_gate_bootstrapping_ciphertext_array(2, temp);
}
