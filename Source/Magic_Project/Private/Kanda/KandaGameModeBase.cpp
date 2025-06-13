// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/KandaGameModeBase.h"
#include "KandaPawn.h"
#include "Kanda/VRActor_ver1.h"
#include "Kanda/VRActor_ver2.h"

AKandaGameModeBase::AKandaGameModeBase()
{
	//DefaultPawnClass = AKandaPawn::StaticClass();
	DefaultPawnClass = AVRActor_ver1::StaticClass();
}