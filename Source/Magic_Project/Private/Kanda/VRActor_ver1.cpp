// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/VRActor_ver1.h"
#include "InputMappingContext.h"
#include "Magic/Onishi_MagicLauncher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Components/ArrowComponent.h" 
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
AVRActor_ver1::AVRActor_ver1()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Cameraを追加する
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	// Camera->SetupAttachment(RootComponent);

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
	Arrow->SetupAttachment(RootComponent);

	// Sphereの頭上に移動するようにLocationを設定する
	Arrow->SetRelativeLocation(FVector(400.0f, 0.0f, 130.0f));

	// Arrowを表示されるようにする
	Arrow->bHiddenInGame = false;

	// scの定義
	FString magic = "/Script/Magic_Project.Onishi_MagicLauncher";
	static ConstructorHelpers::FObjectFinder< UClass > found(*magic); // 上記で設定したパスのオブジェクトを取得する
	sc = found.Object; // 上記で発見したオブジェクトのクラスを取得する

	// Effectのファイルの場所
	MagicEffectFilePath = "/Game/KTP_Effect/Particles/Fly/Explosion_01_01.Explosion_01_01";


	// テスト用
	{
		// 現在時刻の取得
		FDateTime Now = FDateTime::Now();
		FString FormattedTime = Now.ToString(TEXT("%Y_%m_%d__%H_%M"));

		MagicFilePath =
			FPaths::ProjectDir() / TEXT("CSVFile/Export/MagicData_" + FormattedTime + ".csv");
	}
}

// Called when the game starts or when spawned
void AVRActor_ver1::BeginPlay()
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
void AVRActor_ver1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AVRActor_ver1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(ControlMove, ETriggerEvent::Triggered, this, &AVRActor_ver1::ControlPlayer);

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(ControlMagic, ETriggerEvent::Triggered, this, &AVRActor_ver1::GoMagic);

		// LookとIA_LookのTriggeredをBindする
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVRActor_ver1::Look);
	}

}

void AVRActor_ver1::ControlPlayer(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	const FVector2D V = Value.Get<FVector2D>();

	FVector PreLocation = GetActorLocation();
	FVector NewLocation = PreLocation + Arrow->GetComponentToWorld().TransformVectorNoScale(FVector(V.Y, V.X, 0.0f) * MoveSpeedPoint);

	SetActorLocation(NewLocation);
}

//魔法を撃つ
void AVRActor_ver1::GoMagic(const FInputActionValue& Value)
{
	if (const bool v = Value.Get<bool>() && CanMagic)
	{
		//
		// CanMagic = false;
		WritePlayerInfoToCSV(this);
#if true
		// 魔法アクターを取得
		{
			if (sc != nullptr)
			{
				AOnishi_MagicLauncher* a = GetWorld()->SpawnActor<AOnishi_MagicLauncher>(sc); // スポーン処理

				FVector look = GetControlRotation().Vector();
				FVector pos = GetActorLocation();
				//FString path = "/Game/KTP_Effect/Particles/Fly/Explosion_01_01.Explosion_01_01";

				UKismetSystemLibrary::PrintString(
					this,
					pos.ToString(),
					true,
					true,
					FColor::Red,
					5.0f
					);

				a->SetActorLocation(pos);

				UKismetSystemLibrary::PrintString(
					this,
					a->GetActorLocation().ToString(),
					true,
					true,
					FColor::Red,
					5.0f
				);
				//a->LaunchMagic(look, pos, MagicEffectFilePath);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("sc is null"));
			}
		}
#endif
	}
}

// カメラコントローラー
void AVRActor_ver1::Look(const FInputActionValue& Value)
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

		// カメラをまわす
		SetActorRotation(controlRotate);

		// 移動方向を指定する
		FRotator ArrowRotate = FRotator(0,controlRotate.Yaw, 0);
		Arrow->SetWorldRotation(ArrowRotate);
	}
}

// csv出力
void  AVRActor_ver1::WritePlayerInfoToCSV(AActor* m_)
{
	FString MagicName = MagicEffectFilePath;

	// CSVに書き込む内容
	FString CSVContent = MagicName + TEXT(",") + TEXT("\n");

	// ファイルの存在を確認し、存在しない場合はヘッダー行を追加
	if (!FPaths::FileExists(MagicFilePath))
	{
		CSVContent = TEXT("MagicName\n") + CSVContent;
	}

	// ファイルに内容を書き込む
	FFileHelper::SaveStringToFile(CSVContent, *MagicFilePath, FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}