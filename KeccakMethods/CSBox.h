#pragma once
#include "header.h"
#include "CSub2DSpaceSet.h"
class CSBox
{
public:
	int nOutDiff;				// 32λ��ʽ
	int nInputDiffBits[5];		// ������ʽ
	int activeBaseMat[3][6] = { 0 };
	int activeAdditionalVec[8][6] = { 0 };	
	byte nInputSpace[4] = { 0 };
	CSub2DSpaceSet subSpaceSet;
	

public:
	CSBox();
	~CSBox();
	int nInputDiff;
};

