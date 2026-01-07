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
#include <array>
#include "HeadMountedDisplayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "sato/MagicGameInstance.h"


// Sets default values
AVRActor_ver1::AVRActor_ver1() :
	magicData(nullptr),
	circle(nullptr),
	IsInMagicZone(false)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	Player = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Player;

	// SphereComponentの追加と設定
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(Player);
	Sphere->SetSphereRadius(30.f);
	Sphere->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f), false);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AVRActor_ver1::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AVRActor_ver1::OnSphereEndOverlap);

	// Cameraを追加する
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(Sphere);

	// Arrowを追加する
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	Arrow->SetRelativeLocation(FVector(50.f, 0.f, 30.f));
	Arrow->SetupAttachment(Camera);
	// 下の行のコメントアウトを解除するとArrowが見えるようになります※変更後プロジェクトの再起動が必要です
	//Arrow->SetHiddenInGame(false);

	// スプリングアームコンポーネントの追加と設定
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SetupAttachment(Camera);

	// 手のテスト用のSphereを追加する
	InGameHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InGameHand"));
	UStaticMesh* HandMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Game/Satou/Mesh/hand/source/hand"));
	InGameHand->SetStaticMesh(HandMesh);
	UMaterial* HandMeshMaterial = LoadObject<UMaterial>(NULL, TEXT("/Game/Satou/Mesh/hand/source/lambert2"));
	InGameHand->SetMaterial(0, HandMeshMaterial);
	InGameHand->SetupAttachment(SpringArm);

	// チャージ中のナイアガラコンポーネントを追加
	ChargingEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargingMagicEffectComponent"));
	ChargeFinishEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ChargeFinishEffectComponent"));
	ChargingEffect->SetupAttachment(Camera);
	ChargeFinishEffect->SetupAttachment(Camera);
	ChargingEffect->SetHiddenInGame(true);
	ChargeFinishEffect->SetHiddenInGame(true);

	// Input Mapping Context「IMC_TestPad」を読み込む
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Kanda/Input/IMC_TestPad"));
	// インプットアクションの読み込み
	ControlMove  = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_InputMove"));
	MagicCharge  = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_ChargeMagic"));
	ShotMagic    = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_GoMagic"));
	LookAction   = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_Look"));
	MoveStart    = LoadObject<UInputAction>(nullptr, TEXT("/Game/Kanda/Input/IA_MoveAction"));

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
	if (SplineActor)
	{
		FTransform FirstSplineTransform;
		FirstSplineTransform = SplineActor->GetSplineTransform(distance, 0.0f);
		FVector FirstLocation = FVector(FirstSplineTransform.GetLocation());
		SetActorLocation(FirstLocation);
	}

	// 難易度によるパラメーターの調整。現在は０でノーマル、１でハード
	UMagicGameInstance* MagicGame = Cast<UMagicGameInstance>(GetWorld()->GetGameInstance<UMagicGameInstance>());
	switch (MagicGame->Difficulty)
	{
	case 0:
		ArmUpAngle = 30.0f;
		Need_ArmUpDownCnt = MagicGame->Setting_ArmUpDownCnt;
		//Need_ArmUpDownCnt = 3;
		break;
	case 1:
		ArmUpAngle = 45.0f;
		//Need_ArmUpDownCnt = 5;
		break;
	default:
		break;
	}

	// デバイスマネージャーのキャスト
	//DeviceManager_ = Cast<UMagicGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->DeviceManager;

#if PLATFORM_ANDROID
	// 無線デバイスのインスタンス化
	WirelessDevice = NewObject<UWirelessDeviceManager>(this);
	if (!WirelessDevice) return;
	WirelessDevice->Init();
#endif
}

