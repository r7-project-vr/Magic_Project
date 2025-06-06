// Fill out your copyright notice in the Description page of Project Settings.


#include "HitObjectParent/HitObject_0.h"

AHitObject_0::AHitObject_0() {

	objectMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Satou/Stylized_PBR_Nature/Rocks/Assets/SM_R_Rock_05.SM_R_Rock_05"));
	
	// StaticMesh‚ðStaticMeshComponent‚ÉÝ’è‚·‚é
	ObjectMesh->SetStaticMesh(objectMesh);
}

