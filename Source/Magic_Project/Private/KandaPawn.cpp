// Fill out your copyright notice in the Description page of Project Settings.


#include "KandaPawn.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Components/ArrowComponent.h" 
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AKandaPawn::AKandaPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Sphere;

	// StaticMeshをLaodしてStaticMeshComponentのStaticMeshに設定する
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere"));

	// StaticMeshをStaticMeshComponentに設定する
	Sphere->SetStaticMesh(Mesh);

	// MaterialをStaticMeshに設定する
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"));

	// MaterialをStaticMeshComponentに設定する
	Sphere->SetMaterial(0, Material);

	// Simulate Physicsを有効にする
	Sphere->SetSimulatePhysics(true);

	// SpringArmを追加する
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	// 角度を変更する FRotator(Pitch(Y), Yaw(Z), Roll(X))
	//SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));

	// Spring Armの長さを調整する
	SpringArm->TargetArmLength = 450.0f;

	// pawnの角度を利用する
	SpringArm->bUsePawnControlRotation = true;

	// SpringArmからの角度を継承しない
	// SpringArm->bInheritPitch = false;
	// SpringArm->bInheritYaw = false;
	// SpringArm->bInheritRoll = false;

	// CameraのLagを有効にする
	SpringArm->bEnableCameraLag = true;

	// Cameraを追加する
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

	// Input Mapping Context「IMC_TestPad」を読み込む
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Kanda/Input/IMC_TestPad"));

	// Input Action「IA_InputMove」を読み込む
	ControlMove = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_InputMove"));

	// Input Action「IA_GoMagic」を読み込む
	ControlMagic = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_GoMagic"));

	// Input Action「IA_Look」を読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_Look"));

	// Arrowを追加する
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	Arrow->SetupAttachment(Camera);

	// Sphereの頭上に移動するようにLocationを設定する
	Arrow->SetRelativeLocation(FVector(400.0f, 0.0f, 130.0f));

	// Arrowを表示されるようにする
	Arrow->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void AKandaPawn::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AKandaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKandaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(ControlMove, ETriggerEvent::Triggered, this, &AKandaPawn::ControlPlayer);

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(ControlMagic, ETriggerEvent::Triggered, this, &AKandaPawn::GoMagic);

		// LookとIA_LookのTriggeredをBindする
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKandaPawn::Look);
	}
}

//コントローラー
void AKandaPawn::ControlPlayer(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	const FVector2D V = Value.Get<FVector2D>();

	//座標移動
	FVector PreLocation = GetActorLocation();
	FVector NewLocation = PreLocation + Arrow->GetComponentToWorld().TransformVectorNoScale(FVector(V.Y, V.X, 0.0f) * AddMovePoint);
	SetActorLocation(NewLocation);
}

//魔法を撃つ
void AKandaPawn::GoMagic(const FInputActionValue& Value)
{
	if (const bool v = Value.Get<bool>() && CanMagic) 
	{
		UE_LOG(LogTemp, Log, TEXT("魔法を撃ったYO！"));
	}
}

// カメラコントローラー
void AKandaPawn::Look(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	FVector2D v = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(v.X);
		AddControllerPitchInput(v.Y);

		// Pawnが持っているControlの角度を取得する
		FRotator controlRotate = GetControlRotation();

		// controllerのPitchの角度を制限する
		double LimitPitchAngle = FMath::ClampAngle(controlRotate.Pitch, -40.0f, -10.0f);

		// PlayerControllerの角度を設定する
		UGameplayStatics::GetPlayerController(this, 0)->SetControlRotation(FRotator(controlRotate.Pitch, controlRotate.Yaw, 0.0f));
	}
}


