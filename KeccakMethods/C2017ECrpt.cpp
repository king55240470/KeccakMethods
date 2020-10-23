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

		nEqDelta[j] = new byte[1601];		// 最后一列保存方程非齐次方程的解
		nEqMsg[j] = new byte[1601];

		nDependList[j] = new int[1600];
	}

	for (int j = 0; j < 2400; j++)
	{
		nEqDelta[j] = new byte[1601];		// 最后一列保存方程非齐次方程的解
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
	// TODO: 在此处添加实现代码.

	for (int i = 0; i < nSize; i++)
		nAlpha2[i] = a2[i];

	printf("### Alpha2 initialized from input file.\n");
	return 1;
}


int C2017ECrypt::RandBeta1()
{
	// TODO: 在此处添加实现代码.

	// this is an randomized algorithm, so sow a seed first.
	srand(time(NULL));

	// Obtain Beta1 from Alpha2 by randomly choosing the input differential.
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 64; i++)
		{
			// 每个SBox由5个比特组成
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
			// 计算DDT中对应输入差分大于4的个数，然后在其中随机选择一个。
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
	// 有了inDiffBox之后，其本质就是beta1,现在我们需要将他转化为1600维的数组
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
	// 至此，我们获得了nBeta1;


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
	// TODO: 在此处添加实现代码.

	for (int i = 0; i < 320; i++)
	{
		
		if (inDiffBox[i] != 0)
		{

			int nColNum = i / 64;
			int nDepthNum = i % 64;

			// 根据输入差分和输出差分计算出子集，然后再从这个子集中寻找出一个二维仿射子集，最后再设计出约束条件
			// 如果对应的DDT为4，其4个输入值自动成为一个仿射子空间。
			int nCnt = 0;
			int nSet[32] = { 0 };		// input set
//			printf("****\n");
//			printf("nCol=%d,nDepth=%d\n", nColNum, nDepthNum);
			for (int z = 0; z < 32; z++)
			{
				// 依次测试每一个
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
			// 找到4个元素的集合了，需要生成3个条件
			for (int k = 0; k < 5; k++)
			{
				b[0][k] = (nSet[0] >> (4 - k)) & 1;
				b[1][k] = (nSet[1] >> (4 - k)) & 1;
				b[2][k] = (nSet[2] >> (4 - k)) & 1;
				b[3][k] = (nSet[3] >> (4 - k)) & 1;

				// 先找所有同列相同的，这直接确定一个方程
				if (b[0][k] == b[1][k] && b[0][k] == b[2][k] && b[0][k] == b[3][k])
				{
//					printf("%d col is %d\n", k, b[0][k]);
					bUsed[k] = true;			//代表这列考虑过了。
					nCond++;

					// 为G增加约束条件,第k列全部相同，成为方程，m应为b[0][k]；
					// 算出该box对应的5个元素在1600个元素中的位置
					// 此时是第i个box
					
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
	// TODO: 在此处添加实现代码.
	
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
	// TODO: 在此处添加实现代码.
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
	// TODO: 在此处添加实现代码.
	// 只需要求个逆就可以了
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

	// 将Alpha1的box格式顺便求好。
	FromVectorToSBox(nAlpha1, 1600, nAlpha1_BoxList);


//	printf("### Alpha1 obtained by multiplying L_Inv*Beta1.\n");

	return 0;
}


int C2017ECrypt::InitEqDeltaAndEqMsg()
{
	// TODO: 在此处添加实现代码.
	// 初始化EquationDelta, 该方程是关于差分的（到底是原始输入差分，还是经过一次L变换之后的差分呢？）
	// The last c+1 difference must be zero, so the last c+1 lines of L_Inv*beta0 must be zero.
	// c+1 = 513, 那么将

	nEqDelCnt = 513;
	for (int i = 1087; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			nEqDelta[i - 1087][j] = L_Inv[i][j];
		}
		nEqDelta[i-1087][1600] = 0;		// 后面的513比特差分均为0.
	}

	// 满足其第二个约束，这个就简单了，根据alpha1盒子中的数据，将所有的非活跃盒子对应的差分直接置为0.由于变量就是
	// 此处的差分，因此，方程直接令对应5个位置的差分值为0.
	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i] == 0)
		{
			// 将对应的5个位置的差分置为0，这就增加了5个方程，这5个方程中每行只有一个1.
			// 判断该box的位置
			int pos[5] = { 0 };
			FromBoxIdToBitPos(i, pos);

			// 开始加方程
			for (int j = 0; j < 5; j++)
			{
				nEqDelta[nEqDelCnt++][pos[j]] = 1;
			}
		}
	}


	// 初始化nEquationMsg
	// 最后c+1个确定的，这c+1个中的第一个为1，其余为0，这样就能得到c+1个方程。
	nEqMsgCnt = 513;
	
	// 第一个方程
	nEqMsg[0][1087] = 1;
	nEqMsg[0][1600] = 1;			// 增广项

	for (int i = 1088; i < 1600; i++)
	{
		for (int j = 0; j < 1600; j++)
		{
			nEqMsg[i - 1088][j] = L_Inv[i][j];
		}
		nEqDelta[i-1087][1600] = 0;		// 后面的513比特差分均为0.
	}

	// 将这两个方程转化为最简单形式，其目的是后面判断相容性的时候，方便。

	MatrixOps::DiagonalFormat(nEqDelta, nEqDelCnt, 1601, nMainEleColPosInEqDelta, nEqDeltaW, nDependNum, nDependList);

	nEDeltaAfterInit = nEqDelCnt;

	MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt, 1601);

//	printf("### E_Delta and E_M initialized.\n");
	return 0;
}


