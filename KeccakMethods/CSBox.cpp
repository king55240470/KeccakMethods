#include "CSBox.h"



CSBox::CSBox()
{
	nOutDiff = 0;
	for (int i = 0; i < 5; i++)
	{
		nInputDiffBits[i] = -1;
	}
}


CSBox::~CSBox()
{
}
