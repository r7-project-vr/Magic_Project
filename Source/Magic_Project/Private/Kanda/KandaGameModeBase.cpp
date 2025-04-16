// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/KandaGameModeBase.h"
#include "KandaPawn.h"

AKandaGameModeBase::AKandaGameModeBase()
{
	DefaultPawnClass = AKandaPawn::StaticClass();
}