void HomInv(LweSample* res, const LweSample ** a, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* d = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* L = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* K = new_gate_bootstrapping_ciphertext_array(length, bk->params);              
	LweSample* f = new_gate_bootstrapping_ciphertext_array(2, bk->params);	    
    LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    //find dimensions of input matrix
    int C = N; int r = N;

    HomEye(f[M][M],a[N][N],N,length, bk);
     /// Let's start inverse
    for (int  j=0; j<r; i++)
    {
    		for (int i=0; i<r;i++)
    		{
            	bootsCONSTANT(&s,0,bk);
              {
             	    for (int k = 0; k < r; k++)
             		{
             			bootsMUX(&s, &a[i][j], &a[j][k], &s, bk);
             			bootsMUX(&a[j][k], &a[i][j], &a[i][k], &a[j][k],bk);
             			bootsMUX(&a[i][k], &a[i][j], &s, &a[i][k], bk);
             			
                        bootsMUX(&s, &a[i][j], &f[j][k], &s, bk);
             			bootsMUX(&f[j][k], &a[i][j], &f[i][k], &f[j][k], bk);
             			bootsMUX(&f[i][k], &a[i][j], &s, &f[i][k], bk);
             		}

                    
                    HomDiv(t, A, 1, a[j,j], length,bk);

             		for (int k=0;k <N; k++)
             		{
             			HomMulti(a[j][k], t, a[j][k], length,bk);
             			HomMulti(f[j][k], t, f[j][k], length,bk);
             		}
             		for (int L=0;L<N;L++)
             		{
             			//bootsCONSTANT(&sel, L ~= j, bk);
             			bootsCONSTANT(&t,0,bk);
             			HomTwosCompilment(t,a[L][j],length,bk);
                   
             			for (int k=0;k<N;k++)
             			{
             				HomMulti(c,t,a[j][k],length,bk);
                            HomMulti(d,t,f[j][k],length,bk);
             				HomAdd(a[L][k],a[L][k],c,length,bk);
          				  	HomAdd(f[L][k],f[L][k],d,length,bk);
             			}	
             		}	

              } 		
    		}
     }
   
    for(int i = 0; i < length; i++){
        for(int j = 0; j< length; j++){
        bootsCOPY(&res[i][j], &f[L][k], bk);
        }
    }    
//delete bootstrapping ciphertext arrays
	delete_gate_bootstrapping_ciphertext_array(length, c);
	delete_gate_bootstrapping_ciphertext_array(length, d);
	delete_gate_bootstrapping_ciphertext_array(length, L);
	delete_gate_bootstrapping_ciphertext_array(length, K);
	delete_gate_bootstrapping_ciphertext_array(length, f);
	delete_gate_bootstrapping_ciphertext_array(length, A);
}    
