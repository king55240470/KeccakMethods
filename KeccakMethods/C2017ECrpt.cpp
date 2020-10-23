#include "C2017ECrpt.h"
#include <time.h>
#include<stdlib.h>
#include <stdio.h>
#include "MatrixOps.h"
#include <ctime>
#include <windows.h>

C2017ECrypt::C2017ECrypt()
{
	printf("nothing");
}
C2017ECrypt::C2017ECrypt(int bLen)// b = r+c
{
	printf("helo");
	nAlpha2 = new byte[bLen];
	nAlpha1 = new byte[bLen];
	nBeta1 = new byte[bLen];
	nBeta0 = new byte[bLen];
	G = new byte*[1600];
	L = new byte*[1600];
	L_Inv = new byte*[1600];
	GL = new byte*[1600];
	nEqDelta = new byte*[2400];
	nEqMsg = new byte*[2400];
	nDependList = new int*[1600];

	
	for (int j = 0; j < 1600; j++)
	{
		G[j] = new byte[1600];
		L[j] = new byte[1600];
		L_Inv[j] = new byte[1600];
		GL[j] = new byte[1600];

		nEqDelta[j] = new byte[1601];		// ���һ�б��淽�̷���η��̵Ľ�
		nEqMsg[j] = new byte[1601];

		nDependList[j] = new int[1600];
	}

	for (int j = 0; j < 2400; j++)
	{
		nEqDelta[j] = new byte[1601];		// ���һ�б��淽�̷���η��̵Ľ�
		nEqMsg[j] = new byte[1601];
	}

	msgPairList = new int*[8];
	for (int i = 0; i < 8; i++)
	{
		msgPairList[i] = new int[4];
	}

	for (int i = 0; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			G[i][j] = 0;
			L[i][j] = 0;
			L_Inv[i][j] = 0;
			GL[i][j] = 0;
			nDependList[i][j] = 0;
		}	
	}


	for (int i = 0; i < 2400; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			nEqDelta[i][j] = 0;
			nEqMsg[i][j] = 0;
		}
		nEqDelta[i][1600] = 0;
		nEqMsg[i][1600] = 0;
	}

	b = bLen;

	// Initialize them to zero.
	for (int i = 0; i < bLen; i++)
	{
		nAlpha2[i] = 0;
		nAlpha1[i] = 0;
		nBeta1[i] = 0;
		nBeta0[i] = 0;
	}
	
}

C2017ECrypt::~C2017ECrypt()
{

	for (int i = 0; i < 1600; i++)
	{
		delete[]  G[i];
		delete[]  L[i];
		delete[]  GL[i];
		delete[]  nDependList[i];
	}

	for (int i = 0; i < 2400; i++)
	{
		delete[] nEqDelta[i];
		delete[] nEqMsg[i];
	}

	delete[] G;
	delete[] L;
	delete[] GL;
	delete[] nEqDelta;
	delete[] nEqMsg;
	delete[]  nDependList;
	
	// clear memory
	delete[] nAlpha2;
	delete[] nAlpha1;
	delete[] nBeta1;
	delete[] nBeta0;

	for (int i = 0; i < 8; i++)
	{
		delete[] msgPairList[i];
	}
	delete[] msgPairList;
}


int C2017ECrypt::InitAlpha2(int a2[], int nSize)
{
	// TODO: �ڴ˴����ʵ�ִ���.

	for (int i = 0; i < nSize; i++)
		nAlpha2[i] = a2[i];

	printf("### Alpha2 initialized from input file.\n");
	return 1;
}


