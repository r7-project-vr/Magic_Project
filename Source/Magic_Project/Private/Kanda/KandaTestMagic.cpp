// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/KandaTestMagic.h"

KandaTestMagic::KandaTestMagic()
{
	UE_LOG(LogTemp, Log, TEXT("new Magic"));
}

KandaTestMagic::~KandaTestMagic()
{
	UE_LOG(LogTemp, Log, TEXT("delete Magic"));
}

FString KandaTestMagic::MagicName()
{
	return TEXT("NoName");
}

int KandaTestMagic::MagicScore()
{
	// No Score
	return 0;
}
