/*
	this is a class used for matrix computation over GF(2)
*/
#pragma once
#include "header.h"
class MatrixOps
{
public:
	MatrixOps();
	~MatrixOps();

	static unsigned int Rank(byte **G, int nRow, int nCol);
	static int MatMul(byte **A, int nRowA, int nColA, byte **B, int nRowB, int nColB, byte **C);
	static int DisplayMat(byte **M, int nRow, int nCol);
	static int DiagonalFormat(byte **, int nRow, int nCol, int *MainElem, int *nRowWeight, int *nDependNum, int **nDependList);
	static int OutputMatToFile(byte **M, int nRow, int nCol);
	};

