/*SlopeOne Recommender (with bonus points)
 * This program takes as parameters an input .txt file and a user number.
 * It executes the SlopeOne algorithm in order to find the predicted ratings
 * of users on certain items. The program is divided in a main section used
 * primarily for variables creation, memory allocation when needed and for
 * calling functions. Function header_checker extracts the users and items,
 * getRatings is used to obtain all ratings in the file and save them to a
 * matrix. Finally, the calcs function calculates averages, frequencies, and
 * the factors of the final formula before printing out all results. If a number
 * of highest predicted ratings is given, then it shows the list, otherwise it
 * prints all ratings, both predicted and not predicted, for the given user.
 * Fernando Ramón Celaya Loaiza k1156507*/
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Header checker*/
int header_checker (FILE *input_file, int headerInfo_array[]);

/*Creating the ratings matrix*/
int getRatings (FILE *input_file, int headerInfo_array[], int** ratingsMatrix);

/*Fill the averages and frequencies matrixes*/
int calcs (int userid, int tops, int headerInfo_array[], int** ratingsMatrix, float** averagesMatrix, int** frequenciesMatrix);

int main(int argc, char *argv[]) 
{
	int code = 0, userid = 0, tops = 0, i, j;							//tops is zero if highest rated items are not requested.
	int headerInfo_array[] = {0, 0};									//First number is total users, second total rated items.
	int **ratingsMatrix, **frequenciesMatrix;
    int *ratings_array, *frequencies_array;
    float **averagesMatrix;
    float *averages_array;											
	FILE *input_file = NULL;
	
	/*Check congruence of command line*/
	if (argc < 3 || argc > 4){
		fprintf(stderr, "Incorrect number of arguments, check command line. Error code 1.\n");									
		return 1;
	}
	else
		if(argc==4){
			tops = atoi(argv[3]);
			if(tops <= 0){
				fprintf(stderr, "Invalid number of highest rated items in command line. Error code 1.\n");
				return 1;
			}
		}
		
	/*Opening input file and looking for errors*/
	input_file = fopen(argv[1], "r");									
	if (input_file == NULL){
		fprintf(stderr, "Failed to open file '%s' for reading. Error code 2.\n", argv[1]);
		return 2;
	}
	
	/*Get user ID from command line and verify it congruence*/
	userid = atoi(argv[2]);
	if(userid <= 0){
		fprintf(stderr, "Invalid user ID in command line. Error code 1.\n");
		return 1;
	}
	
	/*Parse total users and items from head of file*/
	code = header_checker(input_file, headerInfo_array);
	if(code!=0)
		return code;
	
	/*Verify the given user exists in our file*/
	if(userid > headerInfo_array[0]){
		fprintf(stderr, "User ID not found in file, please check command line. Error code 1.\n");
		return 1;
	}
	
	/*Verify there are enough items to show as highest rated items*/
	if(tops>headerInfo_array[1]){
		fprintf(stderr, "Not enough total items for requested number of highest items. Error code 1.\n");
		return 1;
	}
	
	/*Start constructing our ratings matrix with the info extracted from the header*/
	/*Allocating memory for the array*/
    ratings_array = malloc(headerInfo_array[0] * headerInfo_array[1] * sizeof(int));
    if (ratings_array == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    /*Allocating memory for the pointers to the rows */
    ratingsMatrix = malloc(headerInfo_array[0] * sizeof(int *));
    if (ratingsMatrix == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    /*Assign the pointers*/
    for (i = 0; i < headerInfo_array[0]; i++)
        ratingsMatrix[i] = ratings_array + (i * headerInfo_array[1]);
        
    /*Fill the matrix with zeros, indicative to us of "no rating given"*/
    for (i = 0; i < headerInfo_array[0]; i++)
        for (j = 0; j < headerInfo_array[1]; j++)
            ratingsMatrix[i][j] = 0;
	
	/*Fill the ratings with the values of the file*/
	code = getRatings (input_file, headerInfo_array, ratingsMatrix);
	if(code!=0)
		return code;
	
	/*Start constructing averages and frequencies matrixes*/
	/*Allocating memory for both arrays*/
	averages_array = malloc(headerInfo_array[1] * headerInfo_array[1] * sizeof(float));
    if (averages_array == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    frequencies_array = malloc(headerInfo_array[1] * headerInfo_array[1] * sizeof(int));
    if (frequencies_array == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    /*Allocating memory for the pointers to the rows */
    averagesMatrix = malloc(headerInfo_array[1] * sizeof(float *));
    if (averagesMatrix == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    frequenciesMatrix = malloc(headerInfo_array[1] * sizeof(int *));
    if (frequenciesMatrix == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
        
    /*Assign the pointers*/
    for (i = 0; i < headerInfo_array[1]; i++){
        averagesMatrix[i] = averages_array + (i * headerInfo_array[1]);
        frequenciesMatrix[i] = frequencies_array + (i * headerInfo_array[1]);
	}
		
	/*Fill the matrixes with zeros*/
    for (i = 0; i < headerInfo_array[1]; i++)
        for (j = 0; j < headerInfo_array[1]; j++){
			averagesMatrix[i][j] = 0;
            frequenciesMatrix[i][j] = 0;
		}
    
    /*Let's do the calculations of the algorithm*/
    code = calcs(userid, tops, headerInfo_array, ratingsMatrix, averagesMatrix, frequenciesMatrix);
    
    /*Finish by releasing memory and sending the return code*/
	free(ratings_array);
	free(averages_array);
	free(frequencies_array);
	free(ratingsMatrix);	
	free(averagesMatrix);
	free(frequenciesMatrix);
	close(input_file);
	return code;
}

/**********************************************************************/

int header_checker (FILE *input_file, int headerInfo_array[])
{
	int num, count, iwantzero=0;										//Variable iwantzero is used so the function getline allocates space
	char *dynamic_array = NULL;											//automatically according to its needs
	char *p;
	
	/*Get first line from input file. Verify success*/
	count = getline(&dynamic_array, &iwantzero, input_file);
	if(count == 0){
		fprintf(stderr, "Error reading from input file. Error code 2.\n");
		return 2;
	}
	
	/*Point p to our total users*/	
	p = &dynamic_array[7];
	if(p == NULL){
		fprintf(stderr, "Error in header formatting. Error code 4.\n");
		return 4;
	}
	headerInfo_array[0]=num;
	
	/*Retrieve total users and save it in headerInfo_array array*/
	sscanf(p, "%i", &num); 
	if(num<=0){
		fprintf(stderr, "Invalid number of users, please check input file. Error code 4.\n");
		return 4;
	}
	headerInfo_array[0]=num;
	
	/*Move pointer to the coma*/
	do{
		p++;
		if (*p == NULL){												//In case we never find the comma
			fprintf(stderr, "Error in header formatting. Error code 4.\n");
			return 4;
		}
	}	
	while(*p != ',');
	
	/*Move pointer to number of total items*/
	do{
		p++;
		if (*p == NULL){												//In case we never find the total items
			fprintf(stderr, "Error in header formatting. Error code 4.\n");
			return 4;
		}
	}	
	while(*p < 48 || *p > 57);
	
	/*Retrieve total items and save it in headerInfo_array array*/
	sscanf(p, "%i", &num);
	if(num<=0){															
		fprintf(stderr, "Invalid number of items, please check input file. Error code 4.\n");
		return 4;
	}
	headerInfo_array[1]=num;
	
	/*Free the memory and return 0 if we had no problems*/
	free(dynamic_array);				
	return 0;
}

/**********************************************************************/

int getRatings (FILE *input_file, int headerInfo_array[], int** ratingsMatrix)
{
	int usr, itm, rtng, count, iwantzero=0;								//Variable iwantzero is used so the function getline allocates space
	char *dynamic_array = NULL;											//automatically according to its needs
	char *p;
	
	/*First line, just to get in*/
	count = getline(&dynamic_array, &iwantzero, input_file);
	if(count == 0){
		fprintf(stderr, "Error reading from input file. Error code 2.\n");
		return 2;
	}
	
	/*Point p to the first position in dynamic_array*/	
	p = &dynamic_array[0];
		
	do{
		/*Scan user id*/
		sscanf(p, "%i", &usr);
		if(usr<=0){
			fprintf(stderr, "Invalid user ID found in the input file. Error code 4.\n");
			return 4;
		}
		
		/*Point P to the first white space*/
		do{
			p++;
			if (*p == NULL){											//In case we never find the total items
				fprintf(stderr, "Error in ratings formatting. Error code 4.\n");
				return 4;
			}
		}	
		while(*p != 32 && *p != 9);
		
		/*Point P to next number, item ID*/
		do{
			p++;
			if (*p == NULL){												//In case we never find the total items
				fprintf(stderr, "Error in ratings formatting. Error code 4.\n");
				return 4;
			}
		}	
		while(*p < 48 || *p > 57);
	
		/*Scan item id*/
		sscanf(p, "%i", &itm); 
		if(itm<=0){
			fprintf(stderr, "Invalid item ID found. Error code 4.\n");
			return 4;
		}
		
		/*Point P to the first white space*/
		do{
			p++;
			if (*p == NULL){												//In case we never find the total items
				fprintf(stderr, "Error in ratings formatting. Error code 4.\n");
				return 4;
			}
		}	
		while(*p != 32 && *p != 9);
	
		/*Point P to next number, rating*/
		do{
			p++;
			if (*p == NULL){												//In case we never find the total items
				fprintf(stderr, "Error in ratings formatting. Error code 4.\n");
				return 4;
			}
		}	
		while(*p < 48 || *p > 57);
	
		/*Scan rating*/
		sscanf(p, "%i", &rtng); 
		if(rtng<=0){
			fprintf(stderr, "Invalid rating number found in the input file. Error code 4.\n");
			return 4;
		}
		
		/*Since arrays start counting at 0, we have to decrease the values extracted from the file*/
		ratingsMatrix[usr-1][itm-1]=rtng;
		
		/*Get next line from input file*/
		count = getline(&dynamic_array, &iwantzero, input_file);
		if(count == 0){
			fprintf(stderr, "Error reading from input file. Error code 2.\n");
			return 2;
		}
	
		/*Point p to the first position in dynamic_array*/	
		p = &dynamic_array[0];
	}
	while(!feof(input_file));
	
	/*Free the memory and return 0 if we had no problems*/
	free(dynamic_array);
	return 0;
}

/**********************************************************************/

int calcs (int userid, int tops, int headerInfo_array[], int** ratingsMatrix, float** averagesMatrix, int** frequenciesMatrix)
{
	int i, j, u;
	float **predictionsMatrix;											//We use this array to save invidual sums of frequencies and
	float *predictions_array;											//weighted sums.
	
	float topsArray[headerInfo_array[1]];								//We will use this if we are requested a number of highest items
		for (i=0; i<headerInfo_array[1]; i++)							//Fill it with zeros
			topsArray[i]=0;												
		
	/*Allocating memory for predictions array*/
    predictions_array = malloc(headerInfo_array[1] * 2 * sizeof(float));
    if (predictions_array == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    /*Allocating memory for the pointers to the rows */
    predictionsMatrix = malloc(headerInfo_array[1] * sizeof(float *));
    if (predictionsMatrix == NULL){
        fprintf(stderr, "Error allocating memory. Error code 3.\n");
		return 3;
    }
    
    /*Assign the pointers*/
    for (i = 0; i < headerInfo_array[1]; i++)
        predictionsMatrix[i] = predictions_array + (i * 2);    
	
	/*This gets the sums of differences and frequencies*/
	for (i=0; i<headerInfo_array[1]; i++)
		for (j=0; j<headerInfo_array[1]; j++)
			for (u=0; u<headerInfo_array[0]; u++)
				if(i!=j && ratingsMatrix[u][i] > 0 && ratingsMatrix[u][j] > 0){
					averagesMatrix[i][j] += ratingsMatrix[u][j] - ratingsMatrix[u][i];
					frequenciesMatrix[i][j]++;
				}
				
	/*Now calculate averages of the differences*/
	for (i=0; i<headerInfo_array[1]; i++)
		for (j=0; j<headerInfo_array[1]; j++)
			if(frequenciesMatrix[i][j] > 0)
				averagesMatrix[i][j] /= frequenciesMatrix[i][j];
			
	/*Initialize the predictions Matrix*/
	for(i=0; i<headerInfo_array[1]; i++)
		for(j=0; j<2; j++)
			predictionsMatrix[i][j]=0;
		
	/*Next comes the upper part of the formula*/
	for (i=0; i<headerInfo_array[1]; i++)
		if (ratingsMatrix[userid-1][i]>0)
			for (j=0; j<headerInfo_array[1]; j++){
				predictionsMatrix[j][0] += (ratingsMatrix[userid-1][i] + averagesMatrix[i][j]) * frequenciesMatrix[i][j];
				predictionsMatrix[j][1] += frequenciesMatrix[i][j];
			}
	
	/*Next we divide and show the results, if we weren't requested highest ratings*/
	for(i=0; i<headerInfo_array[1]; i++){
		if(ratingsMatrix[userid-1][i]>0){
			if(tops==0){printf("%i %.2f\n", i+1, (float)ratingsMatrix[userid-1][i]);}
		}
		else{
			if(predictionsMatrix[i][1]!=0){
				if(tops==0){printf("%i %.2f\n", i+1, (predictionsMatrix[i][0]/predictionsMatrix[i][1]));}
				else{topsArray[i] = predictionsMatrix[i][0]/predictionsMatrix[i][1];}
			}
			else
				if(tops==0){printf("%i %.2f\n", i+1, 0.00);}
		}
	}
	
	/*Return if highest ratings weren't requested*/
	if(tops==0)
	{
		free(predictionsMatrix);
		free(predictions_array);
		return 0;
	}
	
	/*Return if the user has not enough predictions for requested highest predictions*/
	int count = 0;
	for(i=0;i<headerInfo_array[1];i++)
		if(topsArray[i]!=0)
			count++;	
	if(count < tops){
        fprintf(stderr, "Not enough predictions generated to satisfy requested number. Error code 1.\n");
		return 1;
    }
	
	/*Search for highest items*/
	count = 0;
	int pos = 0;
	float temp = 0;
	do{																	//Do while we dont get enough ratings
		for(i=0; i < headerInfo_array[1]; i++)							//For every item
			if(topsArray[i]>temp){										//If our temporal maximum number is lower than a new one
				pos = i;												//Save position
				temp=topsArray[i];										//Save rating
			}
		printf("%i %.2f\n", pos+1, temp);								//Print them
		topsArray[pos]=0;												//Make the value found zero so we don't print it again
		temp=0;															//Reset temp variable
		count++;														//We got a new rating
	}while(count<tops);
	
	/*Free the memory and return 0 if we had no problems*/
	free(predictionsMatrix);
	free(predictions_array);
	return 0;
}

/**********************************************************************/
