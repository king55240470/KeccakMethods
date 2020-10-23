#pragma once
class CKeccakDDT
{
public:
	int DDT[32][32] = { 0 };

	CKeccakDDT();
	~CKeccakDDT();
    int SBox(int);
	int DisplayDDT();
	int GetMsgPairListFromDiff(int nInDiff, int nOutDiff, int **);

};

