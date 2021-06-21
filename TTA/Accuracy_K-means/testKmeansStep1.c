#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <math.h>

int main() {

    printf("Step1 start\n");

    // key setting process //
    // generate a keyset //
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    // generate a random key //
    uint32_t seed[] = { 314, 1592, 657 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    // export the secret key to file for later use //
    FILE* secret_key = fopen("secret.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);

    // export the cloud key to a file (for the cloud) //
    FILE* cloud_key = fopen("cloud.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);

    /////////////////////////////////////////////////////////////////////////////
    // initial value //
    int length = 10;
    int N = 64;
    int k = 3;

    // generate encrypt the data //
    int plaintext1[N];

    FILE* data = fopen("Kmeansdata.txt","rb");
    for(int i = 0; i < N; i++)
    {
        fscanf(data, "%d ", &plaintext1[i]);
    }
    fclose(data);

    LweSample* ciphertext1[N];

    for(int i = 0; i < N; i++)
	    ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);

    // encryption plaintext1 //
    for (int i = 0; i < N; i++){
	   for (int j = 0; j < length; j++) {
            bootsSymEncrypt(&ciphertext1[i][j], (plaintext1[i]>>j)&1, key);
    }}


    // generate encrypt the initial cluster mean //
    int plaintext2[k];

    FILE* cluster = fopen("Kmeanscluster.txt","rb");
    for(int i = 0; i < k; i++)
    {
        fscanf(cluster, "%d ", &plaintext2[i]);
    }
    fclose(cluster);




    LweSample* ciphertext2[k];

    for(int i = 0; i< k; i++)
	    ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(length, params);

    // encryption plaintext2 //
    for (int i = 0; i < k; i++){
	   for (int j = 0; j < length; j++) {
            bootsSymEncrypt(&ciphertext2[i][j], (plaintext2[i]>>j)&1, key);
	}}


    printf("Let's start homomorphic k-means algorithms\n");
    
    // export the N x length ciphertexts  = data //
    FILE* cloud_data = fopen("cloud.data","wb");
    for (int i = 0; i < N; i++){
	    for (int j = 0; j < length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i][j], params);}}

    // export the k x length ciphertexts = initial cluster mean //
    for (int i = 0; i < k; i++){
	    for (int j = 0; j < length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i][j], params);}}

    fclose(cloud_data);

    //clean up all pointers //
    for(int i = 0; i < N; i++)
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);
    for(int i = 0; i < k; i++)
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2[i]);
    

    //clean up all pointers
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

}
