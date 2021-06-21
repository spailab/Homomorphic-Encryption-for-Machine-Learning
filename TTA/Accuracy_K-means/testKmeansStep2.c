#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>
#include <HomOper.c>
#include <HomMultiDiv.c>
#include <HomKmeans.c>


int main() {

    printf("Step2 start\n");

    // reads the cloud key from file //
    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    // if necessary, the params are inside the key //
    const TFheGateBootstrappingParameterSet* params = bk->params;

    // initial value // 
    int N = 64; 
    int k = 3;
    int length = 10;
    int res_length = 10;
    int res_number = 3;

    LweSample* ciphertext1[N], *ciphertext2[k], *result[res_number], *M[N];

    for(int i = 0; i < N; i++){
	    ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);}
    for(int i = 0; i < k; i++){
    	ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(length, params);}

    // reads the N x length ciphertexts from the cloud file  = data//
    // reads the k x length ciphertexts from the cloud file  = cluster mean//
    FILE* cloud_data = fopen("cloud.data","rb");
    for (int i = 0; i < N; i++){
	    for (int j = 0; j < length; j++){
		    import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i][j], params);}}
    for (int i = 0; i < k; i++){
    	for (int j = 0; j < length; j++){
	        import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i][j], params);}}
    fclose(cloud_data);


    // result pointer //
    for(int i = 0; i < res_number; i++){
	    result[i] = new_gate_bootstrapping_ciphertext_array(res_length, params);}
    for(int i = 0; i < N; i++){
	    M[i] = new_gate_bootstrapping_ciphertext_array(k, params);}

    // start time check //
    float time = -clock();

    //////////////////////////////
    ////// start operation ///////

    HomKmeans(result, M, ciphertext1, ciphertext2, length, N, k, bk);

    ////// end operation /////////
    //////////////////////////////

    // end time check //    
    time += clock();
    time = time/(CLOCKS_PER_SEC);

    printf("done in %f seconds...\n", time);

    // export the res_number x res_length ciphertexts to a file //
    FILE* answer_data = fopen("answer.data","wb");
    for (int i = 0; i < res_number; i++){
	    for (int j = 0; j < res_length; j++){
		    export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i][j], params);}}
    fclose(answer_data);
    FILE* label_data = fopen("label.data","wb");    
    for (int i = 0; i < N; i++){
	    for (int j = 0; j < k; j++){
		    export_gate_bootstrapping_ciphertext_toFile(label_data, &M[i][j], params);}}
    fclose(label_data);

    // clean up all pointers //
    for(int i = 0; i < N; i++){
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);}

    for(int i = 0; i < k; i++){
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext2[i]);}

    for(int i = 0; i < res_number; i++){    
	    delete_gate_bootstrapping_ciphertext_array(res_length, result[i]);}
	    
    for(int i = 0; i < N; i++){    
	    delete_gate_bootstrapping_ciphertext_array(k, M[i]);}

    delete_gate_bootstrapping_cloud_keyset(bk);

}