int C2017ECrypt::RandBeta1()
{
	// TODO: �ڴ˴����ʵ�ִ���.

	// this is an randomized algorithm, so sow a seed first.
	srand(time(NULL));

	// Obtain Beta1 from Alpha2 by randomly choosing the input differential.
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			// ÿ��SBox��5���������
			outDiffBox[j * 64 + i] = (nAlpha2[j * 320 + i] << 4) + (nAlpha2[j * 320 + i + 64] << 3) + (nAlpha2[j * 320 + i + 128] << 2) + (nAlpha2[j * 320 + i + 192] << 1) + nAlpha2[j * 320 + i + 256];
		}
	}

	for (int i = 0; i < b / 5; i++)
	{
		if (outDiffBox[i] == 0)
		{
			inDiffBox[i] = 0;
		}
		else
		{
			// randomly choose one input differential, it should be 2-dimentional affine subspace.
			// ����DDT�ж�Ӧ�����ִ���4�ĸ�����Ȼ�����������ѡ��һ����
			int nCandidate = 0;
			int nPotentialInputDiff[32] = { 0 };
			for (int k = 0; k < 32; k++)
			{
				if (ddt.DDT[k][outDiffBox[i]] == 4)
				{
					nPotentialInputDiff[nCandidate++] = k;
				}
			}
			int nRandVal = rand() % nCandidate;

			inDiffBox[i] = nPotentialInputDiff[nRandVal];
		}
	}
	// ����inDiffBox֮���䱾�ʾ���beta1,����������Ҫ����ת��Ϊ1600ά������
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			int nVal = inDiffBox[i * 64 + j];
			nBeta1[i * 320 + j] = (nVal >> 4) & 1;
			nBeta1[i * 320 + j+64] = (nVal >> 3) & 1;
			nBeta1[i * 320 +j +128] = (nVal >> 2) & 1;
			nBeta1[i * 320 + j+192] = (nVal >> 1) & 1;
			nBeta1[i * 320 + j+256] = (nVal) & 1;
		}
	}
	// ���ˣ����ǻ����nBeta1;


	/*	output the randomly selected beta1
		for (int i = 0; i < 320; i++)
		{
			printf("%d  %d %d\n", i, inDiffBox[i], outDiffBox[i]);
		}
	*/

//	printf("### Randomly generated Beta1.\n");
	return 0;
}


