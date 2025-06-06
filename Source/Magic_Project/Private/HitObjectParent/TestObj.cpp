// Fill out your copyright notice in the Description page of Project Settings.


#include "HitObjectParent/TestObj.h"

ATestObj::ATestObj() {

	objectMesh= LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Satou/Stylized_PBR_Nature/Foliage/Assets/SM_Common_Tree_11.SM_Common_Tree_11"));
	
	// StaticMesh‚ðStaticMeshComponent‚ÉÝ’è‚·‚é
	ObjectMesh->SetStaticMesh(objectMesh);
}

