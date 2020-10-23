#pragma once
#include "CKeccakDDT.h"
#include "header.h"
#include "CSub2DSpaceSet.h"
#include "CSBox.h"
#include "header.h"
/*
 * 
 */

class C2017ECrypt
{

public:

	int b = 0;
	byte *nAlpha2;		// equals to Delta(Si)
	byte *nBeta1;
	byte *nAlpha1;
	byte *nBeta0;

	byte **nEqDelta;
	int nEqDeltaW[MAX_ROW];
	int nMainEleColPosInEqDelta[1600] = { 0 };
	int nMainEleColPosInEqMsg[1600] = { 0 };
	byte **nEqMsg;
	int nEqDelCnt = 0;
	int nEqMsgCnt = 0;

	// record the differential of alpha2 and beta1 in SBox format.
	int outDiffBox[320] = { 0 };
	int inDiffBox[320] = { 0 };

	// record the differential of alpha1 and beta0 in SBox format.
	int nAlpha1_BoxList[320] = { 0 };
	int nBeta0_BoxList[320] = { 0 };

	CSBox chi_1_SBox[320];

	int **msgPairList;

	int nDependNum[1600] = { 0 };
	int **nDependList;

	// The following two items are used to specify the linear equations to constrain the input bits of the second sbox layer.
	byte **G;
	int nGcnt = 0;
	byte m[1600] = { 0 };

	byte **L;	// L stands for the linear 
	byte **L_Inv;
	byte **GL;	// used to store the result of G*L

	CKeccakDDT ddt;

	// 定义32个集合。但是第1个我们不用，为的是初始化时，子集编号与输出差分能一致。
	CSub2DSpaceSet TwoDspaceSet[32];

	C2017ECrypt(int);
	C2017ECrypt();
	~C2017ECrypt();

	int InitAlpha2(int a2[], int);
	
	int RandBeta1();
	int InitGandM();
	int Load_L_Matix();
	int MultiplyGwithL();
	int GetAlpha1FromBeta1();
	int InitEqDeltaAndEqMsg();
	int run();
	int FromVectorToSBox(byte *vec, int nVecLen, int *BoxList);
	int FromBoxIdToBitPos(int id, int *);
	int FindBeta0();
	int CheckConsistency(byte **,int, int, int);
	int ResetVariables();
	int m_nMaxI;
	int InitSubspaceSets();
	int InitSpaceSets();
	int maxCheckedBoxNum;
	int nEDeltaAfterInit;
	int FindBetaZero();
	int InitChi1SBox();
	int LoadAlpha2FromText();
};