int C2017ECrypt::InitGandM()
{
	// TODO: �ڴ˴����ʵ�ִ���.

	for (int i = 0; i < 320; i++)
	{
		
		if (inDiffBox[i] != 0)
		{

			int nColNum = i / 64;
			int nDepthNum = i % 64;

			// ���������ֺ������ּ�����Ӽ���Ȼ���ٴ�����Ӽ���Ѱ�ҳ�һ����ά�����Ӽ����������Ƴ�Լ������
			// �����Ӧ��DDTΪ4����4������ֵ�Զ���Ϊһ�������ӿռ䡣
			int nCnt = 0;
			int nSet[32] = { 0 };		// input set
//			printf("****\n");
//			printf("nCol=%d,nDepth=%d\n", nColNum, nDepthNum);
			for (int z = 0; z < 32; z++)
			{
				// ���β���ÿһ��
				int zb = z ^ inDiffBox[i];
				int zaRes = ddt.SBox(z);
				int zbRes = ddt.SBox(zb);

				if ((zaRes^zbRes) == outDiffBox[i])
				{
					nSet[nCnt++] = z;
//					printf("%d ", z);
				}
			}
//			printf("---%d %d\n", inDiffBox[i], outDiffBox[i]);
			

			int nCond = 0;	// condition
			bool bUsed[5] = { false };
			byte b[4][5] = { 0 };
			// �ҵ�4��Ԫ�صļ����ˣ���Ҫ����3������
			for (int k = 0; k < 5; k++)
			{
				b[0][k] = (nSet[0] >> (4 - k)) & 1;
				b[1][k] = (nSet[1] >> (4 - k)) & 1;
				b[2][k] = (nSet[2] >> (4 - k)) & 1;
				b[3][k] = (nSet[3] >> (4 - k)) & 1;

				// ��������ͬ����ͬ�ģ���ֱ��ȷ��һ������
				if (b[0][k] == b[1][k] && b[0][k] == b[2][k] && b[0][k] == b[3][k])
				{
//					printf("%d col is %d\n", k, b[0][k]);
					bUsed[k] = true;			//�������п��ǹ��ˡ�
					nCond++;

					// ΪG����Լ������,��k��ȫ����ͬ����Ϊ���̣�mӦΪb[0][k]��
					// �����box��Ӧ��5��Ԫ����1600��Ԫ���е�λ��
					// ��ʱ�ǵ�i��box
					
					G[nGcnt][nColNum * 320 + nDepthNum + k * 64] = 1;
					m[nGcnt] = b[0][k];
					nGcnt++;

				}
			}
/*
			for (int m = 0; m < 4; m++)
			{
				for (int n = 0; n < 5; n++)
				{
					printf("%d ", b[m][n]);
				}
				printf("\n");
			}
*/
			int pos[5] = { 0 };
			int ncnt = 0;
			for (int k = 0; k < 5; k++)
			{
				if (bUsed[k] == false)
				{
					pos[ncnt++] = k;
				}
			}
			int v1 = pos[0];
			int v2 = pos[1];
			int v3 = pos[2];
			int v4 = pos[3];

			if (nCond == 2)//maybe 1,2,3
			{
				if (b[0][v1] == b[0][v2] && b[1][v1] == b[1][v2] && b[2][v1] == b[2][v2] && b[3][v1] == b[3][v2])
				{
//					printf("%d+%d=0\n", v1, v2);
					nCond++;

					G[nGcnt][nColNum * 320 + nDepthNum + v1 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v2 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;
				}
				else if(b[0][v1] == b[0][v3] && b[1][v1] == b[1][v3] && b[2][v1] == b[2][v3] && b[3][v1] == b[3][v3])
				{
//					printf("%d+%d=0\n", v1, v3);
					nCond++;

					G[nGcnt][nColNum * 320 + nDepthNum + v1 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v3 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;
				}
				else if (b[0][v2] == b[0][v3] && b[1][v2] == b[1][v3] && b[2][v2] == b[2][v3] && b[3][v2] == b[3][v3])
				{
//					printf("%d+%d=0\n", v2, v3);
					nCond++;

					G[nGcnt][nColNum * 320 + nDepthNum + v2 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v3 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;
				}
				
			}
			else if (nCond == 1)	// need two more conditions
			{
				if (b[0][v1] == b[0][v2] && b[1][v1] == b[1][v2] && b[2][v1] == b[2][v2] && b[3][v1] == b[3][v2])
				{
//					printf("%d+%d=0\n", v1, v2);
//					printf("%d+%d=0\n", v3, v4);
					nCond=nCond+2;

					G[nGcnt][nColNum * 320 + nDepthNum + v1 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v2 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;

					G[nGcnt][nColNum * 320 + nDepthNum + v3 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v4 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;

				}
				else if(b[0][v1] == b[0][v3] && b[1][v1] == b[1][v3] && b[2][v1] == b[2][v3] && b[3][v1] == b[3][v3])
				{
//					printf("%d+%d=0\n", v1, v3);
//					printf("%d+%d=0\n", v2, v4);
					nCond = nCond + 2;

					G[nGcnt][nColNum * 320 + nDepthNum + v1 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v3 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;

					G[nGcnt][nColNum * 320 + nDepthNum + v2 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v4 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;
				}
				else if (b[0][v1] == b[0][v4] && b[1][v1] == b[1][v4] && b[2][v1] == b[2][v4] && b[3][v1] == b[3][v4])
				{
//					printf("%d+%d=0\n", v1, v4);
//					printf("%d+%d=0\n", v2, v3);
					nCond = nCond + 2;

					G[nGcnt][nColNum * 320 + nDepthNum + v1 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v4 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;

					G[nGcnt][nColNum * 320 + nDepthNum + v2 * 64] = 1;
					G[nGcnt][nColNum * 320 + nDepthNum + v3 * 64] = 1;
					m[nGcnt] = 0;
					nGcnt++;
				}
				
			}

		}
	}
	/*
	for (int i = 0; i < nGcnt; i++)
	{
		printf("\n%d\n", i);
		for (int j = 0; j < 1600; j++)
		{
			printf("%d", G[i][j]);
		}
	}
	*/

//	printf("### matrix G and vector m have been obtained.\n");
	return 0;
}


int C2017ECrypt::Load_L_Matix()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	
	// Load matrix L from txt.
	FILE *fp = NULL;
	fp = fopen("L.txt", "rb");
	for (int i = 0; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			fscanf(fp, "%d ", &L[i][j]);
		}
		fscanf(fp, "\n");
	}
	fclose(fp);

	printf("### matrix L obtained.\n");

	// Load matrix L_Inv from txt.
	FILE *fp2 = NULL;
	fp2 = fopen("L_Inv.txt", "rb");
	for (int i = 0; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			fscanf(fp2, "%d ", &L_Inv[i][j]);
		}
		fscanf(fp2, "\n");
	}
	fclose(fp2);

	printf("### matrix L_Inv obtained.\n");
	return 0;
}


int C2017ECrypt::MultiplyGwithL()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// This function is used to obtain the product of G*L

	int nRet = MatrixOps::MatMul(G, nGcnt, 1600, L, 1600, 1600, GL);
	
//	MatrixOps::DisplayMat(GL, nGcnt, 1600);
//	int nRank = MatrixOps::Rank(GL, nGcnt, 1600);
//	printf("Rank is %d.", nRank);

//	printf("### G * L obtained.\n");

	if (nRet == 0)
	{
		return 0;
	}
	else {
		return 1;
	}
}




int C2017ECrypt::GetAlpha1FromBeta1()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// ֻ��Ҫ�����Ϳ�����
//	printf("Getting alpha1.\n");
	for (int i = 0; i < 1600; i++)
	{
		int sum = 0;
		for (int j = 0; j < 1600; j++)
		{
			sum = sum ^ (L_Inv[i][j] * nBeta1[j]);
		}
		nAlpha1[i] = sum;
//		printf("%d ", nAlpha1[i]);
	}

	// ��Alpha1��box��ʽ˳����á�
	FromVectorToSBox(nAlpha1, 1600, nAlpha1_BoxList);


//	printf("### Alpha1 obtained by multiplying L_Inv*Beta1.\n");

	return 0;
}


int C2017ECrypt::InitEqDeltaAndEqMsg()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// ��ʼ��EquationDelta, �÷����ǹ��ڲ�ֵģ�������ԭʼ�����֣����Ǿ���һ��L�任֮��Ĳ���أ���
	// The last c+1 difference must be zero, so the last c+1 lines of L_Inv*beta0 must be zero.
	// c+1 = 513, ��ô��

	nEqDelCnt = 513;
	for (int i = 1087; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			nEqDelta[i - 1087][j] = L_Inv[i][j];
		}
		nEqDelta[i-1087][1600] = 0;		// �����513���ز�־�Ϊ0.
	}

	// ������ڶ���Լ��������ͼ��ˣ�����alpha1�����е����ݣ������еķǻ�Ծ���Ӷ�Ӧ�Ĳ��ֱ����Ϊ0.���ڱ�������
	// �˴��Ĳ�֣���ˣ�����ֱ�����Ӧ5��λ�õĲ��ֵΪ0.
	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i] == 0)
		{
			// ����Ӧ��5��λ�õĲ����Ϊ0�����������5�����̣���5��������ÿ��ֻ��һ��1.
			// �жϸ�box��λ��
			int pos[5] = { 0 };
			FromBoxIdToBitPos(i, pos);

			// ��ʼ�ӷ���
			for (int j = 0; j < 5; j++)
			{
				nEqDelta[nEqDelCnt++][pos[j]] = 1;
			}
		}
	}


	// ��ʼ��nEquationMsg
	// ���c+1��ȷ���ģ���c+1���еĵ�һ��Ϊ1������Ϊ0���������ܵõ�c+1�����̡�
	nEqMsgCnt = 513;
	
	// ��һ������
	nEqMsg[0][1087] = 1;
	nEqMsg[0][1600] = 1;			// ������

	for (int i = 1088; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			nEqMsg[i - 1088][j] = L_Inv[i][j];
		}
		nEqDelta[i-1087][1600] = 0;		// �����513���ز�־�Ϊ0.
	}

	// ������������ת��Ϊ�����ʽ����Ŀ���Ǻ����ж������Ե�ʱ�򣬷��㡣

	MatrixOps::DiagonalFormat(nEqDelta, nEqDelCnt, 1601, nMainEleColPosInEqDelta, nEqDeltaW, nDependNum, nDependList);

	nEDeltaAfterInit = nEqDelCnt;

	MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt, 1601);

