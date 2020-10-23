#pragma once
#include "CKeccakDDT.h"
#include "CSub2DSpace.h"
class CSub2DSpaceSet
{
public:
	int nOutDiff;
	CKeccakDDT ddt;
	CSub2DSpace *pSpace;
	int nSubspaceList[20][4] = { 0 };

	int activeBaseMat[3][6] = { 0 };
	int activeAdditionalVec[8][6] = { 0 };


public:
	CSub2DSpaceSet();
	~CSub2DSpaceSet();
	int Check2DSubSpace(int *nVal);
	int nSubspaceNum;
	int Init(int nOutDiff);
	int RandomlyChooseSubSpace();
};

