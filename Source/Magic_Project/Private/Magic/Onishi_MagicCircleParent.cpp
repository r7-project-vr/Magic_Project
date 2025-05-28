// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicCircleParent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kanda/VRActor_ver1.h"

// Sets default values
AOnishi_MagicCircleParent::AOnishi_MagicCircleParent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//スフィアコリジョン作成
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
	SphereComponent->SetSphereRadius(200.0f);

	//オーバーラップ時の動作をバインド
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AOnishi_MagicCircleParent::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AOnishi_MagicCircleParent::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AOnishi_MagicCircleParent::BeginPlay()
{
	Super::BeginPlay();

	if (Ef_MagicCircle != nullptr) {
		FVector loc = this->GetActorLocation();
		FRotator rot = this->GetActorRotation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Ef_MagicCircle, loc, rot);
	}
}

// Called every frame
void AOnishi_MagicCircleParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

//オーバーラップ時の動作を定義
void AOnishi_MagicCircleParent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("HIT"));
	if (AVRActor_ver1* Pawn = Cast<AVRActor_ver1>(OtherActor)) {
		
		Pawn->SetMagicData(Ef_MagicFly, Ef_Destroy);
	}
}

//オーバーラップ終了時の動作を定義
void AOnishi_MagicCircleParent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("END"));
	if (AVRActor_ver1* Pawn = Cast<AVRActor_ver1>(OtherActor)) {

		Pawn->SetMagicData(nullptr, nullptr);
	}
}