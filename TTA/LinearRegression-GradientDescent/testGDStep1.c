#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <math.h>

int16_t encode_double_t16(int length, double m) {
    int16_t result = (int16_t) round(m * (1<<(int)(length/2 )));
    return result;
}


int main() {
    printf("Step1 start\n");
    //generate a keyset
    const int minimum_lambda = 110;
    TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);

    //generate a random key
    uint32_t seed[] = { 314, 1592, 657 };
    tfhe_random_generator_setSeed(seed,3);
    TFheGateBootstrappingSecretKeySet* key = new_random_gate_bootstrapping_secret_keyset(params);

    //export the secret key to file for later use
    FILE* secret_key = fopen("secret.key","wb");
    export_tfheGateBootstrappingSecretKeySet_toFile(secret_key, key);
    fclose(secret_key);

    //export the cloud key to a file (for the cloud)
    FILE* cloud_key = fopen("cloud.key","wb");
    export_tfheGateBootstrappingCloudKeySet_toFile(cloud_key, &key->cloud);
    fclose(cloud_key);


    //you can put additional instructions here!!//////////////////////////////////////////////////////////////
    int length = 16;
    int N = 5;
    
    
    //generate an encryption of 5 intput plaintext1 : X 
    
    double p[5] = {1,4,2,6,3};
    int16_t plaintext1[N];


    for(int i = 0; i < N; i++)
    {
        plaintext1[i] = encode_double_t16(length, p[i]);

    }

 
    LweSample* ciphertext1[N];

    for(int i = 0; i < N; i++)
	    ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);

    for (int i = 0; i < N; i++)
    {
       for (int j = 0; j < length; j++) 
        {
            bootsSymEncrypt(&ciphertext1[i][j], (plaintext1[i]>>j)&1, key);
        }
    }


    //generate an encryption of 5 intput plaintext2 : Y

    double q[5] = {1,2,3,4,5};
    int16_t plaintext2[N];

    for(int i = 0; i < N; i++)
    {
        plaintext2[i] = encode_double_t16(length, q[i]);

    }


    LweSample* ciphertext2[N];

    for(int i = 0; i< N; i++)
	    ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(length, params);

    for (int i = 0; i < N; i++)
    {
       for (int j = 0; j < length; j++) 
        {
            bootsSymEncrypt(&ciphertext2[i][j], (plaintext2[i]>>j)&1, key);
        }
    }


    printf("Hi there! Today, I will ask the cloud what is the linear regeression-gradient descent of :\n ");
   
    for(int i=0; i<N; i++)
    {
        printf("(%f, %f),\n" , p[i], q[i]);    
    }


    //export the 2x16 ciphertexts to a file (for the cloud)
    FILE* cloud_data = fopen("cloud.data","wb");
    for (int i = 0; i < N; i++){
	    for (int j = 0; j < length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext1[i][j], params);}}

    for (int i = 0; i < N; i++){
	    for (int j = 0; j < length; j++){
	        export_gate_bootstrapping_ciphertext_toFile(cloud_data, &ciphertext2[i][j], params);}}

    fclose(cloud_data);

    //clean up all pointers
    for(int i = 0; i < N; i++)
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);
    for(int i = 0; i < N; i++)
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext2[i]);

    //////////////////////////////////////////////////////////////
    //clean up all pointers
        delete_gate_bootstrapping_secret_keyset(key);
        delete_gate_bootstrapping_parameters(params);

}
