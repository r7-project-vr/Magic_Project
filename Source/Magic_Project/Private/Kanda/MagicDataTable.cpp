// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/MagicDataTable.h"
#include "Math/RandomStream.h"

MagicDataTable::MagicDataTable(int m_, int l_):
	magicCnt(m_),
	magicfileLengh(l_)
{
	for (int i = 0; i < magicCnt; i++)
	{
		// ランダム
		int rand = FMath::RandRange(0, magicfileLengh);

		// ぷしゅバック
		magaicfileFlag.Add(rand);
	}
}

bool MagicDataTable::DecMagicCnt() {

	if (magicCnt - 1 < 0) { return true; }

	this->magicCnt--;
	return false;
}

int MagicDataTable::GetMagicCnt() {
	return magicCnt;
}

TArray<int> MagicDataTable::MagicFileFlag() {

	return magaicfileFlag;
}
