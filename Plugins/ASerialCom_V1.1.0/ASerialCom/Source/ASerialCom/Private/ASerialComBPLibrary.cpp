// Copyright Epic Games, Inc. All Rights Reserved.

#include "ASerialComBPLibrary.h"
#include "ASerialCom.h"

UASerialComBPLibrary::UASerialComBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UASerialLibControllerWin* UASerialComBPLibrary::CreateASerialLibController()
{
	return NewObject<UASerialLibControllerWin>();;
}