//	printf("### E_Delta and E_M initialized.\n");
	return 0;
}


int C2017ECrypt::run()
{
	// TODO: �ڴ˴����ʵ�ִ���.

//	Load_L_Matix();			// ���L��L��
//	InitGandM();			// ����beta1��alpa2
//	MultiplyGwithL();		// ���������G�ͻ�������L

	LoadAlpha2FromText();		// ���Alpha2
	InitSpaceSets();

	// ���ݻ������ݿ�ʼ�㷨
	
	RandBeta1();			// ����Alpha2 ������Beta1
	
	GetAlpha1FromBeta1();	// ����beta1��L_Inv
	InitEqDeltaAndEqMsg();	// ��ʼ��ʱ����Ҫ�õ�Alpha1���������ݣ������0��֣���ô�����ȻҲ��0��Ҳ�ͳ���Լ��

		FindBetaZero();

	return 0;
}

int C2017ECrypt::FromVectorToSBox(byte *vec, int nVecLen, int *BoxList)
{
	int nDepth = nVecLen / 25;		// nVecLen=b
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < nDepth; j++)
		{
			BoxList[i * 64 + j] = (vec[i * 320 + j] << 4) + (vec[i * 320 + j + 64] << 3) + (vec[i * 320 + j + 128] << 2) + (vec[i * 320 + 192] << 1) + vec[i * 320 + j + 256];
		}
	}

