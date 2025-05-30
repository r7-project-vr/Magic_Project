// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

/**
 * 魔法を飛ばすデータクラス
 */
class MAGIC_PROJECT_API MagicDataTable
{
public:

	// コンストラクタ
	MagicDataTable(
		int m_,						// カウンターの初期化
		TArray<UNiagaraSystem*> f_, // 飛ぶときのエフェクトの配列
		TArray<UNiagaraSystem*> d_  // 破壊したときのエフェクトの配列
	);

	// デスストラクタ
	~MagicDataTable();

private:

	int							magicCnt;			// カウンター
	int							cntMax;				// カウンターの最大値
	TArray<int>					flyRand;			// 乱数を保存する配列
	TArray<int>					deathRand;			// 乱数を保存する配列
	TArray<UNiagaraSystem*>		flyNs;				// 飛ぶときのエフェクトのポインタ
	TArray<UNiagaraSystem*>		deathNs;			// 破壊したときのエフェクトのポインタ

public:

	// カウンターを減らす
	bool DecMagicCnt();

	// カウンターを取得
	int GetMagicCnt();

	// 配列を送る
	UNiagaraSystem* GetFlyNiagaraSystem(const int cnt);

	// 配列を送る
	UNiagaraSystem* GetDeathNiagaraSystem(const int cnt);
};
