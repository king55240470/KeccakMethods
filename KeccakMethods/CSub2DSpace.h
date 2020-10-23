#pragma once
#include "header.h"
class CSub2DSpace
{
public:
	// ����3��Լ���������������2D�ӿռ�
	byte baseMatrix[3][6] = { 0 };

	// ���ڱ��������Ԫ��λ�ã����ֵ�ڽ�һ��������������ʱҪ�õ���
	byte baseMatMainElem[5] = { 0 };

	// ���Ǳ�ʾ����ӿռ䣬��4��Ԫ�ر�
	int baseVal[4] = { 0 };

	// �����Լ������,ÿ��Ԫ�����������������ʹ������Լ������4��Ԫ�ع���8��������
	int additionalVectors[8][6] = { 0 };


public:
	CSub2DSpace();
	~CSub2DSpace();
	int Init(int *x);
	int GenBaseMatrix();
	int GenAdditionalVec();
};