// Called every frame
void AVRActor_ver1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	VRInformation();
	SpawnMagicChargeEffect();
	// デバイスのピッチでSpringArmの角度を変える。SpringArmのヨーを180にしているので、ピッチにマイナスをかける
	SpringArm->SetRelativeRotation(FRotator(-Final_Device_Rotate.Pitch, 180.f, 0));

	ArriveSplinePoint(StopPointNum);
	// スプラインの上を移動していく処理
	if (SplineActor && !isStop) // ただのnullチェック&今停止中かチェック
	{
		FTransform SplineTransform;
		SplineTransform = SplineActor->GetSplineTransform(distance, MoveSpeedPoint * DeltaTime);
		FVector newLocation = FVector(SplineTransform.GetLocation());
		SetActorLocation(newLocation);
	}

	// デバイスマネージャーからデバイスのデータを取得
	//Final_Device_Rotate = DeviceManager_->GetLatestData();
#if PLATFORM_ANDROID
	// 無線デバイスからデバイスのデータを取得
	Final_Device_Rotate.Pitch = WirelessDevice->DevicePitchAngleGetter();
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, FString::Printf(TEXT("pitch = %f"), Final_Device_Rotate.Pitch));
#endif

	// デバイスの角度が0度以下になったら       (腕を下げたら)
	if (Final_Device_Rotate.Pitch < 0 && IsArmUp)
	{
		IsArmUp = false;
		ArmUpDownCnt++;
		DeviceGoMagic();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HandStar,									// エフェクト
			InGameHand->GetComponentLocation(),			// Location
			FRotator(0, 0, 0),							// Rotation
			FVector(1.f),								// Scale
			true,										// bAutoDestroy(基本true)
			true,										// bAutoActivate
			ENCPoolMethod::None,
			true										// bPrecullCheck(カリングチェック)
		);
		// 魔法陣にいるとき、魔法発射の効果音と被るので魔法陣の上にいるかチェック
		if (magicData == nullptr)
		{
			UGameplayStatics::PlaySound2D(this, HandDownSound);
		}
		//手のマテリアルが変わる処理。必要ならコメントアウトを外すこと
		//UMaterial* HandMeshDownMaterial = LoadObject<UMaterial>(NULL, TEXT("/Game/Satou/Materials/M_Red"));
		//InGameHand->SetMaterial(0, HandMeshDownMaterial);
	}

	// デバイスの角度がArmUpAngle以上になったら(腕を上げたら)
	if (Final_Device_Rotate.Pitch > ArmUpAngle)
	{
		// 手を挙げた最初の一回だけ効果音を鳴らす（下ろしたらリセット）
		if (IsArmUp == false)
		{
			UGameplayStatics::PlaySound2D(this, HandUpSound);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				HandStar,									// エフェクト
				InGameHand->GetComponentLocation(),	// Location
				FRotator(0, 0, 0),							// Rotation
				FVector(1.f),								// Scale
				true,										// bAutoDestroy(基本true)
				true,										// bAutoActivate
				ENCPoolMethod::None,
				true										// bPrecullCheck(カリングチェック)
			);
		}
		IsArmUp = true;
		// 魔法陣にいる場合はチャージタイムも増える
		if (IsInMagicZone)
		{
			ChargeMagic();
		}
		//手のマテリアルが変わる処理。必要ならコメントアウトを外すこと
		//UMaterial* HandMeshUpMaterial = LoadObject<UMaterial>(NULL, TEXT("/Game/Satou/Materials/M_Green"));
		//InGameHand->SetMaterial(0, HandMeshUpMaterial);
	}

	// 腕を一定回数以上上げ下げしたら動く
	if (ArmUpDownCnt >= Need_ArmUpDownCnt && !AlreadyMove)
	{
		PlayerMoveStart();
		ArmUpDownCnt = 0;
		AlreadyMove = true;
		//Need_ArmUpDownCnt = 10;
	}

	UKismetSystemLibrary::PrintString(this, IsInMagicZone ? TEXT("IsInMagicZone = true") : TEXT("IsInMagicZone = false"), true, false, FColor::Red, 0.1f, NAME_None);
	//UKismetSystemLibrary::PrintString(this, IsArmUp ? TEXT("true") : TEXT("false"), true, false, FColor::Red, 0.05f, NAME_None);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("updownCNT = %d"), ArmUpDownCnt), true, false, FColor::Green, 0.05f, NAME_None);
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MagicScore = %d"), Magic_Score), true, false, FColor::Blue, 0.05f, NAME_None);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MagicChargeTime = %f"), MagicChargeTime), true, false, FColor::Yellow, 0.1f, NAME_None);
}

