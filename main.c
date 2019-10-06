#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAXCHAR 1000
#define N 6

char plainText[100]; // read content of plainTextFile file into plainText
int n=6; // size of matrix
char cipherText[200]; // read content of cipherTextFile into cipherText
char cipherTextXEncSym[200];

int key[N][N]; // key matrix
int keyMatrixSum; // sum of elements of all elements of key matrix
int cipherTextLength;
int keyMatrixTSum;

// file required for the program
char plainTextFile[100]; // plainText file
char keyFile[100]; // key File
char cipherTextFile[100]; // cipherText file

// substitution tables
char letters[26];
int tP[26];
int tC[26];
char tA[] = {'*', '#', '&', '%', '$', '+', '?', '!','@', '^', '-'};

char plainTextMatrix[100][100]; // to hold plainText vector
int plainTextNumberMatrix[100][100]; // to hold plainText number matrix
int productMatrixC1[100][100]; // stores C Matrix
int cipherTextNumberMatrixC[100][100];
char cipherTextCharMatrixC[100][100];

int extensionVectorX[100][100];
int extensionMatrixE[100][100];
int encryptedExtensionsXenc[100][100];

int M;
// function to read plainText file
void readPlainTextFile(char infile[])
{
    FILE *fp;
    char* filename = infile;
    // printf("I am here");
    fp = fopen(filename, "r");
    while (fgets(plainText, MAXCHAR, fp) != NULL)
        //printf("%s", str);
        fclose(fp);
}

// function to create key matrix out of key file
void readKeyFile(char infile[])
{
    FILE *fp;
    char* filename = infile;
    char line[50];
    fp = fopen(filename, "r");
    int i = 0;
    while (fgets(line, 50, fp) != NULL)
    {
        int j = 0;
        int init_size = strlen(line);
        char delim[] = " ";

        char *ptr = strtok(line, delim);

        while(ptr != NULL)
        {
            key[i][j] = atoi(ptr);
            // printf("%d\n", key[i][j]);
            ptr = strtok(NULL, delim);
            j++;
        }
        i++;
        fclose(fp);
    }
}

