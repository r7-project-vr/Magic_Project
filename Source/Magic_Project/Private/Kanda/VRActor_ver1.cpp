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


// Sets default values
AVRActor_ver1::AVRActor_ver1() :
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
	IsInMagicZone = false;

	// インスタンス化
	deviceInfo_ = NewObject<UASerialPacket>(this);
	device_ = NewObject<UASerialLibControllerWin>(this);
	deviceCmd_ = NewObject<AMagicDeviceCmdSender>(this);

	// デバイス接続。引数は左からデバイスのIDとデバイスのバージョン
	device_->Initialize(0x02, 0x01);
	device_->SetInterfacePt(new WindowsSerial());
	device_->AutoConnectDevice();
	deviceCmd_->SendCmd_Cali(device_);
}

// Called every frame
void AVRActor_ver1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//VRInformation();
	TimeAccumulator += DeltaTime;


	ArriveSplinePoint(StopPointNum);
	// スプラインの上を移動していく処理
	if (SplineActor && !isStop) // ただのnullチェック&今停止中かチェック
	{
		FTransform transformTemp;
		transformTemp = SplineActor->GetSplineTransform(distance, MoveSpeedPoint * DeltaTime);
		FVector newLocation = FVector(transformTemp.GetLocation());
		SetActorLocation(newLocation);
	}

	// デバイスとの通信
	// if文で通信回数を制限。ヘッダーファイルのIntervalの値でfpsを調整できます。
	if (TimeAccumulator >= Interval)
	{
		TimeAccumulator -= Interval;

		// デバイスにオイラー角取得のコマンドを送る。そのデータをReadDataする
		deviceCmd_->SendCmd_Euler(device_);
		ASerialDataStruct::ASerialData ReceiveData;
		int Result = device_->ReadData(&ReceiveData);

		// ＝＝＝＝＝＝デバッグ情報＝＝＝＝＝＝
		uint16_t a = device_->GetLastErrorCode();
		UE_LOG(LogTemp, Log, TEXT("ErrorCode     = %X"), a);
		UE_LOG(LogTemp, Log, TEXT("deviceCONNECT = %d"), Result);
		UE_LOG(LogTemp, Log, TEXT("deviceRESULT  = %x"), ReceiveData.data);
		// ＝＝＝＝＝＝デバッグ情報＝＝＝＝＝＝

		// デバイスからもらった情報をFRotatorに変換する。1000倍されているので割る1000した値を最終的な値にする。
		FRotator Device_Rotate = TransformEulerAngles(ReceiveData.data, 4);
		Final_Device_Rotate = FRotator(Device_Rotate.Pitch / 1000, Device_Rotate.Yaw / 1000, Device_Rotate.Roll / 1000);
		UE_LOG(LogTemp, Log, TEXT("Final_Device_Rotate.Pitch = %.0f"), Final_Device_Rotate.Pitch);
		UE_LOG(LogTemp, Log, TEXT("Final_Device_Rotate.Yaw = %.0f"),   Final_Device_Rotate.Yaw);
		UE_LOG(LogTemp, Log, TEXT("Final_Device_Rotate.Roll = %.0f"),  Final_Device_Rotate.Roll);
	}

	// デバイスの角度が0度以下になったら       (腕を下げたら)
	if (Final_Device_Rotate.Pitch < 0 && IsArmUp)
	{
		IsArmUp = false;
		ArmUpDownCnt++;
		DeviceGoMagic();
	}
	// デバイスの角度がArmUpAngle以上になったら(腕を上げたら)
	if (Final_Device_Rotate.Pitch > ArmUpAngle)
	{
		IsArmUp = true;
		MagicChargeTime += DeltaTime;
	}
	UKismetSystemLibrary::PrintString(this, IsArmUp ? TEXT("true") : TEXT("false"), true, false, FColor::Red, 0.05f, NAME_None);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("updownCNT = %d"), ArmUpDownCnt), true, false, FColor::Green, 0.05f, NAME_None);

	// 腕を一定回数以上上げ下げしたら動く
	if (ArmUpDownCnt >= Need_ArmUpDownCnt)
	{
		PlayerMoveStart();
		ArmUpDownCnt = 0;
	}
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MagicScore = %d"), Magic_Score), true, false, FColor::Blue, 0.05f, NAME_None);
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MagicChargeTime = %f"), MagicChargeTime), true, false, FColor::Yellow, 0.05f, NAME_None);
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
		//EnhancedInputComponent->BindAction(MoveStart, ETriggerEvent::Triggered, this, &AVRActor_ver1::kariPlayerMoveStart);
	}
}

void AVRActor_ver1::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	device_->DisConnectDevice();

	UE_LOG(LogTemp, Log, TEXT("deviceDisconnected"));

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
				0.3f
			);
			Charged = true;
		}
	}
}

// 魔法を撃つ_コントローラーのみ
void AVRActor_ver1::GoMagic()
{
		if (magicData == nullptr) { return; }
		UKismetSystemLibrary::PrintString(
			this,
			TEXT("HELLO"),
			true,
			true,
			FColor::Red,
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
	IsInMagicZone = true;
}

// 接触判定の処理、コライダー同士が離れたときに呼び出される
void AVRActor_ver1::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	IsInMagicZone = false;
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

// デバイスからもらった情報をTransformEulerAnglesからもらって、int32型に変換して返す
int32 AVRActor_ver1::TransformDataToInt32(const uint8_t* Data, int Size)
{
	int32 Result = 0;

	for (int i = 0; i < Size; ++i)
	{
		Result |= (static_cast<int32>(Data[i]) << (8 * (Size - 1 - i)));
	}
	return Result;
}

// デバイスからもらった情報をTransformDataToInt32に入れて、その結果をFRotatorで返す
FRotator AVRActor_ver1::TransformEulerAngles(const uint8_t* Data, int Size)
{
	std::array<int32, 3> Angles;
	Angles[0] = TransformDataToInt32(Data, Size);       // X
	Angles[1] = TransformDataToInt32(Data + 4, Size);   // Y
	Angles[2] = TransformDataToInt32(Data + 8, Size);   // Z

	// FRotatorの引数は（ピッチ、ヨー、ロール）の順なのでそれにあわせて番号を変えてる
	FRotator ResultRotate = FRotator(Angles[1], Angles[2], Angles[0]);
	return ResultRotate;
}

void AVRActor_ver1::DeviceGoMagic()
{
	GoMagic();
	UKismetSystemLibrary::PrintString(this, TEXT("isInMagicZone = true"), true, false, FColor::Yellow, 2.f, NAME_None);
}