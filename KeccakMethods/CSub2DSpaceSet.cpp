#include "CSub2DSpaceSet.h"
#include "MatrixOps.h"
#include <stdio.h>
#include <stdlib.h>



CSub2DSpaceSet::CSub2DSpaceSet()
{
}

CSub2DSpaceSet::~CSub2DSpaceSet()
{
	delete[] pSpace;
}


// 用于判断是不是一个二维子空间
// 如果是2维空间，那么必然有3个约束条件，我们就是要根据值的规律来判断出其约束条件
int CSub2DSpaceSet::Check2DSubSpace(int *nVal)
{
	// TODO: 在此处添加实现代码.

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
		nBit[0][j] = (nVal[j] >> 4) & 1;
		nBit[1][j] = (nVal[j] >> 3) & 1;
		nBit[2][j] = (nVal[j] >> 2) & 1;
		nBit[3][j] = (nVal[j] >> 1) & 1;
		nBit[4][j] = (nVal[j]) & 1;
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

	int nRank = MatrixOps::Rank(M, mCnt, 5);

	for (int i = 0; i < 15; i++)
	{
		delete[] M[i];
	}
	delete M;

	if (nRank == 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;

}


int CSub2DSpaceSet::Init(int nOutDiff)
{
	// TODO: 在此处添加实现代码.

	// TODO: 在此处添加实现代码.
	int nRange[32] = { 0 };
	int nCnt = 0;
	for (int i = 0; i < 32; i++)
	{
		if (ddt.DDT[i][nOutDiff] != 0)
		{
			nRange[nCnt++] = i;
		}
	}
	// 每次取4个，这里要算组合数了
	int nSubSpace = 0;
	for (int a = 0; a < nCnt - 3; a++)
	{
		for (int b = a + 1; b < nCnt - 2; b++)
		{
			for (int c = b + 1; c < nCnt - 1; c++)
			{
				for (int d = c + 1; d < nCnt; d++)
				{
					// abcd
					int nVal[4] = { 0 };
					nVal[0] = nRange[a];
					nVal[1] = nRange[b];
					nVal[2] = nRange[c];
					nVal[3] = nRange[d];

					if (Check2DSubSpace(nVal) == 1)
					{
						//	TRACE("%d-%d-%d-%d\n", nVal[0], nVal[1], nVal[2], nVal[3]);
						nSubspaceList[nSubspaceNum][0] = nVal[0];
						nSubspaceList[nSubspaceNum][1] = nVal[1];
						nSubspaceList[nSubspaceNum][2] = nVal[2];
						nSubspaceList[nSubspaceNum][3] = nVal[3];

						nSubSpace++;
						nSubspaceNum++;

					}

				}
			}
		}
	}
	// printf("%d:%d nSubSpace:%d\n", nOutDiff, nCnt, nSubSpace);

	pSpace = new CSub2DSpace[nSubspaceNum];

	for (int i = 0; i < nSubspaceNum; i++)
	{
		pSpace[i].Init(nSubspaceList[i]);
	}
	// 随机选择一个子空间
//	RandomlyChooseSubSpace();
	return 0;
}


int CSub2DSpaceSet::RandomlyChooseSubSpace()
{
	// TODO: 在此处添加实现代码.
	int n = rand() % nSubspaceNum;
	// 拷贝baseMat
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			activeBaseMat[i][j] = pSpace[n].baseMatrix[i][j];
		}
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			activeAdditionalVec[i][j] = pSpace[n].additionalVectors[i][j];
		}
	}



	return 0;
}