void getCofactor(int key[N][N], int temp[N][N], int p, int q, int n)
{
    int i = 0, j = 0;

    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those element
            //  which are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = key[row][col];

                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

int determinantOfMatrix(int key[N][N], int n)
{
    int D = 0; // Initialize result

    //  Base case : if matrix contains single element
    if (n == 1)
        return key[0][0];

    int temp[N][N]; // To store cofactors

    int sign = 1;  // To store sign multiplier

    // Iterate for each element of first row
    for (int f = 0; f < n; f++)
    {
        // Getting Cofactor of mat[0][f]
        getCofactor(key, temp, 0, f, n);
        D += sign * key[0][f] * determinantOfMatrix(temp, n - 1);

        // terms are to be added with alternate sign
        sign = -sign;
    }

    return D;
}

// calculate GCD of two numbers
int calculateGCD(int n1, int n2)
{
    // if user enters negative number, sign of the number is changed to positive
    n1 = ( n1 > 0) ? n1 : -n1;
    n2 = ( n2 > 0) ? n2 : -n2;
    while(n1!=n2)
    {
        if(n1 > n2)
            n1 -= n2;
        else
            n2 -= n1;
    }
    return n1;
}

// calculate determinant, GCD and verifies the GCD
int verifyKeyDeterminant()
{
    int res = determinantOfMatrix(key, N);
    int gcd = calculateGCD(res, 26);
    if(gcd != 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//function to compute sum of main diagonal
int calculateSumOfMainDiagonal(int matrix[N][N], int rows)
{
    int sum = 0;
    int i;
    for(i = 0; i < rows; i++)
    {
        sum = sum + matrix[i][i];
    }

    return sum;
}

//function to compute sum of Secondary Diagonal
int calculateSumOfSecondaryDiagonal(int matrix[N][N], int rows)
{
    int sum = 0;
    int  i;
    int j = rows; // counter for columns since rows = cols in our matrix

    for(i = 0; i < rows; i++)
    {
        sum = sum + matrix[i][j - 1];
        j--;
    }

    return sum;
}

// creates permutation matrix for converting characters into numbers
void createTP(){
    int i;
    for(i = 1; i <= 26; i++){
        letters[i-1] = (char) (65+i-1);
        tP[i] = keyMatrixSum * i;
        printf("%d\n", tP[i]);
    }
}


void createPlainTextMatrix(){
    int len = strlen(plainText);

    int i, j, k;
    k = 0;

    M = len/N + 1;
    int R = N - (len % N);
    // printf("%d", len);
    for(i = 0; i <= len/N; i++){
        for(j = 0; j < N; j++){
            if(k < len){
                plainTextMatrix[i][j] = plainText[k];
                k++;
                printf("%c", plainTextMatrix[i][j]);
            }else if(R != N){
                plainTextMatrix[i][j] = plainText[k-1];
                printf("%c", plainTextMatrix[i][j]);
            }
        }
    }
}


void createPlainTextNumberMatrix(){
    int len = strlen(plainText);
    if(len%N == 0){
        M--;
    }
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            int pos = (int)(plainTextMatrix[i][j]) - 65 + 1;
            printf("%c: %d : %d\n", plainTextMatrix[i][j], (int)(plainTextMatrix[i][j]), tP[pos]);
            plainTextNumberMatrix[i][j] = tP[pos];
        }
    }
}


void matrixMultiplication(){
    int i, j, k;


    //productMatrix stores multiplication of two matrices
	// Initializing elements of productMatrix to 0.
	for(i = 0; i < M; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			productMatrixC1[i][j] = 0;
		}
	}

	printf("\nP matrix\n");

	for(i = 0; i < M; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			printf("%d\t", plainTextNumberMatrix[i][j]);
		}
		printf("\n");
	}

	printf("\nK matrix\n");

	for(i = 0; i < N; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			printf("%d\t", key[i][j]);
		}
		printf("\n");
	}


	// Multiplying matrix firstMatrix and secondMatrix and storing in array.
	for(i = 0; i < M; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			for(k=0; k< N; ++k)
			{
				productMatrixC1[i][j]  +=  plainTextNumberMatrix[i][k] * key[j][k] ;
			}
			printf("%d\n", productMatrixC1[i][j]);
		}
	}


}


void cipherTextVectorC(){
    for(int i=0; i<M; i++){
        for(int j=0; j<N; j++){
            cipherTextNumberMatrixC[i][j] = productMatrixC1[i][j]%26;
        }
    }
}


void createTCAndCipherText(){

    char temp = letters[25];
    for(int i = 25; i > 0; i--)
    {
        tC[i] = letters[i-1];
        // printf("tc: %c", tC[i]);
    }
    // printf("temp: %c", temp);
    tC[0] = temp;

    for(int i=0; i< M; i++){
        for(int j=0; j< N; j++){
            cipherTextCharMatrixC[i][j] = tC[cipherTextNumberMatrixC[i][j]];
        }
    }


}


void createExtensionVectorX(){
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            extensionVectorX[i][j] = (int)(ceil(productMatrixC1[i][j]/26.0));
        }
    }

    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            printf("%d\t", extensionVectorX[i][j]);
        }
        printf("\n");
    }
}


void createExtensionMatrix(){
    int identityMatrix[100][100];
    int i,j,k;

    // creating S*One matrix
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            identityMatrix[i][j] = keyMatrixSum * 1;
        }
    }

    for(i = 0; i < N; ++i){
		for(j = 0; j < N; ++j){
			extensionMatrixE[i][j] = identityMatrix[i][j] + key[i][j];
        }
    }

	printf("\nextension matrix\n");

	for(i = 0; i < N; ++i){
		for(j = 0; j < N; ++j){
			printf("%d\t", extensionMatrixE[i][j]);
		}
		printf("\n");
	}
}


