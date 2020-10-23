#include "MatrixOps.h"
#include<stdio.h>



MatrixOps::MatrixOps()
{
}


MatrixOps::~MatrixOps()
{
}


unsigned int MatrixOps::Rank(byte **G, int nRow, int nCol)
{
	// TODO: 在此处添加实现代码.
	int **M = new int*[nRow];
	for (int j = 0; j < nRow; j++)
	{
		M[j] = new int[nCol];
	}


	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			//printf("%d\t", *((GFType*)G + nCol*i + j));
			// M[i][j] = *((GFType*)G + nCol*i + j);
			M[i][j] = *(*(G + i) + j);
		}
		// printf("\n");
	}
	// printf("\n");
/*
	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			TRACE("%d\t", M[i][j]);
		}
		TRACE("\n");
	}
	TRACE("\n");
*/
// Define a variable to record the position of the main element.
	int yPos = 0;

	for (int i = 0; i < nRow; i++)
	{
		// Find the main element which must be non-zero.

		bool bFind = false;

		for (int x = yPos; x < nCol; x++)
		{
			for (int k = i; k < nRow; k++)
			{
				if (M[k][x] != 0)
				{
					// Exchange the two vectors.
					for (int x = 0; x < nCol; x++)
					{
						int nVal = M[i][x];
						M[i][x] = M[k][x];
						M[k][x] = nVal;
					}										// We have exchanged the two vectors.	
					bFind = true;
					break;
				}
			}
			if (bFind == true)
			{
				yPos = x;
				break;
			}
		}

		for (int j = i + 1; j < nRow; j++)
		{
			// Now, the main element must be nonsingular.
			if (M[j][yPos] == M[i][yPos])	//	主元为1
			{
				for (int z = 0; z < nCol; z++)
				{
					M[j][z] = M[j][z] ^ M[i][z];
				}
			}


			/*
			byte temp = M[j][yPos]/ M[i][yPos];
			for (int z = 0; z < nCol; z++)
			{
				M[j][z] = M[j][z] ^ temp* M[i][z];
			}
			*/
		}
		// 
		yPos++;

	}

	// The matrix becomes a scalar matrix. we need to make more elements become 0 with elementary transformations.
	yPos = 0;
	for (int i = 1; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			if (M[i][j] != 0)
			{
				// the main element is found.
				yPos = j;
				break;
			}
		}
		for (int k = 0; k < i; k++)
		{
			if (M[k][yPos] == M[i][yPos])
			{
				for (int z = 0; z < nCol; z++)
				{
					M[k][z] = M[k][z] ^ M[i][z];
				}
			}
		}
	}

//	DisplayMat((byte**)M, nRow, nCol);

	int nRank = 0;
	// Get the rank.
	for (int i = 0; i < nRow; i++)
	{
		int nNonzero = 0;
		for (int j = 0; j < nCol; j++)
		{
			if (M[i][j] != 0)
			{
				nNonzero++;
			}

		}
		// If there is only one nonzero element in the new matrix, it is concluded an original packet is leaked. 
		if (nNonzero > 0)
		{
			// Leaked.
			nRank++;
		}
	}



	for (int i = 0; i < nRow; i++)
	{
		delete[]  M[i];
	}
	delete[] M;

	return nRank;
}


int MatrixOps::MatMul(byte **A, int nRowA, int nColA, byte **B, int nRowB, int nColB, byte **C)
{
	// TODO: 在此处添加实现代码.
	// Precondition: nColA must equal to nRowB.
	if (nColA != nRowB)
	{
		return 0;
	}

	for (int i = 0; i < nRowA; i++)
	{
		for (int j = 0; j < nColB; j++)
		{
			byte sum = 0;
			for (int k = 0; k < nColA; k++)		// nColA = nRowB
			{
				sum = sum ^ A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}

	return 1;
}


int MatrixOps::DisplayMat(byte **M, int nRow, int nCol)
{
	// TODO: 在此处添加实现代码.
	// TODO: 在此处添加实现代码.
	for (int i = 0; i < nRow; i++)
	{
		int nWeight = 0;
		for (int j = 0; j < nCol; j++)
		{
			printf("%d ", M[i][j]);
			if (M[i][j] == 1)
			{
				nWeight++;
			}
		}
		printf("  Weight: %d\n", nWeight);
	}
	return 0;
}


int MatrixOps::DiagonalFormat(byte **M, int nRow, int nCol, int *MainElem, int *nRowWeight, int *nDependNum, int **nDependList)
{
	// TODO: 在此处添加实现代码.
	// Define a variable to record the position of the main element.
	int yPos = 0;

	for (int i = 0; i < nRow; i++)
	{
		// Find the main element which must be non-zero.

		bool bFind = false;

		for (int x = yPos; x < nCol; x++)
		{
			for (int k = i; k < nRow; k++)
			{
				if (M[k][x] != 0)
				{
					// Exchange the two vectors.
					for (int x = 0; x < nCol; x++)
					{
						int nVal = M[i][x];
						M[i][x] = M[k][x];
						M[k][x] = nVal;
					}										// We have exchanged the two vectors.	
					bFind = true;
					break;
				}
			}
			if (bFind == true)
			{
				yPos = x;
				MainElem[i] = yPos +1;		// 主元的下标是从1开始的。
				break;
			}
		}
		

		for (int j = i + 1; j < nRow; j++)
		{
			// Now, the main element must be nonsingular.
			if (M[j][yPos] == M[i][yPos])	//	主元为1
			{
				for (int z = 0; z < nCol; z++)
				{
					M[j][z] = M[j][z] ^ M[i][z];
				}
			}


			/*
			byte temp = M[j][yPos]/ M[i][yPos];
			for (int z = 0; z < nCol; z++)
			{
				M[j][z] = M[j][z] ^ temp* M[i][z];
			}
			*/
		}
		// 
		yPos++;

	}

	// The matrix becomes a scalar matrix. we need to make more elements become 0 with elementary transformations.
	yPos = 0;
	for (int i = 1; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			if (M[i][j] != 0)
			{
				// the main element is found.
				yPos = j;
				break;
			}
		}
		for (int k = 0; k < i; k++)
		{
			if (M[k][yPos] == M[i][yPos])
			{
				for (int z = 0; z < nCol; z++)
				{
					M[k][z] = M[k][z] ^ M[i][z];
				}
			}
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		int nCnt = 0;
		bool bFindMain = false;
		int nMainPos = -1;
		for (int j = 0; j < nCol - 1; j++)
		{
			if (bFindMain == false && M[i][j] == 1)
			{
				bFindMain = true;
				nMainPos = j;
				nDependNum[j]++;
			}

			if (M[i][j] == 1)
			{
				nDependList[nMainPos][nCnt++] = j;	// 记住关联的序号。
			}
			nRowWeight[i]++;
		}
	}

/*
	DisplayMat(M, nRow, nCol);
	for (int i = 0; i < nRow; i++)
	{
		printf("%d= %d\n", i, MainElem[i]);
	}
*/	
	return 0;
}


int MatrixOps::OutputMatToFile(byte **M, int nRow, int nCol)
{
	// TODO: 在此处添加实现代码.
	FILE *fp = NULL;
	fp = fopen("mat.txt", "w");
	for (int i = 0; i < nRow; i++)
	{
		int nWeight = 0;
		for (int j = 0; j < nCol; j++)
		{
			fprintf(fp, "%d", M[i][j]);
			if (M[i][j] == 1)
			{
				nWeight++;
			}
			
		}
		fprintf(fp, " %3d", nWeight);
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}
