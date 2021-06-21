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


/////////////////////////////   Multiplication //////////////////////////////////////

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

void HomRealP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk){

	// LweSample* temp = new_gate_bootstrapping_ciphertext_array(5, bk->params);

	int16_t plain = num;

	for(int i = 0; i < (length/2); i++)
		bootsCONSTANT(&res[i], 0, bk);
	for(int i = 0; i < (length/2); i++)
		bootsCONSTANT(&res[i+(length/2)], (plain>>i)&1, bk);

	// delete_gate_bootstrapping_ciphertext_array(1, temp);
}


/////////////////////////////////////////////Linear Regression Normal Equation method ////////////////////////////////////////////////////
void HomLinRegNE(LweSample* res[2], LweSample **x, LweSample **y, const int N, const int length, const TFheGateBootstrappingCloudKeySet* bk){
	LweSample* sumx = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumxy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* sumx2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a0 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* a1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	

////initialization of intercept(a0) and slope(a1)
	//a0 is an intercept
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a0[i], 0, bk);
		bootsCONSTANT(&sumx[i], 0, bk);
		bootsCONSTANT(&sumy[i], 0, bk);
		bootsCONSTANT(&sumxy[i], 0, bk);
		bootsCONSTANT(&sumx2[i], 0, bk);
	}
	

	//a1 is a slope 
	for(int i=0;i<length;i++)
	{
		bootsCONSTANT(&a1[i], 0, bk);
	}
	

	LweSample *M1[N], *M2[N];
	for(int i=0;i<N;i++)
	{
		M1[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		M2[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	}

	// LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	
	LweSample* A3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A4 = new_gate_bootstrapping_ciphertext_array(length, bk->params);	
	LweSample* A5 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* A6 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B1 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* B3 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	LweSample* EN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	
	for(int i=0; i<length; i++){
		bootsCONSTANT(&A1[i], 0, bk);
		bootsCONSTANT(&A2[i], 0, bk);
		bootsCONSTANT(&A3[i], 0, bk);
		bootsCONSTANT(&A4[i], 0, bk);
		bootsCONSTANT(&A5[i], 0, bk);
		bootsCONSTANT(&A6[i], 0, bk);
		bootsCONSTANT(&B1[i], 0, bk);
		bootsCONSTANT(&B2[i], 0, bk);
		bootsCONSTANT(&B3[i], 0, bk);
		bootsCONSTANT(&EN[i], 0, bk);
	}

	HomRealP2C(EN, N, length, bk);

// //Calculation Start///
	for(int i =0;i<N;i++)
	{
		HomAdd(sumx, sumx, x[i], length, bk); //sumx = sumx + x(i)
		HomMultiReal(M1[i],x[i], x[i], length,bk); //x(i)*x(i)
		HomAdd(sumx2, sumx2, M1[i], length, bk); //sumx^2 = sumx^2+x(i)*x(i)
		HomAdd(sumy, sumy, y[i], length, bk); //sumy = sumy+y(i)
		HomMultiReal(M2[i], x[i], y[i], length, bk); //x(i)*y(i)
		HomAdd(sumxy, sumxy, M2[i], length, bk); //sumxy = sumxy+x(i)*y(i)		
	}	
	

	HomMultiReal(A1, sumx2, sumy, length, bk); //sumx^2*sumy
	HomMultiReal(A2, sumx, sumxy, length, bk); //sumx*sumxy
	HomMultiReal(A3, EN, sumx2, length, bk); //N*sumx^2
	HomMultiReal(A4, sumx, sumx, length, bk); //sumx*sumx
	HomMultiReal(A5, EN, sumxy, length, bk); //N*sumxy
	HomMultiReal(A6, sumx, sumy, length, bk); //sumx*sumy
	HomSubt(B1, A1, A2, length, bk); //sumx^2*sumy-sumx*sumxy
	HomSubt(B2, A3, A4, length, bk); //N*sumx^2-sumx*sumx
	HomSubt(B3, A5, A6, length, bk); //N*sumxy-sumx*sumy
	HomRealDiv(a0, B1, B2, length, bk); //sumx^2*sumy-sumx*sumxy/N*sumx^2-sumx*sumx
	HomRealDiv(a1, B3, B2, length, bk); //N*sumxy-sumx*sumy/N*sumx^2-sumx*sumx

	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[0][i], &a0[i], bk);
	}

	for(int i=0;i<length;i++)
	{
		bootsCOPY(&res[1][i], &a1[i], bk);
	}


	delete_gate_bootstrapping_ciphertext_array(length, sumx);
	delete_gate_bootstrapping_ciphertext_array(length, sumy);
	delete_gate_bootstrapping_ciphertext_array(length, sumxy);
	delete_gate_bootstrapping_ciphertext_array(length, sumx2);
	delete_gate_bootstrapping_ciphertext_array(length, a0);
	delete_gate_bootstrapping_ciphertext_array(length, a1);
	
	for(int i=0;i<N;i++)
	{
		delete_gate_bootstrapping_ciphertext_array(length, M1[i]);
		delete_gate_bootstrapping_ciphertext_array(length, M2[i]);	
	}	

	// delete_gate_bootstrapping_ciphertext_array(length, A);
	delete_gate_bootstrapping_ciphertext_array(length, A1);
	delete_gate_bootstrapping_ciphertext_array(length, A2);
	delete_gate_bootstrapping_ciphertext_array(length, A3);
	delete_gate_bootstrapping_ciphertext_array(length, A4);
	delete_gate_bootstrapping_ciphertext_array(length, A5);
	delete_gate_bootstrapping_ciphertext_array(length, A6);
	delete_gate_bootstrapping_ciphertext_array(length, B1);
	delete_gate_bootstrapping_ciphertext_array(length, B2);
	delete_gate_bootstrapping_ciphertext_array(length, B3);
	delete_gate_bootstrapping_ciphertext_array(length, EN);
}