// creates encrypted extensions
void createEncryptedExtensionsXEnc(){
 int i, j, k;


    //productMatrix stores multiplication of two matrices
	// Initializing elements of productMatrix to 0.
	for(i = 0; i < M; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			encryptedExtensionsXenc[i][j] = 0;
		}
	}
	// Multiplying matrix firstMatrix and secondMatrix and storing in array.
	for(i = 0; i < M; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			for(k=0; k< N; ++k)
			{
				encryptedExtensionsXenc[i][j]  +=  extensionVectorX[i][k] * extensionMatrixE[j][k] ;
			}
			printf("%d\n", encryptedExtensionsXenc[i][j]);
		}
	}


}


void joinCipherWithExtension(){
    int k = 0;
    char result[50];
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            cipherText[k] = cipherTextCharMatrixC[i][j];
            // printf("%c", cipherText[k]);
            k++;
            sprintf(result, "%f", (float)(encryptedExtensionsXenc[i][j]));
            int len = strlen(result);
            for(int l = 0; l < len; l++){
                if(result[l] == '.'){
                    break;
                }else{
                    cipherText[k] = result[l];
                    k++;
                }
            }

        }
    }
    cipherTextLength = k - 1;
}


void createAdditionalSymbolTable(){
    int i, j;
    keyMatrixTSum = 0;
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			keyMatrixTSum = keyMatrixTSum + key[i][j];
		}
	}
    int shiftCount = keyMatrixTSum%11;

    printf("\n\n");

    for(int i=0; i<11; i++){
        printf("%c\t", tA[i]);
    }

    if(shiftCount>0){
        char slicedArray[shiftCount];
        int k = 0;
        for(int i = 11-shiftCount; i<11; i++){
            slicedArray[k] = tA[i];
            k++;
        }


        k = 11 - shiftCount - 1;
        for(int i = 10; i > 11 - shiftCount; i--){
            tA[i] = tA[k];
            k--;
        }
        printf("\nafter slicing\n");
        for(int i=0; i<11; i++){
            printf("%c\t", tA[i]);
        }
        printf("\n");
        for(int i=0; i< strlen(slicedArray) - 1; i++){
            tA[i] = slicedArray[i];
        }
    }
    for(int i=0; i<11; i++){
        printf("%c\t", tA[i]);
    }
}

void createXEncSym(){

    printf("\nciphertext\n");

    for(int i=0; i< strlen(tA); i++){
        printf("%c", tA[i]);
    }
    printf("\n");
    int num;
    for(int i=0; i< strlen(cipherText); i++){
        if(isdigit(cipherText[i])){
            //printf("N");
            num = cipherText[i] - '0';
            cipherTextXEncSym[i] = tA[num+1];
           // printf("%c: %d: %c\n", cipherTextXEncSym[i], num, tA[num+1]);
        }else{
            //printf("C");
            cipherTextXEncSym[i] = cipherText[i];
            // printf("%c: %d: %c\n", cipherTextXEncSym[i], num, tA[num+1]);
        }
    }
}