/*
	int nZeroNum = 0;
	for (int i = 0; i < 320; i++)
	{
		printf("%d: %d\n", i, nAlpha1_BoxList[i]);
		if (nAlpha1_BoxList[i] == 0)
		{
			nZeroNum++;
		}
	}
	printf("%d zeros in nAlpha1Box.\n", nZeroNum);
*/
	return 1;
}



int C2017ECrypt::FromBoxIdToBitPos(int id, int *posList)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int i = id / 64;
	int j = id % 64;

	posList[0] = i * 320 + j;
	posList[1] = i * 320 + j + 64;
	posList[2] = i * 320 + j + 128;
	posList[3] = i * 320 + j + 192;
	posList[4] = i * 320 + j + 256;

	return 0;
}

int C2017ECrypt::FindBeta0()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// Now, it is time to find suitable beta0.���ﲻ��ѡ������������ȥ�ҡ�
	// What we need to do is to determine nBeta0_BoxList, in the meanwhile, E_Delta is consistent.
	// 320 sboxes in nAlpha1_BoxList have been obtained.
/*
	int initEqCnt = nEqDelCnt;
	for (int i = 1; i < 32; i++)
	{
		TwoDspaceSet[i].RandomlyChooseSubSpace();
	}

	int nNonZero = 0;
	DWORD dwStart = GetTickCount();
	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i] != 0)
		{
			nNonZero++;
			int k = nAlpha1_BoxList[i]; // ���Ǿ�ѡTwoDspaceSet[k]��activeBaseMat
			int ii = i / 64;
			int jj = i % 64;

			int nTempPos = nEqDelCnt;
			for (int m = 0; m < 3; m++)
			{
				// ��3*6��activeBaseMatת��Ϊ 3*1600�ģ�

				for (int n = 0; n < 5; n++)
				{
					if (TwoDspaceSet[k].activeBaseMat[m][n] == 1)
					{
						nEqDelta[nTempPos][ii * 320 + jj + 64 * n] = 1;
					}
				}
				nEqDelta[nTempPos][1600] = TwoDspaceSet[k].activeBaseMat[m][5];
				nEqDelCnt = nTempPos;
				nEqDelCnt++;
				nTempPos++;
			}



		}
	}
	DWORD dwUsed = GetTickCount() - dwStart;
	
	int bRet = CheckConsistency(nEqDelta, nEqDelCnt, 1601, 0);
	

	printf("used time is %d ms", dwUsed);
	if (bRet == 1)
	{
		printf("Step1 Consistent.initEqCnt=%d, EqDelta: %d. NonZeroBox:%d.\n", initEqCnt, nEqDelCnt, nNonZero);
		// ����һ�£����ǻ�Ҫ��һ��������������ӣ�ÿ�μ�2�����������Ծ��box����Ϊ320����ô����640��������
		for (int m = 0; m < 320; m++)
		{
			if (nAlpha1_BoxList[m] != 0)
			{
				int k = nAlpha1_BoxList[m]; // ���Ǿ�ѡTwoDspaceSet[k]��additionalVector
				int ii = m / 64;
				int jj = m % 64;


				bool bConsistent = false;
				
				for (int a = 0; a < 8; a=a+2)
				{
					int nTempPos = nEqDelCnt;
					for (int b = 0; b < 5; b++)
					{
						if (TwoDspaceSet[k].activeAdditionalVec[a][b] == 1)
						nEqDelta[nTempPos][ii * 320 + jj + 64 * b] = 1;
					}
					nEqDelta[nTempPos][1600] = TwoDspaceSet[k].activeAdditionalVec[a][5];
					nTempPos++;

					for (int b = 0; b < 5; b++)
					{
						if (TwoDspaceSet[k].activeAdditionalVec[a+1][b] == 1)
							nEqDelta[nTempPos][ii * 320 + jj + 64 * b] = 1;
					}
					nEqDelta[nTempPos][1600] = TwoDspaceSet[k].activeAdditionalVec[a+1][5];
					nTempPos++;

					// ���мӽ�ȥ�ˡ��ж�һ��
					int bRes = CheckConsistency(nEqDelta, nTempPos, 1601, 0);
					if (bRes == 1)
					{
						nEqDelCnt = nTempPos;	// ��ʽ����
						bConsistent = true;
						break;
					}
					else
					{
						// ����ʱ��������У����øĻ�ȥ
						int nTempPos = nEqDelCnt;
						for (int b = 0; b < 5; b++)
						{
							if (TwoDspaceSet[k].activeAdditionalVec[a][b] == 1)
								nEqDelta[nTempPos][ii * 320 + jj + 64 * b] = 0;
						}
						nEqDelta[nTempPos][1600] = 0;
						nTempPos++;

						for (int b = 0; b < 5; b++)
						{
							if (TwoDspaceSet[k].activeAdditionalVec[a + 1][b] == 1)
								nEqDelta[nTempPos][ii * 320 + jj + 64 * b] = 0;
						}
						nEqDelta[nTempPos][1600] =0;
						nTempPos++;
					}

				}
				if (bConsistent == false)
				{
					if (maxCheckedBoxNum < m)
						maxCheckedBoxNum = m;

					printf("Step1.x failed at box %d/%d\n", m, maxCheckedBoxNum);
					return 0;
				}
				
			}

		}

		// MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt, 1601);
		printf("Step2: 320 boxfinished\n");
		return 1;
	}
	else
	{
		printf("Unonsistent.initEqCnt=%d, EqDelta: %d. NonZeroBox:%d.\n", initEqCnt, nEqDelCnt, nNonZero);
		return 0;
	}
*/
return 0;
	
}


