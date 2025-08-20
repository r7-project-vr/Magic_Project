// Fill out your copyright notice in the Description page of Project Settings.


#include "Kanda/VRActor_ver1.h"
#include "InputMappingContext.h"
#include "Magic/Onishi_MagicLauncher.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Components/ArrowComponent.h" 
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Engine/Engine.h"
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplay.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AVRActor_ver1::AVRActor_ver1():
	magicData(nullptr),
	circle(nullptr)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	Player = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Player;

	// SphereComponentを追加し、BoxComponentをRootComponentにAttachする
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(Player);

	// Sphereのサイズを設定する
	Sphere->SetSphereRadius(30.f);

	// Sphereの位置を調整する
	Sphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f), false);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AVRActor_ver1::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AVRActor_ver1::OnSphereEndOverlap);

	// Cameraを追加する
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(Sphere);

	// Input Mapping Context「IMC_TestPad」を読み込む
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Kanda/Input/IMC_TestPad"));

	// Input Action「IA_InputMove」を読み込む
	ControlMove = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_InputMove"));

	// Input Action 「IA_MagicCharge」を読み込む
	MagicCharge = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_ChargeMagic"));

	// Input Action「IA_GoMagic」を読み込む
	ControlMagic = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_GoMagic"));

	// Input Action「IA_Look」を読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_Look"));

	// Input Action「IA_MoveAction」を読み込む
	MoveStart = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_MoveAction"));

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
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		bool VRAllowed = GEngine->XRSystem->IsHeadTrackingAllowed();
		if (VRAllowed) {
			GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Local);
		}
	}

	// スプラインをレベル上から取得（一個だけ） また、プレイヤーの初期位置をセット
	SplineActor = Cast<APlayerWayRoad>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerWayRoad::StaticClass()));
	if (SplineActor) // ただのnullチェック
	{
		FTransform transformTemp;
		transformTemp = SplineActor->GetSplineTransform(distance, 0.0f);
		FVector newLocation = FVector(transformTemp.GetLocation());
		SetActorLocation(newLocation);
	}
}

// Called every frame
void AVRActor_ver1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//VRInformation();

	ArriveSplinePoint(StopPointNum);
	// スプラインの上を移動していく処理
	if (SplineActor && !isStop) // ただのnullチェック&今停止中かチェック
	{
		FTransform transformTemp;
		transformTemp = SplineActor->GetSplineTransform(distance, MoveSpeedPoint * DeltaTime);
		FVector newLocation = FVector(transformTemp.GetLocation());
		SetActorLocation(newLocation);
	}
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
		EnhancedInputComponent->BindAction(MagicCharge, ETriggerEvent::Triggered, this, &AVRActor_ver1::ChargeMagic);
		EnhancedInputComponent->BindAction(ControlMagic, ETriggerEvent::Completed, this, &AVRActor_ver1::GoMagic);

		// LookとIA_LookのTriggeredをBindする
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVRActor_ver1::Look);

		// MoveStartをバインドする
		EnhancedInputComponent->BindAction(MoveStart, ETriggerEvent::Triggered, this, &AVRActor_ver1::PlayerMoveStart);
	}
}

void AVRActor_ver1::ControlPlayer(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	const FVector2D V = Value.Get<FVector2D>();

	FVector PreLocation = GetActorLocation();

	FVector NewLocation = PreLocation + Sphere->GetComponentToWorld().TransformVectorNoScale(FVector(V.Y, V.X, 0.0f) * MoveSpeedPoint);
	
	SetActorLocation(NewLocation);
}

void AVRActor_ver1::ChargeMagic(const FInputActionValue& Value)
{
	if (const bool v = Value.Get<bool>()) {
		MagicChargeTime += GetWorld()->GetDeltaSeconds();
		if (MagicChargeTime >= 2.0f)
		{
			UKismetSystemLibrary::PrintString(
				this,
				TEXT("2byoutattayo"),
				true,
				true,
				FColor::Cyan,
				2.0f
			);
		}
	}
}

// 魔法を撃つ_コントローラーのみ
void AVRActor_ver1::GoMagic(const FInputActionValue& Value)
{
	if (true) {
		UKismetSystemLibrary::PrintString(
			this,
			TEXT("uttayo"),
			true,
			true,
			FColor::Red,
			2.0f
		);
		if (magicData == nullptr) { return; }
		UKismetSystemLibrary::PrintString(
			this,
			TEXT("HELLO"),
			true,
			true,
			FColor(1,1,1,1),
			2.0f
		);
		const int cnt = magicData->GetMagicCnt();

		UNiagaraSystem* f = magicData->GetFlyNiagaraSystem(0);
		UNiagaraSystem* d = magicData->GetDeathNiagaraSystem(0);
		UNiagaraSystem* ff = magicData->GetFlyNiagaraSystem(1);

		// 魔法のチャージ時間を計って何の魔法を出すか決める
		// else外したら多分破棄されたポインタ「d」を使うことになるのでクラッシュする
		if (MagicChargeTime <= 2.0f)
		{
			CreateMagic(f, d);
		}
		else
		{
			CreateMagic(ff, d);
		}

		//チャージ時間の初期化
		MagicChargeTime = 0;


		if (magicData->DecMagicCnt()) {
			UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "magicCnt 0");
			magicData = nullptr;

			// 魔法陣を破壊
			circle->Ef_MagicCircle = nullptr;
			circle->Destroy();
		}
	}
}
void AVRActor_ver1::SetMagicData(TSharedPtr<MagicDataTable> m_, AOnishi_MagicCircleParent* o_) {

	magicData = m_;
	circle = o_;
}

