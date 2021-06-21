#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>
#include <HomOperLR.c>



int main() {
    printf("Step2 start\n");

    //reads the cloud key from file
    FILE* cloud_key = fopen("cloudLR.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = bk->params;

    //read the ciphertexts
    int N = 5;
    int length = 16;
    int res_length = 16;
    int res_param = 2;
    

    LweSample* ciphertext1[N], *ciphertext2[N], *result[res_param];

    for(int i = 0; i < N; i++)
    {
        ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }
    
    for(int i = 0; i < N; i++)
    {
        ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    ////reads the ciphertexts from the cloud file
    FILE* cloud_data = fopen("cloudLR.data","rb");
    //read ciphertext1
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < length; j++)
        {
            import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i][j], params);
        }
    }
    //read ciphertext2
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < length; j++)
        {
            import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i][j], params);
        } 
    }
    //close cloud data
    fclose(cloud_data);

    ////do some operations on the ciphertexts: here, we will compute the
    //linear regression of X : ciphertext1 and Y : ciphertext2
    //result : number of parameter inference
    for(int i = 0; i < res_param; i++)
    {
        result[i] = new_gate_bootstrapping_ciphertext_array(res_length, params);
    }

    //record time : start
    float time = -clock();

    ////// start operation ///////

   
    HomLogReg(result, ciphertext1,ciphertext2, N, length, bk); 

    ////// end operation ///////
    
    //record time : end
    time += clock();
    time = time/(CLOCKS_PER_SEC);
    printf("done in %f seconds...\n", time);

    //export the 32 ciphertexts to a file (for the cloud)
    FILE* answer_data = fopen("answerLR.data","wb");
    for (int i = 0; i < res_param; i++)
    {
        for (int j = 0; j < res_length; j++)
        {
            export_gate_bootstrapping_ciphertext_toFile(answer_data, &result[i][j], params);
        }
    }
    fclose(answer_data);

    //clean up all pointers
    for(int i = 0; i < N; i++)
    {
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext2[i]);
    }

    for(int i = 0; i < res_param; i++)
    {
        delete_gate_bootstrapping_ciphertext_array(res_length, result[i]);
    }
    delete_gate_bootstrapping_cloud_keyset(bk);

}