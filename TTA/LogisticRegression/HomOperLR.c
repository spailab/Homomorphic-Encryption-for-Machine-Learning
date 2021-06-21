#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


///////////////////////////////     Addition     ////////////////////////////
//res = a + b

void HomAdd(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);	    
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);	    

	bootsCONSTANT(&c[0], 0, bk);
    
	for(int i = 0; i < length -1; i++){
		bootsXOR(&temp[0], &a[i], &b[i], bk);
		bootsAND(&temp[1], &a[i], &b[i], bk);
		bootsXOR(&res[i], &temp[0], &c[i], bk);
		bootsAND(&temp[0], &temp[0], &c[i], bk);
		bootsOR(&c[i+1], &temp[0], &temp[1], bk);
	}

	bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
	bootsXOR(&res[length-1], &temp[0], &c[length-1], bk);

	delete_gate_bootstrapping_ciphertext_array(length, c);    
	delete_gate_bootstrapping_ciphertext_array(2, temp);    
}


///////////////////////////////     2's compliment     ////////////////////////////
//res = -a

void HomTwosCompliment(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(1, bk->params);	    
	LweSample* b = new_gate_bootstrapping_ciphertext_array(length, bk->params);	    

	bootsCONSTANT(&b[0], 1, bk);
    

	for(int i = 0; i < length - 2; i++){
		bootsNOT(&temp[0], &a[i], bk);
		bootsXOR(&res[i], &temp[0], &b[i], bk);
		bootsAND(&b[i+1], &temp[0], &b[i], bk);}

	bootsNOT(&temp[0], &a[length-2], bk);
	bootsXOR(&res[length-2], &temp[0], &b[length-2], bk);

	bootsNOT(&res[length-1], &a[length-1], bk);
	

	delete_gate_bootstrapping_ciphertext_array(length, b);    
	delete_gate_bootstrapping_ciphertext_array(1, temp);    
}





///////////////////////////////     subtract     ////////////////////////////
//res = a - b

void HomSubt(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

    LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);       
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);       

    bootsCONSTANT(&c[0], 0, bk);
    
    for(int i = 0; i < length -1; i++){
        bootsXOR(&temp[0], &a[i], &b[i], bk);
        bootsANDNY(&temp[1], &a[i], &b[i], bk);
        bootsXOR(&res[i], &temp[0], &c[i], bk);
        bootsANDNY(&temp[0], &temp[0], &c[i], bk);
        bootsOR(&c[i+1], &temp[1], &temp[0], bk);}

    bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
    bootsXOR(&res[length-1], &temp[0], &c[length-1], bk);

    delete_gate_bootstrapping_ciphertext_array(length, c);   
    delete_gate_bootstrapping_ciphertext_array(2, temp);   
} 

 
///////////////////////////////     right shift     ////////////////////////////

void HomRShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk) {

    bootsCOPY(&res[length-1], &a[length-1], bk);
    for(int i = length-2; i > k-1; i--){
        bootsCOPY(&res[i], &a[i-k], bk);}
    for(int i = 0; i < k; i++){
        bootsCONSTANT(&res[i], 0, bk);}


} 



///////////////////////////////     left shift     ////////////////////////////

void HomLShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk) {

	for(int i = 0; i < length - k; i++){
		bootsCOPY(&res[i], &a[i+k], bk);}
	for(int i = length-k; i < length; i++){
		bootsCOPY(&res[i], &a[length-1], bk);}
}



///////////////////////////////     equivalent or not     ////////////////////////////
// if a = b then res = E(1)
// else res = E(0)

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



///////////////////////////////     small comparison     ////////////////////////////
// if a < b then res = E(1)
// else res = E(0)

void HomCompS(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);
	
	bootsCONSTANT(&temp[0], 0, bk);
	
	for(int i = 0; i < length; i++){
		bootsXNOR(&temp[1], &a[i], &b[i], bk);
		bootsMUX(&temp[0], &temp[1], &temp[0], &b[i], bk);}

	bootsCOPY(&res[0], &temp[0], bk);
	delete_gate_bootstrapping_ciphertext_array(2, temp);
}

///////////////////////////////     absolute value     ////////////////////////////
// if a < b then res = a

