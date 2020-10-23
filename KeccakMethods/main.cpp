#include "stdio.h"
#include "CKeccakDDT.h"
#include "C2017ECrpt.h"
#include "MatrixOps.h"
#include "CSub2DSpace.h"
#include <stdlib.h>
#include <time.h>
int main()
{
//	CKeccakDDT ddt;
//	ddt.Get2DSubspaceNum(13);
	// ddt.DisplayDDT();

/*	CSub2DSpace ss;
	int val[4] = { 0,3,8,11 };
	ss.Init(val);
*/

	/*
		The following codes is to test the attack methods in 2017ECrypt
	*/
	C2017ECrypt ec17(1600);
	ec17.run();

	/*
	byte **M;
	srand(time(NULL));
	M = new byte*[1600];
	for (int j = 0; j < 5; j++)
	{
		M[j] = new byte[4];
	}
	M[0][0] = 1;
	M[0][1] = 0;
	M[0][2] = 1;
	M[0][3] = 1;

	M[1][0] = 0;
	M[1][1] = 1;
	M[1][2] = 0;
	M[1][3] = 1;

	M[2][0] = 1;
	M[2][1] = 1;
	M[2][2] = 1;
	M[2][3] = 0;

	M[3][0] = 0;
	M[3][1] = 0;
	M[3][2] = 1;
	M[3][3] = 0;

	M[4][0] = 0;
	M[4][1] = 0;
	M[4][2] = 1;
	M[4][3] = 1;

	

	int nRet = ec17.CheckConsistency(M,3, 4, 2);
	printf("return:%d.\n", nRet);
	*/
//	int mainpos[3] = { 0 };
//	int nRank = MatrixOps::DiagonalFormat(M, 4, 4, mainpos);

	/*
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			M[i][j] = rand() % 2;
			printf("%d ", M[i][j]);
		}
		printf("\n");
	}
	*/
//	int mainpos[10] = { 0 };
//	int nRank = MatrixOps::DiagonalFormat(M, 10, 10, mainpos);

	printf("\nover.");
	getchar();

	return 0;
}


/*
	// ²âÊÔÇóÖÈ
	byte **M;
	srand(time(NULL));
	M = new byte*[1600];
	for (int j = 0; j < 10; j++)
	{
		M[j] = new byte[10];
	}


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			M[i][j] = rand() % 2;
			printf("%d ", M[i][j]);
		}
		printf("\n");
	}
	int nRank = MatrixOps::DiagonalFormat(M, 10, 10);
*/


