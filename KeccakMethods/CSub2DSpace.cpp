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
	// TODO: �ڴ˴����ʵ�ִ���.
	for (int i = 0; i < 4; i++)
	{
		baseVal[i] = x[i];
	}

	// ������3��Ԫ����ɵĻ�������
	GenBaseMatrix();

	// ����д�4����ȷ��һ������Ҫ����2�����������ǵ�AdditionalVector�����У�ÿ2�����ǰ��3�ж���������ȷ��һ������
	// ������Ǿ���Ҫ��׼������8��������
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
	// TODO: �ڴ˴����ʵ�ִ���.
		// TODO: �ڴ˴����ʵ�ִ���.
	// �����2ά�ռ䣬��ô��Ȼ��3��Լ�����������Ǿ���Ҫ����ֵ�Ĺ������жϳ���Լ������
/*
˼·��������Ҫ�����������жϡ������ж�ÿһ�У��Լ��ܲ��ܳɡ�
����0��3��8��b
0 0 0 0
0 0 1 1
0 0 0 0
0 1 0 1
0 1 0 1
��������У���һ�к͵�������Ȼ�γ�����������x4+x5=0 ��Ϊ��һ����������ˣ�����һ��2ά�ӿռ䡣
*/
// �����ж�һ������ʱ��Լ����2������ʱ��Լ����3������ʱ��Լ��
	int nBit[5][4] = { 0 };
	for (int j = 0; j < 4; j++)
	{
		nBit[0][j] = (baseVal[j] >> 4) & 1;
		nBit[1][j] = (baseVal[j] >> 3) & 1;
		nBit[2][j] = (baseVal[j] >> 2) & 1;
		nBit[3][j] = (baseVal[j] >> 1) & 1;
		nBit[4][j] = (baseVal[j]) & 1;
	}

	// ����һ��������M(5*6)����������Լ������
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

	// ��5����ѡȡ1��
	for (int i = 0; i < 5; i++)
	{
		if (nBit[i][0] == nBit[i][1] && nBit[i][0] == nBit[i][2] && nBit[i][0] == nBit[i][3])
		{
			M[mCnt][i] = 1;
			M[mCnt][5] = nBit[i][0];
			mCnt++;
		}
	}
	// ��5����ѡ2��
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

	// ��5����ѡ3��
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

	// �������Ŀ϶�����Ϊ3�ġ�
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
	// TODO: �ڴ˴����ʵ�ִ���.
	// Ϊÿ��Ԫ�����2������������baseMat�е�3�����������ܹ�Ψһ��ȷ��һ��ֻ��һ��Ԫ�ص��ӿռ��ˡ�
	// ������Щ������������Ҫ������֮ǰ�������Ԫλ�á�
	bool bPosUsed[5] = { 0 };

	for (int i = 0; i < 3; i++)
	{
		bPosUsed[baseMatMainElem[i]-1] = true;
	}

	int nCnt = 0;
	for (int i = 0; i < 4; i++)
	{
		// ���baseVal[i];
		for (int j = 0; j < 5; j++)
		{
			if (bPosUsed[j] == false)
			{
				// ����baseVal[i]�ĵ�jλ��������
				additionalVectors[nCnt][j] = 1;
				additionalVectors[nCnt][5] = (baseVal[i]>>(4-j))&1;
				nCnt++;
			}
		}


	}

	return 0;
}
