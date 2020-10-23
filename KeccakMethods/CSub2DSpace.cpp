#include "CSub2DSpace.h"
#include "MatrixOps.h"
#include <stdio.h>



CSub2DSpace::CSub2DSpace()
{
}


CSub2DSpace::~CSub2DSpace()
{
}


int CSub2DSpace::Init(int *x)
{
	// TODO: 在此处添加实现代码.
	for (int i = 0; i < 4; i++)
	{
		baseVal[i] = x[i];
	}

	// 生成由3个元素组成的基础矩阵；
	GenBaseMatrix();

	// 如果有从4个中确定一个，还要增加2个条件，我们的AdditionalVector矩阵中，每2行配合前面3行都可以用来确定一个数。
	// 这里，我们就是要先准备好这8个向量。
	GenAdditionalVec();

	/*
	for (int i = 0; i < 3; i++)
	{
		int nWeight = 0;
		for (int j = 0; j < 6; j++)
		{
			printf("%d ", baseMatrix[i][j]);
			
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 5; i++)
		printf("%d ", baseMatMainElem[i]);
	printf("\n\n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			printf("%d ", additionalVectors[i][j]);

		}
		printf("\n");
	}

	*/
	return 0;
}


int CSub2DSpace::GenBaseMatrix()
{
	// TODO: 在此处添加实现代码.
		// TODO: 在此处添加实现代码.
	// 如果是2维空间，那么必然有3个约束条件，我们就是要根据值的规律来判断出其约束条件
/*
思路，我们需要对下例进行判断。首先判断每一行，自己能不能成。
比如0，3，8，b
0 0 0 0
0 0 1 1
0 0 0 0
0 1 0 1
0 1 0 1
这个例子中，第一行和第三行天然形成两个条件，x4+x5=0 成为另一个条件。因此，这是一个2维子空间。
*/
// 依次判断一行作用时的约束，2行作用时的约束，3行作用时的约束
	int nBit[5][4] = { 0 };
	for (int j = 0; j < 4; j++)
	{
		nBit[0][j] = (baseVal[j] >> 4) & 1;
		nBit[1][j] = (baseVal[j] >> 3) & 1;
		nBit[2][j] = (baseVal[j] >> 2) & 1;
		nBit[3][j] = (baseVal[j] >> 1) & 1;
		nBit[4][j] = (baseVal[j]) & 1;
	}

	// 定义一个方程组M(5*6)，用来保存约束条件
	byte **M;
	M = new byte*[15];
	for (int i = 0; i < 15; i++)
	{
		M[i] = new byte[6];
	}
	for (int i = 0; i < 15; i++)
		for (int j = 0; j < 6; j++)
			M[i][j] = 0;

	int mCnt = 0;

	// 从5个中选取1个
	for (int i = 0; i < 5; i++)
	{
		if (nBit[i][0] == nBit[i][1] && nBit[i][0] == nBit[i][2] && nBit[i][0] == nBit[i][3])
		{
			M[mCnt][i] = 1;
			M[mCnt][5] = nBit[i][0];
			mCnt++;
		}
	}
	// 从5个中选2个
	for (int i = 0; i < 4; i++)
	{
		for (int j = i + 1; j < 5; j++)
		{
			int xor0 = nBit[i][0] ^ nBit[j][0];
			int xor1 = nBit[i][1] ^ nBit[j][1];
			int xor2 = nBit[i][2] ^ nBit[j][2];
			int xor3 = nBit[i][3] ^ nBit[j][3];
			if (xor0 == xor1 && xor0 == xor2 && xor0 == xor3)
			{
				M[mCnt][i] = 1;
				M[mCnt][j] = 1;
				M[mCnt][5] = xor0;
				mCnt++;
			}

		}
	}

	// 从5个中选3个
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			for (int k = j + 1; k < 5; k++)
			{
				int xor0 = nBit[i][0] ^ nBit[j][0] ^ nBit[k][0];
				int xor1 = nBit[i][1] ^ nBit[j][1] ^ nBit[k][1];
				int xor2 = nBit[i][2] ^ nBit[j][2] ^ nBit[k][2];
				int xor3 = nBit[i][3] ^ nBit[j][3] ^ nBit[k][3];
				if (xor0 == xor1 && xor0 == xor2 && xor0 == xor3)
				{
					M[mCnt][i] = 1;
					M[mCnt][j] = 1;
					M[mCnt][k] = 1;
					M[mCnt][5] = xor0;
					mCnt++;
				}
			}
		}
	}

	// 传进来的肯定是秩为3的。
	int nMainElem[15] = { 0 };
	int nDependNum[15] = { 0 };
	
	int **nDependList;
	nDependList = new int*[15];
	for (int i = 0; i < 15; i++)
	{
		nDependList[i] = new int[15];
		
	}
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			nDependList[i][j] = 0;
		}
	}

	int nWeightList[15] = { 0 };
	MatrixOps::DiagonalFormat(M, mCnt, 6, nMainElem, nWeightList, nDependNum, nDependList);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			baseMatrix[i][j] = M[i][j];
		}
	}
	for (int i = 0; i < 5; i++)
	{
		baseMatMainElem[i] = nMainElem[i];
	}

	for (int i = 0; i < 15; i++)
	{
		delete[] M[i];
		delete[] nDependList[i];
	}
	delete M;
	delete nDependList;

	return 0;
}


int CSub2DSpace::GenAdditionalVec()
{
	// TODO: 在此处添加实现代码.
	// 为每个元素添加2个向量，加上baseMat中的3个向量，就能够唯一地确定一个只有一个元素的子空间了。
	// 计算这些额外向量，主要是依靠之前保存的主元位置。
	bool bPosUsed[5] = { 0 };

	for (int i = 0; i < 3; i++)
	{
		bPosUsed[baseMatMainElem[i]-1] = true;
	}

	int nCnt = 0;
	for (int i = 0; i < 4; i++)
	{
		// 针对baseVal[i];
		for (int j = 0; j < 5; j++)
		{
			if (bPosUsed[j] == false)
			{
				// 利用baseVal[i]的第j位构建方程
				additionalVectors[nCnt][j] = 1;
				additionalVectors[nCnt][5] = (baseVal[i]>>(4-j))&1;
				nCnt++;
			}
		}


	}

	return 0;
}
