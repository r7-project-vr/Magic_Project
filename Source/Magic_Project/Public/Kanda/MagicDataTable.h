// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 魔法を飛ばすデータクラス
 */
class MAGIC_PROJECT_API MagicDataTable
{
public:
	MagicDataTable(int m_,int l_);
private:
	int magicCnt;
	int magicfileLengh;
	TArray<int> magaicfileFlag;

public:

	// カウンターを減らす
	bool DecMagicCnt();

	// カウンターを取得
	int GetMagicCnt();

	// 配列を送る
	TArray<int> MagicFileFlag();
};