void HomAbs(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	LweSample* na = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	HomTwosCompliment(na, a, length, bk);
	
	for(int i = 0; i < length; i++){
		bootsMUX(&res[i], &a[length-1], &na[i], &a[i], bk);}

	delete_gate_bootstrapping_ciphertext_array(length, na);
}




///////////////////////////////   new  multiplication     ////////////////////////////

void HomMultiReal(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* AA = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* C = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* D = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* F = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	
	HomAbs(A, a, length, bk);
	HomAbs(B, b, length, bk);
	for(int i = 0; i < length; i++){
		bootsAND(&AA[i], &A[i], &B[0], bk);}
	HomLShift(AA, AA, length, length/2, bk);	


	
	for(int i = 1; i < length-1; i++){
		if(i < length/2){
			HomLShift(C, A, length, length/2-i, bk);

			for(int j = 0; j < length; j++)
				bootsAND(&D[j], &C[j], &B[i], bk);

			HomAdd(AA, AA, D, length, bk);
		}
		else if(i == length/2){
			for(int j = 0; j < length; j++)
				bootsAND(&D[j], &A[j], &B[i], bk);

			HomAdd(AA, AA, D, length, bk);
		}

		else {
			HomRShift(C, A, length, i-length/2, bk);
			for(int j = 0; j < length; j++){
				bootsAND(&D[j], &C[j], &B[i], bk);}

			HomAdd(AA, AA, D, length, bk);
		}

	}
	bootsCONSTANT(&AA[length-1], 0, bk);

	HomTwosCompliment(D, AA, length, bk);
	
	bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
	bootsNOT(&temp[1], &temp[0], bk);

	for(int i = 0; i < length; i++){
		bootsAND(&E[i], &AA[i], &temp[1], bk);
		bootsAND(&F[i], &D[i], &temp[0], bk);}

	HomAdd(res, E, F, length, bk);

	delete_gate_bootstrapping_ciphertext_array(2, temp);
	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, AA);
	delete_gate_bootstrapping_ciphertext_array(length, B);
	delete_gate_bootstrapping_ciphertext_array(length, C);
	delete_gate_bootstrapping_ciphertext_array(length, D);
	delete_gate_bootstrapping_ciphertext_array(length, E);
	delete_gate_bootstrapping_ciphertext_array(length, F);
}


///////////////////////////////     real division.     ////////////////////////////
// a / (-b)  =>  (-a) / b
// (-a) / (-b)  =>  a / b
//length * length = length
//res1 = Quotient

void HomRealDiv(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(3, bk->params);
	LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* QR = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
	LweSample* D = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* C = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* Q = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* DD = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* R = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	
	HomAbs(A, a, length, bk);
	HomAbs(B, b, length, bk);
	//printf("absolute value clear\n");

	for(int i = 0; i < length; i++){
		bootsCOPY(&QR[i], &A[i], bk);
		bootsCOPY(&D[i], &B[i], bk);
		bootsCONSTANT(&QR[length + i], 0, bk);}
	

	HomRShift(QR, QR, 2*length, 1, bk);
//	printf("stting clear\n");
	for(int s = 1; s < length; s++){
		HomRShift(QR, QR, 2*length, 1, bk);

		for(int i = 0; i < length; i++){
			bootsCOPY(&R[i], &QR[length+i], bk);}

		HomCompS(&temp[0], R, D, length, bk);
		bootsNOT(&temp[1], &temp[0], bk);
		bootsCOPY(&QR[0], &temp[1], bk);

		for(int i = 0; i < length; i++){
			bootsAND(&DD[i], &D[i], &temp[1], bk);}

		HomSubt(R, R, DD, length, bk);

		for(int i = 0; i < length; i++){
			bootsCOPY(&QR[length+i], &R[i], bk);}
		
//		printf("%d loop clear\n",s);
	}
		


	for(int s = length; s < length*3/2; s++){
		HomRShift(QR, QR, 2*length, 1, bk);
		HomRShift(R, R, length, 1, bk);

		HomCompS(&temp[0], R, D, length, bk);
		bootsNOT(&temp[1], &temp[0], bk);
		bootsCOPY(&QR[0], &temp[1], bk);

		for(int i = 0; i < length; i++){
			bootsAND(&DD[i], &D[i], &temp[1], bk);}

		HomSubt(R, R, DD, length, bk);

		
//		printf("%d loop clear\n",s);
	}


	for(int i = 0; i < length; i++){
		bootsCOPY(&Q[i], &QR[i], bk);}
//		bootsCOPY(&res[i], &QR[i], bk);}



	HomTwosCompliment(C, Q, length, bk);
	
	bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
	bootsNOT(&temp[1], &temp[0], bk);

	for(int i = 0; i < length; i++){
		bootsAND(&Q[i], &Q[i], &temp[1], bk);
		bootsAND(&C[i], &C[i], &temp[0], bk);}

	HomAdd(res, Q, C, length, bk);
	
	
	
	
	delete_gate_bootstrapping_ciphertext_array(3, temp);
	delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, B);
	delete_gate_bootstrapping_ciphertext_array(2*length, QR);
	delete_gate_bootstrapping_ciphertext_array(length, D);
	delete_gate_bootstrapping_ciphertext_array(length, DD);
	delete_gate_bootstrapping_ciphertext_array(length, Q);
	delete_gate_bootstrapping_ciphertext_array(length, R);
	delete_gate_bootstrapping_ciphertext_array(length, C);
	delete_gate_bootstrapping_ciphertext_array(length, E0);
}

