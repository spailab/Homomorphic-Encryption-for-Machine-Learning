 void PlainKmeans(int res[10], int *a, int *u, const int length, const int N, const int k){


	int L[N];
 	int M[N][k];
 	int w[k-1];

	
	// data copy //
	for(int i = 0; i < N; i++)
	{
 		L[i] = a[i];
 	}



 	int s = 0;
 	while(s < 1){
 		// calculate median of near cluster's means //
 		// w[i] = median(mean) of u[i] and u[i+1] //
		for(int i = 0; i < k-1; i++)
		{
			w[i] = (u[i] + u[i+1])/2;
		}
 				
		//printf("%d - calculating median of cluster's means clear!\n", s+1);

 		// label setting process //
 		for(int i = 0; i < N; i++)
 		{
 			for(int j = 0; j < k; j++)
 			{
 				switch(j){
 					case 0 :
 					if (L[i] <= w[0])
 						M[i][j] = 1;
 					else
 						M[i][j] = 0;
 					break;

 					case 1 :
 					if (L[i] > w[0] && L[i] <= w[1])
 						M[i][j] = 1;
 					else
 						M[i][j] = 0;
 					break;
 					default :
 					if (L[i] > w[1])
 						M[i][j] = 1;
 					else
 						M[i][j] = 0;
 				}


 			}

 			//printf("%d - %d - label setting process clear!\n", s+1, i+1);		
 		}
 			

 		// counting # of each cluster's data //
 		// sum of each cluster's data //
 		// new_cluster mean u[i] //
		
 		int sum_data[k];
 		int sum_label[k];
 		int p = 0;


		for(int i = 0; i < k; i++)
		{
			sum_data[i] = 0;
			sum_label[i] = 0;

			for(int j = 0; j < N; j++)
			{
				p = L[j]*M[j][i];
				sum_data[i] += p;
				sum_label[i] += M[j][i];
			}

			u[i] = sum_data[i] / sum_label[i];
//			printf("%d / %d = %d \n", sum_data[i], sum_label[i], u[i]);
		}


		//printf("%d - %d cluster mean calculate complete\n", s+1,k);
		

		s++;

 		//printf("%d - loop clear\n", s);


 	}

 	for(int i = 0; i < k; i++)
 	{
 		res[i] = u[i];
 	}
 	
 	return res[k];

 }