int C2017ECrypt::run()
{
	// TODO: 在此处添加实现代码.

//	Load_L_Matix();			// 获得L和L逆
//	InitGandM();			// 根据beta1和alpa2
//	MultiplyGwithL();		// 根据上面的G和基础数据L

	LoadAlpha2FromText();		// 获得Alpha2
	InitSpaceSets();

	// 根据基础数据开始算法
	
	RandBeta1();			// 根据Alpha2 随机获得Beta1
	
	GetAlpha1FromBeta1();	// 根据beta1和L_Inv
	InitEqDeltaAndEqMsg();	// 初始化时，需要用到Alpha1盒子中数据，如果是0差分，那么输入必然也是0，也就成了约束

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
	// TODO: 在此处添加实现代码.
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
	// TODO: 在此处添加实现代码.
	// Now, it is time to find suitable beta0.这里不是选择，这里是依次去找。
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
			int k = nAlpha1_BoxList[i]; // 我们就选TwoDspaceSet[k]的activeBaseMat
			int ii = i / 64;
			int jj = i % 64;

			int nTempPos = nEqDelCnt;
			for (int m = 0; m < 3; m++)
			{
				// 将3*6的activeBaseMat转化为 3*1600的；

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
		// 初步一致，我们还要进一步迭代往里面添加，每次加2个，如果不活跃的box数量为320，那么还有640个条件。
		for (int m = 0; m < 320; m++)
		{
			if (nAlpha1_BoxList[m] != 0)
			{
				int k = nAlpha1_BoxList[m]; // 我们就选TwoDspaceSet[k]的additionalVector
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

					// 两行加进去了。判断一下
					int bRes = CheckConsistency(nEqDelta, nTempPos, 1601, 0);
					if (bRes == 1)
					{
						nEqDelCnt = nTempPos;	// 正式加入
						bConsistent = true;
						break;
					}
					else
					{
						// 那临时加入的两行，还得改回去
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
	// TODO: 在此处添加实现代码.
	// Now, it is time to find suitable beta0.这里不是选择，这里是依次去找。
	// What we need to do is to determine nBeta0_BoxList, in the meanwhile, E_Delta is consistent.
	// 320 sboxes in nAlpha1_BoxList have been obtained.
	for (int i = 0; i < 320; i++)
	{
		if (nAlpha1_BoxList[i]!=0)
		{
			int k = nAlpha1_BoxList[i];
			// 根据i计算出对应的5比特位置
			int ii = i / 64;
			int jj = i % 64;

			bool bFindTheInputDiff = false;
			// 此时需要处理，依次找到一个它的二维仿射空间使其相容。
			for (int j = 0; j < 32; j++)
			{
				if (ddt.DDT[j][k] == 4)
				{
					// 选定他，然后测试E_Delta的一致性
					// ddt.GetMsgPairListFromDiff(j,k,msgPairList);
					// 测试这个差分所引入的方程（5个）与现有的nEqDelta是否相容。这5个方程非常简单。
					int nTempPos = nEqDelCnt;
					for (int z = 0; z < 5; z++)
					{
						nEqDelta[nTempPos][ii * 320 + jj + 64 * z] = 1;
						nEqDelta[nTempPos][1600] = (j >> (4 - z)) & 1;
						nTempPos++;
					}
					// MatrixOps::OutputMatToFile(nEqDelta, nEqDelCnt + 5, 1601);
					// 检测相容性
					int bRet = CheckConsistency(nEqDelta, nEqDelCnt, 1601, 5);

					if (bRet == 1)
					{
						
						nEqDelCnt = nTempPos;	// 将这5个方程正式加入
						bFindTheInputDiff = true;
						break;
					}
					else {
						// 将那5个新加的方程再清零回去。
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
					// 前提是对输入集合进行判断，如果是8个，那么他对应6个二维仿射空间，我们要依次选他。
				}
			}

			if (bFindTheInputDiff == false)
			{
				// 没有为这个输入box找到相容的输入差分，寻找beta0失败。
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
	// TODO: 在此处添加实现代码.

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


/* 一致性检验旧版
	bool bAllConsistent = true;
	for (int i = nCurConsistentRow; i < nCurConsistentRow + rowsNeedCheck; i++)
	{
		int nConsistent = -1;		// -1:未定， 0，失败，1成功
		int kStart = 0;
		for (int j = 0; j < nCol; j++)
		{
			if (M[i][j] == 1)
			{
				if (j == nCol-1)
				{
					// 已经到最后一列了，表示经过前面1600个比特都没有找到让自己成为主元的位置。
					// 所以，这个方程是不相容的。
					nConsistent = 0;
					bAllConsistent = false;
					break;
				}

				// 到nMainElemDelta数组中找对应位主元的，如果没有找到，那么就一定是一致的。
				// 如果找到了，那就用自己异或对应行。然后继续往后，找到自己的主元。找到一个位置，自己是主元，就表示相容。如果没找到自己做主元的位置，同时增广列还是1，那么就不相容。
				for (int k = kStart; k < nCurConsistentRow; k++)
				{

					if (MainElemIndex[k]-1 == j)	// 主元位置标号是从1开始的。因为初始值是0，为了区分。
					{
						// 对应两行异或
						for (int z = j; z < nCol; z++)
						{
							M[i][z] = M[i][z] ^ M[k][z];

						}
						kStart = k;
						break;
					}
					else if (MainElemIndex[k] - 1 > j)
					{
						// 自己就是主元，必然相容
						// 默认bConsitent就是true,直接跳过这一个。
						nConsistent = 1;
						break;
					}
					// 还有一种情况，他就是主元，而上面已加入的向量中，最靠右的主元也没有他自己大

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
	// TODO: 在此处添加实现代码.

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
	// TODO: 在此处添加实现代码.


	return 0;
}


int C2017ECrypt::InitSpaceSets()
{
	// TODO: 在此处添加实现代码.
	for (int i = 1; i < 32; i++)
	{
		TwoDspaceSet[i].Init(i);
	}

	return 0;
}


int C2017ECrypt::FindBetaZero()
{
	// TODO: 在此处添加实现代码.
	// 新版本

	// 初始化chi_1_SBox[320]
	for (int i = 0; i < 320; i++)
	{
		// 依次为每个SBox寻找一个子空间，相比之前的findBeta0版本，没有考虑每个box之间的约束，导致成功率低。
		// 这个版本需要考虑后面Sbox对前面SBox的依赖。这就需要参考已有矩阵中重量的分布。
		// 先只对重量为1和2的进行考虑
		if (nAlpha1_BoxList[i] != 0)
		{
			chi_1_SBox[i].nOutDiff = nAlpha1_BoxList[i];

			int k = nAlpha1_BoxList[i]; // 我们就选TwoDspaceSet[k]的activeBaseMat
			int ii = i / 64;
			int jj = i % 64;	// 找出5个坐标


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
	// TODO: 在此处添加实现代码.

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
	// TODO: 在此处添加实现代码.
	// 从文件读入alpha2的差分
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