/*
int C2017ECrypt::FindBeta0()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// Now, it is time to find suitable beta0.���ﲻ��ѡ������������ȥ�ҡ�
	// What we need to do is to determine nBeta0_BoxList, in the meanwhile, E_Delta is consistent.
	// 320 sboxes in nAlpha1_BoxList have been obtained.
	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i]!=0)
		{
			int k = nAlpha1_BoxList[i];
			// ����i�������Ӧ��5����λ��
			int ii = i / 64;
			int jj = i % 64;

			bool bFindTheInputDiff = false;
			// ��ʱ��Ҫ���������ҵ�һ�����Ķ�ά����ռ�ʹ�����ݡ�
			for (int j = 0; j < 32; j++)
			{
				if (ddt.DDT[j][k] == 4)
				{
					// ѡ������Ȼ�����E_Delta��һ����
					// ddt.GetMsgPairListFromDiff(j,k,msgPairList);
					// ����������������ķ��̣�5���������е�nEqDelta�Ƿ����ݡ���5�����̷ǳ��򵥡�
					int nTempPos = nEqDelCnt;
					for (int z = 0; z < 5; z++)
					{
						nEqDelta[nTempPos][ii * 320 + jj + 64 * z] = 1;
						nEqDelta[nTempPos][1600] = (j >> (4 - z)) & 1;
						nTempPos++;
					}
					// MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt + 5, 1601);
					// ���������
					int bRet = CheckConsistency(nEqDelta, nEqDelCnt, 1601, 5);

					if (bRet == 1)
					{
						
						nEqDelCnt = nTempPos;	// ����5��������ʽ����
						bFindTheInputDiff = true;
						break;
					}
					else {
						// ����5���¼ӵķ����������ȥ��
						nTempPos = nEqDelCnt;

						// MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt + 5, 1601);
						for (int z = 0; z < 5; z++)
						{
							nEqDelta[nTempPos][ii * 320 + jj + 64 * z] = 0;
							nEqDelta[nTempPos][1600] = 0;
							nTempPos++;
						}
					}

				}
				else if (ddt.DDT[j][k] == 8)
				{
					// ǰ���Ƕ����뼯�Ͻ����жϣ������8������ô����Ӧ6����ά����ռ䣬����Ҫ����ѡ����
				}
			}

			if (bFindTheInputDiff == false)
			{
				// û��Ϊ�������box�ҵ����ݵ������֣�Ѱ��beta0ʧ�ܡ�
				if (i == 0)
				{
					MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt, 1601);
					printf("i=0;%d", nAlpha1_BoxList[0]);
					
				}

				if (m_nMaxI < i) m_nMaxI = i;
				printf("nEqDeltaCnt = %d. sbox num is %d, m_nMaxI is %d.\n", nEqDelCnt,i, m_nMaxI);
				return 0;
			}
			
		}
	}

	printf("### Beta0 found.\n");

	
	return 1;
}
*/


