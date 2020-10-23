#pragma once
#include "header.h"
class CSub2DSpace
{
public:
	// 保存3个约束条件，代表这个2D子空间
	byte baseMatrix[3][6] = { 0 };

	// 用于保存各行主元的位置，这个值在进一步补充两个向量时要用到。
	byte baseMatMainElem[5] = { 0 };

	// 还是表示这个子空间，用4个元素表达；
	int baseVal[4] = { 0 };

	// 额外的约束条件,每个元素配上两个向量，就代表了自己。因此4个元素共有8个向量；
	int additionalVectors[8][6] = { 0 };


public:
	CSub2DSpace();
	~CSub2DSpace();
	int Init(int *x);
	int GenBaseMatrix();
	int GenAdditionalVec();
};