///////////////////////////////     plaintext to ciphertext     ////////////////////////////

void HomP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	// LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);

	
	int8_t plain = num;

	for(int i = 0; i < length; i++)
		bootsCONSTANT(&res[i], (plain>>i)&1, bk);


	// delete_gate_bootstrapping_ciphertext_array(1, temp);
}

///////////////////////////////     plaintext to ciphertext     ////////////////////////////

void HomRealP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	// LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);

	int16_t plain = num;

	for(int i = 0; i < (length/2); i++)
		bootsCONSTANT(&res[i], 0, bk);
	for(int i = 0; i < (length/2); i++)
		bootsCONSTANT(&res[i+(length/2)], (plain>>i)&1, bk);

	// delete_gate_bootstrapping_ciphertext_array(1, temp);
}

///////////////////////////////     Positive Exponential function ver.3(revised)      ////////////////////////////
// exponential 2 of b
// binary exponentiation

void HomBinExpP(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	/// 1) integer part of x : int_x -> 3 out of 8 xxxs 0000
	LweSample* int_x = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = (length/2); i < length; i++){ 
		bootsCOPY(&int_x[i-(length/2)], &b[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&int_x[i], 0, bk);
	}

	/// 2) decimal part of x : xr -> xxxx 0000
	LweSample* xr = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < (length/2); i++){ // 4 out of 8
		bootsCOPY(&xr[i], &b[i], bk);
	}
	for(int i = (length/2); i < length; i++){
		bootsCONSTANT(&xr[i], 0, bk);
	}

	/// temp, temp2
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&temp[i], 0 , bk); // 3 out of 8, but 0000 0000
		bootsCONSTANT(&temp2[i], 0 , bk); // 4 out of 8, but 0000 0000
	}

	/// 3) k : 1 (shift) 1000 0000
	LweSample* k = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	bootsCONSTANT(&k[0], 1, bk); // 3 out of 8
	for(int i = 1; i < length; i++){
		bootsCONSTANT(&k[i], 0, bk);
	}

	// lwe samples
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
		///// E[i] /////		
		HomP2C(enci, i, length, bk); 

		///// msb part /////
		HomEqui(&output[i], enci, int_x, length, bk); 
		HomRShift(msk, k, length, i, bk); // 3 out of 8
				
		for(int j = 0; j < length/2 - 1; j++){
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
	
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final_pos[i], bk);
	}
	

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
	
}

///////////////////////////////     Negative Exponential function ver.3     ////////////////////////////
// exponential 2 of b
// binary exponentiation

void HomBinExpN(LweSample* res, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	/// absolute value of x
	LweSample* avx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomAbs(avx, b, length, bk);

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
	
	for(int i = 0; i < length; i++){
		bootsCOPY(&res[i], &final_neg[i], bk);
	}

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
}

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