int C2017ECrypt::CheckConsistency(byte **M, int nCurConsistentRow, int nCol, int rowsNeedCheck)
{
	// TODO: �ڴ˴����ʵ�ִ���.

	int *MainElemIndex = new int[nCurConsistentRow + rowsNeedCheck];
	int *nWeighList = new int[nCurConsistentRow + rowsNeedCheck];
	for (int i = 0; i < nCurConsistentRow + rowsNeedCheck;i++)
	{
		MainElemIndex[i] = 0;
		nWeighList[i] = 0;
	}
	

	MatrixOps::DiagonalFormat(M, nCurConsistentRow + rowsNeedCheck, nCol, MainElemIndex, nWeighList, nDependNum, nDependList);

	bool bConsistent = true;
	for (int i = 0; i < nCurConsistentRow + rowsNeedCheck; i++)
	{
		if (MainElemIndex[i] == nCol)
		{
//			printf("unconsistent i: %d", i);
			bConsistent = false;
			break;
		}
	}

	delete[] MainElemIndex;
	delete[] nWeighList;
	return bConsistent;
}


/* һ���Լ���ɰ�
	bool bAllConsistent = true;
	for (int i = nCurConsistentRow; i < nCurConsistentRow + rowsNeedCheck; i++)
	{
		int nConsistent = -1;		// -1:δ���� 0��ʧ�ܣ�1�ɹ�
		int kStart = 0;
		for (int j = 0; j < nCol; j++)
		{
			if (M[i][j] == 1)
			{
				if (j == nCol-1)
				{
					// �Ѿ������һ���ˣ���ʾ����ǰ��1600�����ض�û���ҵ����Լ���Ϊ��Ԫ��λ�á�
					// ���ԣ���������ǲ����ݵġ�
					nConsistent = 0;
					bAllConsistent = false;
					break;
				}

				// ��nMainElemDelta�������Ҷ�Ӧλ��Ԫ�ģ����û���ҵ�����ô��һ����һ�µġ�
				// ����ҵ��ˣ��Ǿ����Լ�����Ӧ�С�Ȼ����������ҵ��Լ�����Ԫ���ҵ�һ��λ�ã��Լ�����Ԫ���ͱ�ʾ���ݡ����û�ҵ��Լ�����Ԫ��λ�ã�ͬʱ�����л���1����ô�Ͳ����ݡ�
				for (int k = kStart; k < nCurConsistentRow; k++)
				{

					if (MainElemIndex[k]-1 == j)	// ��Ԫλ�ñ���Ǵ�1��ʼ�ġ���Ϊ��ʼֵ��0��Ϊ�����֡�
					{
						// ��Ӧ�������
						for (int z = j; z < nCol; z++)
						{
							M[i][z] = M[i][z] ^ M[k][z];

						}
						kStart = k;
						break;
					}
					else if (MainElemIndex[k] - 1 > j)
					{
						// �Լ�������Ԫ����Ȼ����
						// Ĭ��bConsitent����true,ֱ��������һ����
						nConsistent = 1;
						break;
					}
					// ����һ���������������Ԫ���������Ѽ���������У���ҵ���ԪҲû�����Լ���

					if (k == nCurConsistentRow - 1)
					{
						if (MainElemIndex[k] - 1 < j)
						{
							nConsistent = 1;
							break;
						}
					}


				}
				if (nConsistent == 0)
				{
					bAllConsistent = false;
					break;
				}
				else if(nConsistent ==1)
				{
					break;
				}
			}
		}
		if (bAllConsistent == false)
		{
			return 0;
		}
	}



*/