void sendCipherTextEncSym(){
    FILE *fileAddress;
    fileAddress = fopen(cipherTextFile, "w");
    if (fileAddress != NULL){
        for(int i=0; i< strlen(cipherTextXEncSym); i++){
            fputc(cipherTextXEncSym[i], fileAddress);
        }
        fclose(fileAddress);
    }else{
  	  printf("\n Unable to Create CipherText File");
    }
}
void encrypt()
{
    // bool key_status = false;
    /*while(! key_status){
         // step 1: Choose an invertible n*n-matrix K as key with n >= 5 and n = 6 as default, at which K must hold: gcd(det(K), 26) 6= 1.
    generateKeyMatrix();
        // step 2: Compute s as follows: Sum up the elements of the main diagonal of K
    key_status = verifyKeyMatrix();
    };*/

    // step 1: Read the key file and create an array out of it and verify the determinant
    printf("\nReading key file into matrix....!\n");
    readKeyFile(keyFile);

    printf("\nCalculating determinant and verifying GCD(det(k), 26) != 1\n");
    if(verifyKeyDeterminant() == 0)
    {
        printf("\nEnter a new key file....\n");
        exit(0);
    }
    printf("\nKey matrix ok....!\n");


    // step 2: verifyKeyMatrix;
    printf("\nVerifying primary diagonal Sum..!");
    keyMatrixSum = calculateSumOfMainDiagonal(key, N);
    if(keyMatrixSum == 0)
    {
        printf("\nVerifying secondary diagonal Sum..!");
        keyMatrixSum = calculateSumOfSecondaryDiagonal(key, N);
        if(keyMatrixSum == 0)
        {
            printf("\nEnter a new key file...!\n");
            exit(0);
        }
    }
    printf("\nSum: %d\n", keyMatrixSum);

    // step 3: Compute the substitution table TP for the plainText-character alphabet
    printf("\nComputing the substitution table matrix\n");
    createTP();

    // step 4: Divide the plainText into blocks of n characters.
    printf("\nCreate matrix of plaintext...!\n");
    createPlainTextMatrix();

    // step 5: Replace the plainText characters with numbers using the substitution table TP. So you get the plainText-number vector P.

    printf("\nReplacing the plaintext characters with numbers using the substitution table TP.\n");
    createPlainTextNumberMatrix();

    // step 6: Multiply K by P to get the vector C1: C1 = K * P

    printf("\nMultiplying K by P to get the vector C1: C1 = K * P\n");
    matrixMultiplication();

    // step 7: Compute the cipherText vector C = C1 mod 26.

    printf("\nComputing the cipherText vector C\n..!");
    cipherTextVectorC();

    // step 8: Substitute the elements in C using the substitution table TC for the cipherText alphabet: 0=Z, 1=A, 2=B, ..., 24=X, 25=Y.

    printf("\nSubstitute the elements in C using the substitution table TC for the cipherText alphabet: 0=Z, 1=A, 2=B, ..., 24=X, 25=Y.\n");

    createTCAndCipherText();
    for(int i = 0; i < M; ++i)
	{
		for(int j = 0; j < N; ++j)
		{
			printf("%c\t", cipherTextCharMatrixC[i][j]);
		}
		printf("\n");
	}
    //step 9: Compute the "extension vector" X

    printf("\nCompute the 'extension vector' X as follows: Divide C1 by 26 and round up the value: X = upperbound (C1 / 26)\n");

    createExtensionVectorX();

    // step 10: The "extension matrix" E results by adding the value s to each element of K: E = s * I + K (I is the one matrix)

    printf("\nThe 'extension matrix' E results by adding the value s to each element of K: E = s * I + K (I is the one matrix)\n");

    createExtensionMatrix();

    //step 11: Compute the "encrypted extensions" XEnc

    printf("\nCompute the 'encrypted extensions' XEnc by multiplying E by X: XEnc = E * X\n");
    createEncryptedExtensionsXEnc();

    // step 12:  Join all cipher letters with its encrypted extensions: C1 XEnc1 ... C6 XEnc6
    printf("\nJoin all cipher letters with its encrypted extensions: C1 XEnc1 ... C6 XEnc6\n");
    joinCipherWithExtension();

    for(int i = 0; i < cipherTextLength; i++){
        printf("%c", cipherText[i]);
    }
    printf("\n");
    // step 13a: Compute the sum t of all elements of the key matrix K mod 11.

    // step 13b: the substitution table TA for additional symbols

    createAdditionalSymbolTable();

    // step 14: Substitute XEnc using the substitution table TA.

    printf("\nSubstitute XEnc using the substitution table TA. So you get XEncSym.\n");
    createXEncSym();
    printf("\n");

    for(int i=0; i< strlen(cipherTextXEncSym); i++){
        printf("%c", cipherTextXEncSym[i]);
    }

    // step 15: ( C1 XEncSym1 ... C6 XEncSym6 )
    sendCipherTextEncSym();

}

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("Number of argument is different");
    }
    else
    {
        strcpy(plainTextFile, argv[1]);
        strcpy(keyFile, argv[2]);
        strcpy(cipherTextFile, argv[3]);
        // printf("%s%s%s", plainText)
        readPlainTextFile(plainTextFile);
        encrypt();
    }


    return 0;
}
