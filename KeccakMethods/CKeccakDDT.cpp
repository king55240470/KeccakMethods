#include "CKeccakDDT.h"
#include <math.h>
#include "stdio.h"
#include "MatrixOps.h"



CKeccakDDT::CKeccakDDT()
{
	// Initialize the DDT
	// �г�����DDT
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			// ������Ϊi, ������Ϊj,����ÿ�����һ�����ٸ�
			// S��������һ��32��
			for (int z = 0; z < 32; z++)
			{
				// ���β���ÿһ��
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
	// TODO: �ڴ˴����ʵ�ִ���.
	int nOutV = 0;
	int M[5] = { 0 };
	int outM[5] = { 0 };

	for (int k = 0; k < 5; k++)
	{
		M[4 - k] = nM & 1;
		nM >>= 1;
		//    printf("%d ", M[k]);
	}
	// ����outM
	for (int k = 0; k < 5; k++)
	{
		outM[k] = M[k] ^ ((M[(k + 1) % 5] ^ 1)&M[(k + 2) % 5]);
		nOutV = nOutV + outM[k] * pow(2, 4 - k);
	}
	return nOutV;
}


int CKeccakDDT::DisplayDDT()
{
	// TODO: �ڴ˴����ʵ�ִ���.

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
	// TODO: �ڴ˴����ʵ�ִ���.
	int nCnt = 0;
	for (int z = 0; z < 32; z++)
	{
		// ���β���ÿһ��
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