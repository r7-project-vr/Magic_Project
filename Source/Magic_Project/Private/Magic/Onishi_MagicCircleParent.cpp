// Fill out your copyright notice in the Description page of Project Settings.


#include "Magic/Onishi_MagicCircleParent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kanda/MagicDataTable.h"
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

	//BOOL変数初期化
	bIsPlayerOverlapping = false;
}

// Called when the game starts or when spawned
void AOnishi_MagicCircleParent::BeginPlay()
{
	Super::BeginPlay();

	if (Ef_MagicCircle != nullptr) {
		if (Ef_MagicCircle != nullptr) {

			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				Ef_MagicCircle,
				SphereComponent,
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true
			);
		}
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
	if (AVRActor_ver1* Pawn = Cast<AVRActor_ver1>(OtherActor)) {
		bIsPlayerOverlapping = true;
		// スマートポインタでデータを確保
		TSharedPtr<MagicDataTable> m = MakeShared<MagicDataTable>(magicCnt, Ef_MagicFly, Ef_Destroy);
		Pawn->SetMagicData(m, this);
	}
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AVRActor_ver1* Pwan = Cast<AVRActor_ver1>(OtherActor))
	{
		Pwan->bIsPlayerOverlapping = true;
	}
}

//オーバーラップ終了時の動作を定義
void AOnishi_MagicCircleParent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {
	if (AVRActor_ver1* Pawn = Cast<AVRActor_ver1>(OtherActor)) {
		bIsPlayerOverlapping = false;
		Pawn->SetMagicData(nullptr, nullptr);
	}
	Super::NotifyActorEndOverlap(OtherActor);

	if (AVRActor_ver1* Pawn = Cast<AVRActor_ver1>(OtherActor))
	{
		Pawn->bIsPlayerOverlapping = false;
	}
}