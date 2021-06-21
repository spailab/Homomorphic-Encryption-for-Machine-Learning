#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>


int main() {

    printf("Step3 start\n");

    // reads the cloud key from file //
    FILE* secret_key = fopen("secret.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);
 
    // if necessary, the params are inside the key //
    const TFheGateBootstrappingParameterSet* params = key->params;

    // initial value //
    int res_length = 10;
    int res_number = 3;
    int N = 64;
    int k = 3;
    
    LweSample* answer[res_number], *M[N];

    for(int i = 0; i < res_number; i++){
	    answer[i] = new_gate_bootstrapping_ciphertext_array(res_length, params);}
    for(int i = 0; i < N; i++){
	    M[i] = new_gate_bootstrapping_ciphertext_array(k, params);}
    
    //import the res_number x res_length ciphertexts from the answer file
    FILE* answer_data = fopen("answer.data","rb");
    for(int i = 0; i < res_number; i++){
	    for (int j = 0; j < res_length; j++){
	        import_gate_bootstrapping_ciphertext_fromFile(answer_data, &answer[i][j], params);}}
    fclose(answer_data);
    
    FILE* label_data = fopen("label.data","rb");
    for(int i = 0; i < N; i++){
	    for (int j = 0; j < k; j++){
	        import_gate_bootstrapping_ciphertext_fromFile(label_data, &M[i][j], params);}}
    fclose(label_data);

    
    // int_answer initial value setting //
    int16_t int_answer[res_number], m[N][k];

    for (int i = 0; i < res_number; i++){
        int_answer[i] = 0;
    }

    // decrypt and rebuild the plaintext answer //
    for (int i = 0; i < res_number; i++){
    	for (int j = 0; j < res_length; j++) 
	    {
        	int ai = bootsSymDecrypt(&answer[i][j], key);
	        int_answer[i] |= (ai<<j);
	    }
    }
    for (int i = 0; i < N; i++){
    	for (int j = 0; j < k; j++) 
	    {
        	m[i][j] = bootsSymDecrypt(&M[i][j], key);
 //       	m[i][j] |= (mm<<j);
	    }
    }

    int plaintext1[N];

    FILE* data = fopen("Kmeansdata.txt","rb");
    for(int i = 0; i < N; i++)
    {
        fscanf(data, "%d ", &plaintext1[i]);
    }
    fclose(data);

    // print result //
    printf("/////////// data and it's calculated label with initial clusters  ///////// \n");
    for (int i = 0; i < N; i++){
	    printf("/// %d-th data : %d, and label : %d %d %d \n",i+1, plaintext1[i], m[i][0], m[i][1], m[i][2]);}

    printf("//////////////////////////////////////////////////////////////////////////// \n");

    for (int i = 0; i < res_number; i++){
	    printf("new cluster mean u[%d] is: %d \n",i, int_answer[i]);}





    // save result to file //
    FILE* KmeansCipherResult = fopen("KmeansCipherResult.txt","w");
    fprintf(KmeansCipherResult, "%d %d %d \n", int_answer[0], int_answer[1], int_answer[2]);
    fclose(KmeansCipherResult);
    

    // clean up all pointers //
    for (int i = 0; i < res_number; i++){
        delete_gate_bootstrapping_ciphertext_array(res_length, answer[i]);}

    delete_gate_bootstrapping_secret_keyset(key);
}
