#include "CKeccakDDT.h"
#include <math.h>
#include "stdio.h"
#include "MatrixOps.h"



CKeccakDDT::CKeccakDDT()
{
	// Initialize the DDT
	// 列出所有DDT
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			// 输入差分为i, 输出差分为j,测试每类组合一共多少个
			// S盒中输入一共32种
			for (int z = 0; z < 32; z++)
			{
				// 依次测试每一个
				int zb = z ^ i;
				int zaRes = SBox(z);
				int zbRes = SBox(zb);

				if ((zaRes^zbRes) == j)
				{
					DDT[i][j]++;
				}
			}
		}
	}

}


CKeccakDDT::~CKeccakDDT()
{
}


int CKeccakDDT::SBox(int nM)
{
	// TODO: 在此处添加实现代码.
	int nOutV = 0;
	int M[5] = { 0 };
	int outM[5] = { 0 };

	for (int k = 0; k < 5; k++)
	{
		M[4 - k] = nM & 1;
		nM >>= 1;
		//    printf("%d ", M[k]);
	}
	// 计算outM
	for (int k = 0; k < 5; k++)
	{
		outM[k] = M[k] ^ ((M[(k + 1) % 5] ^ 1)&M[(k + 2) % 5]);
		nOutV = nOutV + outM[k] * pow(2, 4 - k);
	}
	return nOutV;
}


int CKeccakDDT::DisplayDDT()
{
	// TODO: 在此处添加实现代码.

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			printf("%2d ", DDT[i][j]);
		}
		printf("\n");
	}
	return 0;
}


int CKeccakDDT::GetMsgPairListFromDiff(int nInDiff, int nOutDiff, int **msgPairList)
{
	// TODO: 在此处添加实现代码.
	int nCnt = 0;
	for (int z = 0; z < 32; z++)
	{
		// 依次测试每一个
		int zb = z ^ nInDiff;
		int zaRes = SBox(z);
		int zbRes = SBox(zb);

		if ((zaRes^zbRes) == nOutDiff)
		{
			
			msgPairList[nCnt][0] = z;
			msgPairList[nCnt][1] = zb;
			msgPairList[nCnt][2] = zaRes;
			msgPairList[nCnt][3] = zbRes;
			nCnt++;
		}
	}

	return 0;
}