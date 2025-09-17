// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MAGIC_PROJECT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// ìÔà’ìxÅBåªç›ÇÕ0Ç≈NormalÅA1Ç≈Hard
	UPROPERTY(BlueprintReadWrite)
	int Difficulty;
};