int C2017ECrypt::ResetVariables()
{
	// TODO: �ڴ˴����ʵ�ִ���.

	for (int i =nEDeltaAfterInit; i < nEqDelCnt; i++)
	{
		for (int j = 0; j < 1601; j++)
		{
			nEqDelta[i][j] = 0;
		//	nEqMsg[i][j] = 0;
		}

	}
	/*
	for (int i = 0; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			G[i][j] = 0;
			GL[i][j] = 0;
			
		}

		
		nBeta1[i] = 0;
		nAlpha1[i] = 0;
		nBeta0[i] = 0;
		nMainEleColPosInEqDelta[i] = 0;
		nMainEleColPosInEqMsg[i] = 0;
		m[i] = 0;

	}
	*/
	nEqDelCnt = nEDeltaAfterInit;
	// nEqMsgCnt = 0;

	/*
	for (int i = 0; i < 320; i++)
	{
		// record the differential of alpha2 and beta1 in SBox format.
		outDiffBox[i] = 0;
		inDiffBox[i] = 0;

		// record the differential of alpha1 and beta0 in SBox format.
		nAlpha1_BoxList[i] = 0;
		nBeta0_BoxList[i] =  0 ;
	}
	*/

	/*
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			msgPairList[i][j] = 0;
		}
	}
	*/

	return 0;
}


int C2017ECrypt::InitSubspaceSets()
{
	// TODO: �ڴ˴����ʵ�ִ���.


	return 0;
}


int C2017ECrypt::InitSpaceSets()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	for (int i = 1; i < 32; i++)
	{
		TwoDspaceSet[i].Init(i);
	}

	return 0;
}


int C2017ECrypt::FindBetaZero()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// �°汾

	// ��ʼ��chi_1_SBox[320]
	for (int i = 0; i < 320; i++)
	{
		// ����Ϊÿ��SBoxѰ��һ���ӿռ䣬���֮ǰ��findBeta0�汾��û�п���ÿ��box֮���Լ�������³ɹ��ʵ͡�
		// ����汾��Ҫ���Ǻ���Sbox��ǰ��SBox�������������Ҫ�ο����о����������ķֲ���
		// ��ֻ������Ϊ1��2�Ľ��п���
		if (nAlpha1_BoxList[i] != 0)
		{
			chi_1_SBox[i].nOutDiff = nAlpha1_BoxList[i];

			int k = nAlpha1_BoxList[i]; // ���Ǿ�ѡTwoDspaceSet[k]��activeBaseMat
			int ii = i / 64;
			int jj = i % 64;	// �ҳ�5������


		}
		else
		{
			chi_1_SBox[i].nOutDiff = 0;
		}
		


	}
	

	return 0;
}


int C2017ECrypt::InitChi1SBox()
{
	// TODO: �ڴ˴����ʵ�ִ���.

	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i] == 0)
		{
//			chi_1_SBox[i].nOutDiff = 0;
//			chi_1_SBox[i].nInputDiff = 0;
		}
		else
		{
//			chi_1_SBox[i].nOutDiff = nAlpha1_BoxList[i];
		}

//		chi_1_SBox[i].subSpaceSet.Init(chi_1_SBox[i].nOutDiff);

		
	}

	return 0;
}


int C2017ECrypt::LoadAlpha2FromText()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	// ���ļ�����alpha2�Ĳ��
	int nNum = 0;
	int diff[1600] = { 0 };
	FILE *fp = NULL;
	fp = fopen("StateDiff25.txt", "rb");
	// skip the first line.
	char buf[200] = { 0 };
	fgets(buf, 100, fp);
	for (int i = 0; i < 1600; i++)
	{
		fscanf(fp, "%d %d ", &nNum, &diff[i]);
	}
	fclose(fp);

	InitAlpha2(diff, 1600);

	return 0;
}
