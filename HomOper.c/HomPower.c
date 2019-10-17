void HomPower(LweSample* res, const LweSample* a, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	// LweSample *c[N];
	// for(int i = 0; i <N; i++)
	// {
 //     c[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);	    	    
 //    }
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    // c = power, c = 1
	for(int i = 0; i < (length/2)-1; i++){
		bootsCONSTANT(&c[i], 0, bk);
	}
	bootsCONSTANT(&c[(length/2)-1], 1, bk);
    printf("1");

    HomAbs(A,a,length,bk);
    // a = base, b = exponent;
	for(int i = 1; i < N + 1; i++)
	{
		HomMultiReal(c, c, A, length, bk);	
	}

	printf("2");

	for(int i = 0; i < length; i++)
	{
	bootsCOPY(&res[i], &c[i], bk);
	}

	// for(int i=0;i<N;i++)
	// {
	// 	delete_gate_bootstrapping_ciphertext_array(length, c[i]);      
	// }	
	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, c);

}
