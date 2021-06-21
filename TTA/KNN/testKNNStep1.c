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
    
    //////////////////////////////////////////////////////////////
    // initial value //
    int length = 10;
    int N = 32;
    int d = 2;
    int label_length = 1;
    int loop = 10;
    // generate encrypt the data //
    // plaintext1 = data //
    // plaintext2 = label //
    // plaintext3 = new data //
    int plaintext1[N*d];
    
    int plaintext2[N];

    int plaintext3[loop*d];

    FILE* data = fopen("KNNdata.txt","rb");
    for(int i = 0; i < N*d; i++)
    {
        fscanf(data, "%d ", &plaintext1[i]);
    }
    fclose(data);

    FILE* label = fopen("KNNlabel.txt","rb");
    for(int i = 0; i < N; i++)
    {
        fscanf(label, "%d ", &plaintext2[i]);
    }
    fclose(label);

    FILE* newdata = fopen("KNNnewdata.txt","rb");
    for(int i = 0; i < loop*d; i++)
    {
        fscanf(newdata, "%d ", &plaintext3[i]);
    }
    fclose(newdata);

    


    LweSample* ciphertext1[N*d];
    LweSample* ciphertext2[N];
    LweSample* ciphertext3[loop*d];

    // encryption plaintext1 //
    for(int i = 0; i < N*d; i++){
	    ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }
    for (int i = 0; i < N*d; i++){
	   for (int j = 0; j < length; j++) {
            bootsSymEncrypt(&ciphertext1[i][j], (plaintext1[i]>>j)&1, key);
        }
    }

    // encryption plaintext2 //
    for(int i = 0; i < N; i++){
	    ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(label_length, params);
    }
    for (int i = 0; i < N; i++){
	   for (int j = 0; j < label_length; j++) {
            bootsSymEncrypt(&ciphertext2[i][j], (plaintext2[i]>>j)&1, key);
        }
    }

    // encryption plaintext3 //
    for(int i = 0; i< loop*d; i++){
	    ciphertext3[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }
    for (int i = 0; i < loop*d; i++){
	   for (int j = 0; j < length; j++) {
            bootsSymEncrypt(&ciphertext3[i][j], (plaintext3[i]>>j)&1, key);
	   }
    }


    printf("Let's start homomorphic KNN algorithms\n\n");
    
    // export the N*d x length ciphertexts  = data //
    FILE* cloud_data = fopen("cloud.data","wb");
    for (int i = 0; i < N*d; i++){
	    for (int j = 0; j < length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i][j], params);}}

    // export the N x label_length ciphertexts  = label //
    for (int i = 0; i < N; i++){
	    for (int j = 0; j < label_length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i][j], params);}}

    // export the d x length ciphertexts  = new data //
    for (int i = 0; i < loop*d; i++){
	    for (int j = 0; j < length; j++){
                export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext3[i][j], params);}}

    fclose(cloud_data);

    //clean up all pointers
    for(int i = 0; i < N*d; i++)
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);
    for(int i = 0; i < N; i++)
	    delete_gate_bootstrapping_ciphertext_array(label_length, ciphertext2[i]);
    for(int i = 0; i < loop*d; i++)
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext3[i]);

    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

}
