#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <HomOper.c>
#include <HomMultiDiv.c>
#include <HomKNN.c>


int main() {

    printf("Step2 start\n");

    // reads the cloud key from file //
    FILE* cloud_key = fopen("cloud.key","rb");
    TFheGateBootstrappingCloudKeySet* bk = new_tfheGateBootstrappingCloudKeySet_fromFile(cloud_key);
    fclose(cloud_key);
 
    // if necessary, the params are inside the key //
    const TFheGateBootstrappingParameterSet* params = bk->params;

    // initial value // 
    int N = 32;
    int d = 2; 
    int k = 3;
    int length = 10;
    int label_length = 1;
    int loop = 10;

    int res_num = loop;
    int res_length = 1;

    LweSample* ciphertext1[N*d], *ciphertext2[N], *ciphertext3[loop*d], *result[res_num];

    for(int i = 0; i < N*d; i++){
	    ciphertext1[i] = new_gate_bootstrapping_ciphertext_array(length, params);}
    for(int i = 0; i < N; i++){
    	ciphertext2[i] = new_gate_bootstrapping_ciphertext_array(label_length, params);}
    for(int i = 0; i < loop*d; i++){
    	ciphertext3[i] = new_gate_bootstrapping_ciphertext_array(length, params);}

    // reads the N*d x length ciphertexts  = data //
    // reads the N x label_length ciphertexts  = label //
    // reads the d x length ciphertexts  = new data //
    FILE* cloud_data = fopen("cloud.data","rb");
    for (int i = 0; i < N*d; i++){
        for (int j = 0; j < length; j++){
            import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext1[i][j], params);}}

    for (int i = 0; i < N; i++){
    	for (int j = 0; j < label_length; j++){
	        import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext2[i][j], params);}}

    for (int i = 0; i < loop*d; i++){
    	for (int j = 0; j < length; j++){
	        import_gate_bootstrapping_ciphertext_fromFile(cloud_data, &ciphertext3[i][j], params);}}
    fclose(cloud_data);

    // result pointer //
    for (int i = 0; i < res_num; i++){
        result[i] = new_gate_bootstrapping_ciphertext_array(res_length, params);}

    

    //////////////////////////////
    ////// start operation ///////

    

    LweSample* ndata[d];
    LweSample* result1[loop];

    for(int i = 0; i < d; i++){
        ndata[i] = new_gate_bootstrapping_ciphertext_array(length, params);}
    for(int i = 0; i < loop; i++){
        result1[i] = new_gate_bootstrapping_ciphertext_array(3, params);}


    /////////////////////////////////////////////////////////////
    for(int i = 0; i < loop; i++)
    {
        for(int j = 0; j < d; j++)
        {
            for(int p = 0; p < length; p++)
            {
                bootsCOPY(&ndata[j][p], &ciphertext3[d*i+j][p], bk);
            }
        }
        printf("///////////////////////////////////////////////////////////\n");
        printf("///////////////// %dth KNN algorithm start /////////////////\n", i+1);
        printf("///////////////////////////////////////////////////////////\n \n");
    // start time check //
        float time = -clock();

        HomKNN(result, ciphertext1, ciphertext2, ndata, length, label_length, N, d, k, bk);

        bootsCOPY(&result1[i][0], &result[0][0], bk);

    // end time check //    
        time += clock();
        time = time/(CLOCKS_PER_SEC);

        printf("\ndone in %f seconds...\n \n", time);
        printf("///////////////////////////////////////////////////////////\n");
        printf("/////////////// %dth KNN algorithm completed ///////////////\n", i+1);
        printf("///////////////////////////////////////////////////////////\n \n");

    }
    

    ////// end operation ///////
    //////////////////////////////
    


    // export the label_length ciphertexts to a file //
    FILE* answer_data = fopen("answer.data","wb");
    for (int j = 0; j < res_num; j++){
        for (int i = 0; i < res_length; i++){
	        export_gate_bootstrapping_ciphertext_toFile(answer_data, &result1[j][i], params);}}
    fclose(answer_data);

    // clean up all pointers //
    for(int i = 0; i < N*d; i++){
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1[i]);}
    for(int i = 0; i < N; i++){
	    delete_gate_bootstrapping_ciphertext_array(label_length, ciphertext2[i]);}
    for(int i = 0; i < loop*d; i++){
	    delete_gate_bootstrapping_ciphertext_array(length, ciphertext3[i]);}
    for(int i = 0; i < d; i++){
        delete_gate_bootstrapping_ciphertext_array(length, ndata[i]);}
    for(int i = 0; i < loop; i++){
        delete_gate_bootstrapping_ciphertext_array(3, result1[i]);}
    
}



