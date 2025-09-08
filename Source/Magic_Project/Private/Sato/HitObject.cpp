// Fill out your copyright notice in the Description page of Project Settings.


#include "Sato/HitObject.h"
#include "Magic/Onishi_MagicLauncher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHitObject::AHitObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHitObject::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AHitObject::OnOverlapBegin);

}

// Called every frame
void AHitObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 魔法とオーバーラップしたら呼ばれる処理
void AHitObject::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AOnishi_MagicLauncher* a = Cast<AOnishi_MagicLauncher>(OtherActor)) {
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
		AVRActor_ver1* VRActorV1 = Cast<AVRActor_ver1>(Pawn);
		VRActorV1->Magic_Score += 100;
		UGameplayStatics::PlaySound2D(this, MagicHitSound);
		Destroy();
	}
}