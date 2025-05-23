// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "VRActor_ver1.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;

UCLASS()
class MAGIC_PROJECT_API AVRActor_ver1 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRActor_ver1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Character用のStaticMesh : Sphere */
	UPROPERTY(VisibleAnywhere, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Player;

	UPROPERTY(VisibleAnywhere, Category = Collider, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	// コントローラーのマッピング
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlMove;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ControlMagic;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(VisibleAnywhere, Category = Control, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> Arrow;

public:
	UFUNCTION()
	void CreateMagic(float MagicSpeed = 10.f);

	//魔法陣からデータを受け取る
	UFUNCTION()
	void SetMagicData(UNiagaraSystem* Ef_Flying_ = nullptr, UNiagaraSystem* Ef_Destroy_ = nullptr);

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	
	//playerコントロール
	void ControlPlayer(const FInputActionValue& Value);

	//魔法コントロール
	void GoMagic(const FInputActionValue& Value);

	// カメラコントロール
	void Look(const FInputActionValue& Value);

private:

	// 移動倍率
	float MoveSpeedPoint = 10.0f;

	// 移動方向
	FRotator MoveRotator;

	// 魔法変更フラグ
	int MagicPalette;

	// 魔法のエフェクトのファイルを登録
	FString MagicEffectFilePath[9];

	void DebugLogLocation(AActor* a_ , FColor c);

	//----------------------------------------
	// csv用
	//----------------------------------------
	FString MagicFilePath;

	// csvファイル出力
	void WritePlayerInfoToCSV(AActor* m_);

	//魔法エフェクトのポインタ
	UNiagaraSystem* Ef_Flying;
	UNiagaraSystem* Ef_Destroy;

public:
	// 魔法実行フラグ
	bool CanMagic = true;//とりあえず
};