/////////////////////////////////// Sigmoid Function ///////////////////////////////////////////////////////////
void HomSig(LweSample* res, const LweSample* x, const int length, const TFheGateBootstrappingCloudKeySet* bk){

/////for sigmoid function, we need -x for 1/(1+exp(-x))
// -x = 2's compliment
LweSample* mx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomTwosCompliment(mx, x, length, bk);

////exp(-x) using HomBinExp
LweSample* exp_mx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomExp(exp_mx, mx, length, bk);
////1+exp(-x)
//make 1 for addition
LweSample* one = new_gate_bootstrapping_ciphertext_array(length, bk->params);
for(int i=0;i<length;i++)
bootsCONSTANT(&one[i], 0, bk);
bootsCONSTANT(&one[length/2], 1, bk);

//add 1 to exp(-x) that is one + exp_mx and store it in dec
LweSample* dec = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomAdd(dec, one, exp_mx, length, bk);
////Division of dec by 1 is our result
//remainder not needed : A
LweSample* outcome = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomRealDiv(outcome, one, dec, length, bk);
//copy updated result of outcome to res
for(int i=0;i<length;i++)
bootsCOPY(&res[i], &outcome[i], bk);

delete_gate_bootstrapping_ciphertext_array(length, mx);
delete_gate_bootstrapping_ciphertext_array(length, exp_mx);
delete_gate_bootstrapping_ciphertext_array(length, one);
delete_gate_bootstrapping_ciphertext_array(length, dec);
delete_gate_bootstrapping_ciphertext_array(length, outcome);
}


