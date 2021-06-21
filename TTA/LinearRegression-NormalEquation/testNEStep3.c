#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>



int main() {
    printf("Step3 start\n");
    int res_param=2;
    //reads the cloud key from file
    FILE* secret_key = fopen("secretNE.key","rb");
    TFheGateBootstrappingSecretKeySet* key = new_tfheGateBootstrappingSecretKeySet_fromFile(secret_key);
    fclose(secret_key);
 
    //if necessary, the params are inside the key
    const TFheGateBootstrappingParameterSet* params = key->params;

    //read the 16 ciphertexts of the result
    int length = 32;
    LweSample* answer[res_param];

    for(int i=0; i<res_param; i++)
    {
        answer[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    //additional information


    //import the 32 ciphertexts from the answer file
    FILE* answer_data = fopen("answerNE.data","rb");
    for (int i=0; i<res_param; i++)
    {
        for (int j=0; j<length; j++)
        {
            import_gate_bootstrapping_ciphertext_fromFile(answer_data, &answer[i][j], params);
        } 
    } 
        fclose(answer_data);

    //decrypt and rebuild the 16-bit plaintext answer
    int16_t int_answer = 0;
    int16_t int_answer2 = 0;
    
    for (int i=0; i<length; i++) {
        int ai = bootsSymDecrypt(&answer[0][i], key);
        int_answer |= (ai<<i);
    }
    for (int i=0; i<length; i++) {
        int ai = bootsSymDecrypt(&answer[1][i], key);
        int_answer2 |= (ai<<i);
    }

    double float_answer = int_answer * (1.0/(1<<(int)(length/2)));
    double float_answer2 = int_answer2 * (1.0/(1<<(int)(length/2)));

    //printf("And the result is: %d\n", int_answer);
    printf("The result beta_0 is: %f\n", float_answer);
    printf("And the result beta_1 is: %f\n", float_answer2);

    // printf("ER_diff is : %f\n", 0.8785);

    FILE *fp;
    fp = fopen("betaNE.txt","w");
    fprintf(fp,"%f\n%f", float_answer, float_answer2);
    fclose(fp);


    //clean up all pointers
    for(int i=0; i<res_param; i++)
    {
        delete_gate_bootstrapping_ciphertext_array(length, answer[i]);
    } 
    delete_gate_bootstrapping_secret_keyset(key);
}