// 魔法を飛ばす処理
void AVRActor_ver1::CreateMagic(UNiagaraSystem* Ef_Flying_, UNiagaraSystem* Ef_Destroy_, float s_) {

	// エフェクトがnullなら処理なし
	if (Ef_Flying_ == nullptr || Ef_Destroy_ == nullptr) return;

	// 魔法アクターを生成
	{
		FRotator look = Sphere->GetComponentRotation();
		FVector pos = GetActorLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AOnishi_MagicLauncher* magic =
			GetWorld()->SpawnActor<AOnishi_MagicLauncher>(AOnishi_MagicLauncher::StaticClass(), pos, look); // スポーン処理

		magic->MoveSpeed *= s_;
		magic->LaunchMagic(look.Vector(), pos, Ef_Flying_, Ef_Destroy_);

		DebugLogLocation(magic, FColor::Red);
		WritePlayerInfoToCSV(this);
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

	}
}

// 接触判定の処理、コライダー同士が接触したときに呼び出される
void AVRActor_ver1::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// pass
}

// 接触判定の処理、コライダー同士が離れたときに呼び出される
void AVRActor_ver1::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){

	if (AOnishi_MagicCircleParent* Pawn = Cast<AOnishi_MagicCircleParent>(OtherActor)) {

		magicData = nullptr;
		circle = nullptr;
	}
}

// デバッグ用
void AVRActor_ver1::DebugLogLocation(AActor* a_, FColor c)
{
	if (a_ == nullptr) { return; }

	UKismetSystemLibrary::PrintString(
		this,
		a_->GetActorLocation().ToString(),
		true,
		true,
		c,
		2.0f
	);
}

// csv出力
void  AVRActor_ver1::WritePlayerInfoToCSV(AActor* m_)
{
	//FString MagicName = ;

	//// CSVに書き込む内容
	//FString CSVContent = MagicName + TEXT(",") + TEXT("\n");

	//// ファイルの存在を確認し、存在しない場合はヘッダー行を追加
	//if (!FPaths::FileExists(MagicFilePath))
	//{
	//	CSVContent = TEXT("MagicName\n") + CSVContent;
	//}

	//// ファイルに内容を書き込む
	//FFileHelper::SaveStringToFile(CSVContent, *MagicFilePath, FFileHelper::EEncodingOptions::AutoDetect,
	//	&IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}

//---------------------------------------------------------------------------------------------------------------------------------
//sato
// VR機器の情報取得
void AVRActor_ver1::VRInformation()
{
	GEngine->XRSystem->HasValidTrackingPosition();
	if (GEngine->XRSystem->IsHeadTrackingAllowed())
	{
		FQuat OrientationAsQuat;
		FVector Position(0.f);

		GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, OrientationAsQuat, Position);
		this->Sphere->SetRelativeLocationAndRotation(Position, OrientationAsQuat);
	}
}

// IA_MoveActionに登録されたボタンを押されると行う処理（現在はJキー）
void AVRActor_ver1::PlayerMoveStart(const FInputActionValue& Value)
{
	//ストップしている時のみ処理
	if (isStop)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("ugoke"), true, true, FColor::Blue, 2.0f, NAME_None);
		isStop = false;
		StopPointNum++;
	}
}

// 指定したスプラインの点に到達すると行われる処理
void AVRActor_ver1::ArriveSplinePoint(int point_)
{
	// ニアリーイコールを使うための変数たち
	FVector SplinePoint = SplineActor->Spline->GetLocationAtSplinePoint(point_, ESplineCoordinateSpace::World);
	FTransform ATransform = FTransform(FRotator(0, 0, 0), SplinePoint, FVector(1, 1, 1));
	FVector nowLocation = GetActorLocation();
	FTransform BTransform = FTransform(FRotator(0, 0, 0), nowLocation, FVector(1, 1, 1));

	// ニアリーイコールを使ってキャラクターを止めるか動かすか判断
	bool isNearPointCharacter = UKismetMathLibrary::NearlyEqual_TransformTransform(ATransform, BTransform, 5.0f, 0.0001f, 0.0001f);
	if (isNearPointCharacter)
	{
		isStop = true;
	}
}