// Called to bind functionality to input
void AVRActor_ver1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ControlBallとIA_ControlのTriggeredをBindする
		EnhancedInputComponent->BindAction(ControlMove, ETriggerEvent::Triggered, this, &AVRActor_ver1::ControlPlayer);
		EnhancedInputComponent->BindAction(MagicCharge, ETriggerEvent::Triggered, this, &AVRActor_ver1::MouseChargeMagic);
		EnhancedInputComponent->BindAction(ShotMagic,   ETriggerEvent::Completed, this, &AVRActor_ver1::GoMagic);
		EnhancedInputComponent->BindAction(LookAction,  ETriggerEvent::Triggered, this, &AVRActor_ver1::Look);
		//EnhancedInputComponent->BindAction(MoveStart,    ETriggerEvent::Triggered, this, &AVRActor_ver1::kariPlayerMoveStart);
	}
}

void AVRActor_ver1::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AVRActor_ver1::ControlPlayer(const FInputActionValue& Value)
{
	// inputのValueはVector2Dに変換できる
	const FVector2D V = Value.Get<FVector2D>();

	FVector PreLocation = GetActorLocation();

	FVector NewLocation = PreLocation + Sphere->GetComponentToWorld().TransformVectorNoScale(FVector(V.Y, V.X, 0.0f) * MoveSpeedPoint);

	SetActorLocation(NewLocation);
}

void AVRActor_ver1::ChargeMagic()
{
	MagicChargeTime += GetWorld()->GetDeltaSeconds();
	if (MagicChargeTime >= 5.0f && !Charged)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("5byoutattayo!!!!!!!!!!!!!!!"), true, false, FColor::Cyan, 0.3f);
		Charged = true;
		UGameplayStatics::PlaySound2D(this, ChargeFinishSound);
	}
}

void AVRActor_ver1::MouseChargeMagic(const FInputActionValue& Value)
{
	if (const bool v = Value.Get<bool>() && IsInMagicZone) {
		MagicChargeTime += GetWorld()->GetDeltaSeconds();
		if (MagicChargeTime >= 2.0f && !Charged)
		{
			UKismetSystemLibrary::PrintString(this, TEXT("2byoutattayo!!!!!!!!!!!!!!!"), true, false, FColor::Cyan, 0.3f);
			Charged = true;
			UGameplayStatics::PlaySound2D(this, ChargeFinishSound);
		}
	}
}

// 魔法を撃つ_コントローラーのみ
void AVRActor_ver1::GoMagic()
{
	if (magicData == nullptr) { return; }
	const int cnt = magicData->GetMagicCnt();

	TObjectPtr<UNiagaraSystem> f = magicData->GetFlyNiagaraSystem(0);
	TObjectPtr<UNiagaraSystem> d = magicData->GetDeathNiagaraSystem(0);
	TObjectPtr<UNiagaraSystem> ff = magicData->GetFlyNiagaraSystem(1);

	// 魔法のチャージ時間を計って何の魔法を出すか決める
	// else外したら多分破棄されたポインタ「d」を使うことになるのでクラッシュする
	if (MagicChargeTime <= 2.0f)
	{
		CreateMagic(f, d);
		UGameplayStatics::PlaySound2D(this, NormalMagicSound);
	}
	else
	{
		CreateMagic(ff, d);
		UGameplayStatics::PlaySound2D(this, ChargeMagicSound);
	}

	//チャージ初期化
	MagicChargeTime = 0;
	FTimerHandle ResetHandle;
	GetWorldTimerManager().SetTimer(
		ResetHandle,
		this,
		&AVRActor_ver1::ResetCharged,
		2.0f,
		false
	);
	alreadyChargingMagicEffect = false;
	alreadyChargeFinishMagicEffect = false;
	ChargingEffect->SetHiddenInGame(true);
	ChargeFinishEffect->SetHiddenInGame(true);

	if (magicData->DecMagicCnt()) {
		UKismetSystemLibrary::PrintString(GEngine->GetWorld(), "magicCnt 0");
		magicData = nullptr;

		// 魔法陣を破壊
		circle->Ef_MagicCircle = nullptr;
		circle->Destroy();
	}
}

