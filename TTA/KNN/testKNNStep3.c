#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>


int main() {

    printf("Step3 start\n");
    
    int loop = 10; 
    int res_param = loop;
    int length = 1;

    // reads the cloud key from file //
    FILE* secret_key = fopen("secret.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);
 
    // if necessary, the params are inside the key //
    const TFheGateBootstrappingParameterSet* params = key->params;

    //read the 16 ciphertexts of the result
    
    LweSample* answer[res_param];

    for(int i=0; i<res_param; i++)
    {
        answer[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    //additional information


    //import the 32 ciphertexts from the answer file
    FILE* answer_data = fopen("answer.data","rb");
    for (int i=0; i<res_param; i++)
    {
        for (int j=0; j<length; j++)
        {
            import_gate_bootstrapping_ciphertext_fromFile(answer_data, &answer[i][j], params);
        } 
    } 
    fclose(answer_data);

///////////////////////////////////////////////////////////////////////////////////////    
        
    int16_t int_answer[res_param];
    
    for(int i = 0; i < res_param; i++)
    {
        int_answer[i]=0;
        for(int j = 0; j < length; j ++)
        {
            int ai = bootsSymDecrypt(&answer[i][j], key);
            int_answer[i] |= (ai<<j);
        }
        printf("The result %d is: %d \n", i+1, int_answer[i]);
    }
    printf("\n");
///////////////////////////////////////////////////////////////////////////////////////    


    FILE* KNNCipherResult = fopen("KNNCipherResult.txt","w");
    for(int i = 0; i < res_param; i++){
        fprintf(KNNCipherResult, "%d \n", int_answer[i]); 
    }
    fclose(KNNCipherResult);



    // clean up all pointers //

    for(int i = 0; i < res_param; i++){
        delete_gate_bootstrapping_ciphertext_array(length, answer[i]);}
    
    delete_gate_bootstrapping_secret_keyset(key);
}

