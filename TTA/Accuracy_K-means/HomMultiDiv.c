#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>







///////////////////////////////   new  multiplication     ////////////////////////////
//res = a * b
//with addition(length * length = length)

void HomMulti(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	
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
	

	for(int i = 1; i < length-1; i++){

		HomRShift(C, A, length, i, bk);

		for(int j = 0; j < length; j++)
			bootsAND(&D[j], &C[j], &B[i], bk);

		HomAdd(AA, AA, D, length, bk);}


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


///////////////////////////////     division. new version     ////////////////////////////
// a / (-b)  =>  (-a) / b
// (-a) / (-b)  =>  a / b
//length * length = length
//res1 = Quotient
//res2 = Reminder

void HomDiv(LweSample* res1, LweSample* res2, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	
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
		


	for(int i = 0; i < length; i++){
//		bootsCOPY(&Q[i], &QR[i], bk);
		bootsCOPY(&res1[i], &QR[i], bk);
		bootsCOPY(&res2[i], &QR[length+i], bk);}



	HomTwosCompliment(C, Q, length, bk);
	
	bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
	bootsNOT(&temp[1], &temp[0], bk);

	for(int i = 0; i < length; i++){
		bootsAND(&Q[i], &Q[i], &temp[1], bk);
		bootsAND(&C[i], &C[i], &temp[0], bk);}

//	HomAdd(res1, Q, C, length, bk);
/*
	for(int i = 0; i < length; i++){
		bootsCONSTANT(&E0[i], 0, bk);}

	//printf("666666666\n");
	HomEqui(&temp[0], res2, E0, length, bk);

	//printf("777777777\n");
	bootsNOT(&E0[0], &temp[0], bk);
	//printf("8\n");

	HomSubt(res1, Q, E0, length, bk);
	//printf("0\n");

*/
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


///////////////////////////////     Division(Real Number)     ////////////////////////////

void HomDivReal(LweSample* res, const LweSample* Q, const LweSample* M, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

// K1
LweSample* K1 = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsCOPY(K1, &Q[length-1], bk);

// K2
LweSample* K2 = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsCOPY(K2, &M[length-1], bk);

// KR
LweSample* KR = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsXOR(KR, K1, K2, bk);

// Q = HomAbs(Q, length)
HomAbs(Q, Q, length, bk);

// M = HomAbs(M, length)
HomAbs(M, M, length, bk);

// A = real_to_binary(0, length) (4)
LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
for(int i = 0; i < length; i++)
bootsCONSTANT(&A[i], 0, bk);

// Before iteration, we need declaration of LweSample: QA, D, ND, AM, AR, AS (6)
LweSample* QA = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
LweSample* D = new_gate_bootstrapping_ciphertext_array(1, bk->params); // D: determinant of sign
LweSample* ND = new_gate_bootstrapping_ciphertext_array(1, bk->params); // ND: same use as to D
LweSample* AM = new_gate_bootstrapping_ciphertext_array(length, bk->params); // AM: A - M
LweSample* AR = new_gate_bootstrapping_ciphertext_array(length, bk->params);
LweSample* AS = new_gate_bootstrapping_ciphertext_array(length, bk->params);


//////// Iteration in Divison
// for(int i=1; i <= length + (length / 2) - 1; i++){
for(int i=1; i <= length + (length / 2); i++){
// QA = [Q, A]
for(int j=0; j < length; j++)
bootsCOPY(&QA[j], &Q[j], bk);
for(int j=length; j < 2*length; j++)
bootsCOPY(&QA[j], &A[j - length], bk);

// QA = HomRShift(QA, 1, 2*length)
HomRShift(QA, QA, 2*length, 1, bk);

// Take length number of A from right-shifted [Q A]
for(int j=length; j < 2*length; j++)
bootsCOPY(&A[j-length], &QA[j], bk);

// Take length number of Q from right-shifted [Q A]
for(int j=0; j < length; j++)
bootsCOPY(&Q[j], &QA[j], bk);

// Comparison: D = A < M ? 1 : 0
HomCompS(D, A, M, length, bk);
// ND = not(D)
bootsNOT(ND, D, bk);
// Q(0) = ND
bootsCOPY(&Q[0], ND, bk);
// AM = A - M
HomSubt(AM, A, M, length, bk);

// AR = ND AND (A - M)
for(int j=0; j < length; j++)
bootsAND(&AR[j], ND, &AM[j], bk);

// AS = D AND D
for(int j=0; j < length; j++)
bootsAND(&AS[j], D, &A[j], bk);

// A = AR + AS
HomAdd(A, AR, AS, length, bk);

// QA = [Q, A]
for(int j=0; j < length; j++)
bootsCOPY(&QA[j], &Q[j], bk);
for(int j=length; j < 2*length; j++)
bootsCOPY(&QA[j], &A[j - length], bk);
}

// Q of [Q, A]
for(int i=0; i < length; i++)
bootsCOPY(&Q[i], &QA[i], bk);
// A of [Q, A]
for(int i=length; i < 2*length; i++)
bootsCOPY(&A[i-length], &QA[i], bk);

//// Considering Singed bit of Q and M
LweSample* NKR = new_gate_bootstrapping_ciphertext_array(1, bk->params);

// NKR = not(KR)
bootsNOT(NKR, KR, bk);

// NQ = HomTwosCompliment(Q, length)
LweSample* NQ = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomTwosCompliment(NQ, Q, length, bk);

// Adding both positive case (QP) and negative case (QN) (4)
LweSample* QP = new_gate_bootstrapping_ciphertext_array(length, bk->params);
LweSample* QN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

// QP
for(int i=0; i < length; i++)
bootsAND(&QP[i], NKR, &Q[i], bk);
// QN
for(int i=0; i < length; i++)
bootsAND(&QN[i], KR, &NQ[i], bk);

// res = QN + QP
HomAdd(res, QP, QN, length, bk);

// delete LweSample bootstrapping arrays (14)
delete_gate_bootstrapping_ciphertext_array(1, K1);
delete_gate_bootstrapping_ciphertext_array(1, K2);
delete_gate_bootstrapping_ciphertext_array(1, KR);
delete_gate_bootstrapping_ciphertext_array(length, A);

delete_gate_bootstrapping_ciphertext_array(2*length, QA);
delete_gate_bootstrapping_ciphertext_array(1, D);
delete_gate_bootstrapping_ciphertext_array(1, ND);
delete_gate_bootstrapping_ciphertext_array(length, AM);
delete_gate_bootstrapping_ciphertext_array(length, AR);
delete_gate_bootstrapping_ciphertext_array(length, AS);

delete_gate_bootstrapping_ciphertext_array(1, NKR);
delete_gate_bootstrapping_ciphertext_array(length, NQ);
delete_gate_bootstrapping_ciphertext_array(length, QP);
delete_gate_bootstrapping_ciphertext_array(length, QN);

}

/*
///////////////////////////////     Division(Real Number)     ////////////////////////////

void HomDivReal(LweSample* res, const LweSample* Q, const LweSample* M, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

// K1
LweSample* K1 = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsCOPY(K1, &Q[length-1], bk);

// K2
LweSample* K2 = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsCOPY(K2, &M[length-1], bk);

// KR
LweSample* KR = new_gate_bootstrapping_ciphertext_array(1, bk->params);
bootsXOR(KR, K1, K2, bk);

// Q = HomAbs(Q, length)
HomAbs(Q, Q, length, bk);

// M = HomAbs(M, length)
HomAbs(M, M, length, bk);

// A = real_to_binary(0, length) (4)
LweSample* A = new_gate_bootstrapping_ciphertext_array(length, bk->params);
for(int i = 0; i < length; i++)
bootsCONSTANT(&A[i], 0, bk);

// Before iteration, we need declaration of LweSample: QA, D, ND, AM, AR, AS (6)
LweSample* QA = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
LweSample* D = new_gate_bootstrapping_ciphertext_array(1, bk->params); // D: determinant of sign
LweSample* ND = new_gate_bootstrapping_ciphertext_array(1, bk->params); // ND: same use as to D
LweSample* AM = new_gate_bootstrapping_ciphertext_array(length, bk->params); // AM: A - M
LweSample* AR = new_gate_bootstrapping_ciphertext_array(length, bk->params);
LweSample* AS = new_gate_bootstrapping_ciphertext_array(length, bk->params);


//////// Iteration in Divison
for(int i=1; i <= length + (length / 2) - 1; i++){

// QA = [Q, A]
for(int j=0; j < length; j++)
bootsCOPY(&QA[j], &Q[j], bk);
for(int j=length; j < 2*length; j++)
bootsCOPY(&QA[j], &A[j - length], bk);

// QA = HomRShift(QA, 1, 2*length)
HomRShift(QA, QA, 2*length, 1, bk);

// Take length number of A from right-shifted [Q A]
for(int j=length; j < 2*length; j++)
bootsCOPY(&A[j-length], &QA[j], bk);

// Take length number of Q from right-shifted [Q A]
for(int j=0; j < length; j++)
bootsCOPY(&Q[j], &QA[j], bk);

// Comparison: D = A < M ? 1 : 0
HomCompS(D, A, M, length, bk);
// ND = not(D)
bootsNOT(ND, D, bk);
// Q(0) = ND
bootsCOPY(&Q[0], ND, bk);
// AM = A - M
HomSubt(AM, A, M, length, bk);

// AR = ND AND (A - M)
for(int j=0; j < length; j++)
bootsAND(&AR[j], ND, &AM[j], bk);

// AS = D AND D
for(int j=0; j < length; j++)
bootsAND(&AS[j], D, &A[j], bk);

// A = AR + AS
HomAdd(A, AR, AS, length, bk);

// QA = [Q, A]
for(int j=0; j < length; j++)
bootsCOPY(&QA[j], &Q[j], bk);
for(int j=length; j < 2*length; j++)
bootsCOPY(&QA[j], &A[j - length], bk);
}

// Q of [Q, A]
for(int i=0; i < length; i++)
bootsCOPY(&Q[i], &QA[i], bk);
// A of [Q, A]
for(int i=length; i < 2*length; i++)
bootsCOPY(&A[i-length], &QA[i], bk);

//// Considering Singed bit of Q and M
LweSample* NKR = new_gate_bootstrapping_ciphertext_array(1, bk->params);

// NKR = not(KR)
bootsNOT(NKR, KR, bk);

// NQ = HomTwosCompliment(Q, length)
LweSample* NQ = new_gate_bootstrapping_ciphertext_array(length, bk->params);
HomTwosCompliment(NQ, Q, length, bk);

// Adding both positive case (QP) and negative case (QN) (4)
LweSample* QP = new_gate_bootstrapping_ciphertext_array(length, bk->params);
LweSample* QN = new_gate_bootstrapping_ciphertext_array(length, bk->params);

// QP
for(int i=0; i < length; i++)
bootsAND(&QP[i], NKR, &Q[i], bk);
// QN
for(int i=0; i < length; i++)
bootsAND(&QN[i], KR, &NQ[i], bk);

// res = QN + QP
HomAdd(res, QP, QN, length, bk);

// delete LweSample bootstrapping arrays (14)
delete_gate_bootstrapping_ciphertext_array(1, K1);
delete_gate_bootstrapping_ciphertext_array(1, K2);
delete_gate_bootstrapping_ciphertext_array(1, KR);
delete_gate_bootstrapping_ciphertext_array(length, A);

delete_gate_bootstrapping_ciphertext_array(2*length, QA);
delete_gate_bootstrapping_ciphertext_array(1, D);
delete_gate_bootstrapping_ciphertext_array(1, ND);
delete_gate_bootstrapping_ciphertext_array(length, AM);
delete_gate_bootstrapping_ciphertext_array(length, AR);
delete_gate_bootstrapping_ciphertext_array(length, AS);

delete_gate_bootstrapping_ciphertext_array(1, NKR);
delete_gate_bootstrapping_ciphertext_array(length, NQ);
delete_gate_bootstrapping_ciphertext_array(length, QP);
delete_gate_bootstrapping_ciphertext_array(length, QN);

}

*/