///////////////////////////////////////////////Logistic Regression////////////////////////////////////////////////////////////////
void HomLogReg(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	LweSample* beta0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* beta1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//// Initialization : parameter beta0, beta1 set to binary zero array
	// beta0 = 0.25, beta1 = 0.25
	for(int i=0;i<length;i++){
		bootsCONSTANT(&beta0[i], 0, bk);
		bootsCONSTANT(&beta1[i], 0, bk);
	}
	bootsCONSTANT(&beta0[length/2 - 2], 1, bk);
	bootsCONSTANT(&beta1[length/2 - 2], 1, bk);

	//////Algorithm : Logistic Regression Gradient Descent
	//iteration : 3 times
	int iter = 5;
	// 1 / 1 + exp(-BX) ::: BX = beta0*x1 + beta1*x2
	// dJ/dbeta1 = 1/N * sum(sig(x_i)) * x_i
	// 1/N = (E1)/EN = E1_EN 
	LweSample* EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* E1_EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//Division supplementary : not used
	// LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomRealP2C(EN, N, length, bk);
	HomRealP2C(E1, 1, length, bk);
	HomRealDiv(E1_EN, E1, EN, length, bk);

	//////gradient descent part 1 : construct dJ/dbeta0 and dJ/ dbeta1
	// 1) dJ/dbeta0 = grad_b0 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	// 2) dJ/dbeta0 = grad_b1 ; just in case of seg. fault, the length is 2*length
	LweSample* grad_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	//// dJ/dbeta1 = (1/N) * summation{sig(-beta0 + beta1 * x_i)} * x_i
	LweSample *L1[N], *L2[N], *L3[N], *L4[N], *L5[N], *L6[N];
	for(int i=0;i<N;i++)
	{
		L1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L3[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L4[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L5[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		L6[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}
	
	/// a-1) S1 = summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] } = L5[0] + L5[1] + ... + L5[N-1]
	/// a-2) S2 = summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] * x1 } = L6[0] + L6[1] + ... + L6[N-1]	
	LweSample* S1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* S2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	//LR(Learning Rate)= 0.01 (binary) = 0.25 (decimal)
	LweSample* LR = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	for(int i=0;i<length;i++)
		bootsCONSTANT(&LR[i], 0, bk);

	bootsCONSTANT(&LR[(length/2) - 2], 1, bk);

	int cycle = 1;
	printf("Big cycle %d\n", cycle);

	/// ======================================================== ///
	/// ============= iteration cycle; update loop ============= ///
	/// ======================================================== ///
	for(int k=0;k<iter;k++) 
	{
		//S1, S2 initializes with elements all 0
		//every iteration, initialize S1 and S2 to zero
		for(int i=0;i<length;i++)
		{
			bootsCONSTANT(&S1[i], 0, bk);
			bootsCONSTANT(&S2[i], 0, bk);
		}	

		printf("small loop %d\n", k+1);
		///// start calculating (full batch) stochestic gradient descent algorithm
		for(int i=0;i<N;i++)
		{
			HomMultiReal(L1[i], beta1, x[i], length, bk);	// beta1 * x1
			HomAdd(L2[i], L1[i], beta0, length, bk);	// beta0 + beta1 * x1
			HomTwosCompliment(L3[i], L2[i], length, bk);	// -(beta0 + beta1 * x1)
			HomSig(L4[i], L3[i], length, bk);	// 1 (div) 1 + exp(-(beta0 + beta1 * x1))
			HomSubt(L5[i], L4[i], y[i], length, bk);	// {1 (div) 1 + exp(-(beta0 + beta1 * x1))} - y1
			HomMultiReal(L6[i], L5[i], x[i], length, bk);	// [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) - y1 ] * x1
			HomAdd(S1, S1, L5[i], length, bk);	// summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] } for beta0
			HomAdd(S2, S2, L6[i], length, bk);	// summation { [ 1 (div) 1 + exp(-(beta0 + beta1 * x1)) ] * x1 } for beta1
		}

		// /// b-1) (1/N) * summation() = E1_EN * S1
		HomMultiReal(grad_b0, E1_EN, S1, length, bk);
		// /// b-2) -(1/2N) * summation() = E1_EN * S2
		HomMultiReal(grad_b1, E1_EN, S2, length, bk);

		//////gradient descent part 2 : With learning rate(LR) construct LR*(dJ/dbeta0) and LR*(dJ/dbeta1)
		// update_b0 = LR * (dJ/dbeta0) and update_b1 = LR * (dJ/dbeta1)
		// HomMultiReal(update_b0, LR, grad_b0, length, bk);
		LweSample* update_b0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		LweSample* update_b1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		for(int i=0;i<length;i++){
			bootsCONSTANT(&update_b0[i], 0, bk);
			bootsCONSTANT(&update_b1[i], 0, bk);
		}

		// 0.25 * grad_b0 (or grad_b1)
		HomMultiReal(update_b0, LR, grad_b0, length, bk);
		HomMultiReal(update_b1, LR, grad_b1, length, bk);

		//// substitute with LSHIFT for faster computation: LSHIFT(2) = 1/4(=0.25)
		// HomLShift(update_b0, grad_b0, length, 2, bk);
		// HomLShift(update_b1, grad_b1, length, 2, bk);

		//////gradient descent part 3 : parameter update phase
		//(update)beta0 = beta0 - update_b0, (update)beta1 = beta1 - update_b1 
		HomSubt(beta0, beta0, update_b0, length, bk);
		HomSubt(beta1, beta1, update_b1, length, bk);
		
		printf("Big cycle %d termination\n", cycle);
		cycle += 1;
	}
	///iteration terminates
	//copy updated result of beta0 to result(res0)
	for(int i=0;i<length;i++)
		bootsCOPY(&res[0][i], &beta0[i], bk);
	//copy updated result of beta1 to result(res1)
	for(int i=0;i<length;i++)
		bootsCOPY(&res[1][i], &beta1[i], bk);		
	
	//delete bootstrapping ciphertext arrays
	delete_gate_bootstrapping_ciphertext_array(length, beta0);
	delete_gate_bootstrapping_ciphertext_array(length, beta1);
	delete_gate_bootstrapping_ciphertext_array(length, EN);
	delete_gate_bootstrapping_ciphertext_array(length, E1);
	delete_gate_bootstrapping_ciphertext_array(length, E1_EN);
	delete_gate_bootstrapping_ciphertext_array(length, grad_b0);
	delete_gate_bootstrapping_ciphertext_array(length, grad_b1);
	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, L1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L2[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L3[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L4[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L5[i]);
		delete_gate_bootstrapping_ciphertext_array(length, L6[i]);		
	}
	delete_gate_bootstrapping_ciphertext_array(length, S1);
	delete_gate_bootstrapping_ciphertext_array(length, S2);
	delete_gate_bootstrapping_ciphertext_array(length, update_b0);
	delete_gate_bootstrapping_ciphertext_array(length, update_b1);
}