//チャージリセット
void AVRActor_ver1::ResetCharged()
{
	Charged = false;
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
		FRotator look = Arrow->GetComponentRotation();
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
	//この行をコメントアウトするとマウスでカメラ操作できるようになる
	if (true) return;

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
	if (AOnishi_MagicCircleParent* Pawn = Cast<AOnishi_MagicCircleParent>(OtherActor)) {
		IsInMagicZone = true;
	}
}

// 接触判定の処理、コライダー同士が離れたときに呼び出される
void AVRActor_ver1::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (AOnishi_MagicCircleParent* Pawn = Cast<AOnishi_MagicCircleParent>(OtherActor)) {
		IsInMagicZone = false;
		MagicChargeTime = 0;
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
//lee
// VR機器の情報取得
void AVRActor_ver1::VRInformation()
{
	// 修正：
	// GEngine が存在し、かつ XRSystem (TSharedPtr) が有効(IsValid)であることを確認する。
	//　こうしないと、VRが接続されていないパソコンでplayする際にクラッシュが発生する可能性がある。
	// 元々あった GEngine->XRSystem->HasValidTrackingPosition(); は
	// IsValid() チェックの外にあったため、クラッシュを引き起こしていた。
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->HasValidTrackingPosition();
		// XRSystem が有効な場合のみ、その先の関数を呼び出す。
		if (GEngine->XRSystem->IsHeadTrackingAllowed())
		{



			FQuat OrientationAsQuat;
			FVector Position(0.f);
			FRotator ro;

			// これで安全にVRデバイスの情報を取得できる。
			GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, OrientationAsQuat, Position);
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(ro, Position);
			this->Sphere->SetRelativeRotation(ro);
		}
	}
	// XRSystem が無効な場合（VRが接続されていない場合など）は、
	// この関数は何もしない。
}


// IA_MoveActionに登録されたボタンを押されると行う処理（現在はJキー）
//void AVRActor_ver1::kariPlayerMoveStart(const FInputActionValue& Value)
//{
//	//ストップしている時のみ処理
//	if (isStop)
//	{
//		UKismetSystemLibrary::PrintString(this, TEXT("ugoke"), true, true, FColor::Blue, 2.0f, NAME_None);
//		isStop = false;
//		StopPointNum++;
//	}
//}

// デバイス情報からピッチが一定以上になったら呼ばれる処理（今のところは）
void AVRActor_ver1::PlayerMoveStart()
{
	//ストップしている時のみ処理
	if (isStop)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("ugoke2"), true, true, FColor::Blue, 2.0f, NAME_None);
		isStop = false;
		StopPointNum++;
	}
}

// 指定したスプラインの点に到達すると行われる処理
void AVRActor_ver1::ArriveSplinePoint(int point_)
{
	if (SplineActor) // ただのnullチェック
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
}

void AVRActor_ver1::DeviceGoMagic()
{
	GoMagic();
}

void AVRActor_ver1::SpawnMagicChargeEffect()
{
	FVector a = Camera->GetComponentLocation() + FVector(100, 0, 0);
	if (MagicChargeTime > 2.0f && !alreadyChargingMagicEffect)
	{
		ChargingEffect->SetHiddenInGame(false);
		alreadyChargingMagicEffect = true;
		UKismetSystemLibrary::PrintString(this, TEXT("2secondCHAEGE"), true, false, FColor::Black, 2.0f, NAME_None);
	}
	if (MagicChargeTime > 5.0f && !alreadyChargeFinishMagicEffect)
	{
		ChargeFinishEffect->SetHiddenInGame(false);
		alreadyChargeFinishMagicEffect = true;
	}

}
