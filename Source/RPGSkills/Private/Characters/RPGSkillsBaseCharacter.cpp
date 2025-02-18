// WillTheWater

#include "Characters/RPGSkillsBaseCharacter.h"

#include <filesystem>

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Controllers/RPGSkillsPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/RPGOverlayUI.h"
#include "Components/SkeletalMeshComponent.h"
#include "Actors/BombBase.h"
#include "Actors/Ice.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "particles/ParticleSystemComponent.h"

ARPGSkillsBaseCharacter::ARPGSkillsBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Glider = CreateDefaultSubobject<USkeletalMeshComponent>("Glider");
	Glider->SetupAttachment(GetMesh());
	Glider->SetVisibility(false);

	BombReadyPosition = CreateDefaultSubobject<USceneComponent>("Bomb Ready Postion");
	BombReadyPosition->SetupAttachment(GetMesh(), TEXT("socket_head"));

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>("Physics Handle");
	PhysicsHandle->LinearDamping = 100.0f;
	PhysicsHandle->AngularDamping = 250.0f;
	PhysicsHandle->LinearStiffness = 325.0f;
	PhysicsHandle->AngularStiffness = 750.0f;
	PhysicsHandle->InterpolationSpeed = 5.0f;

	PhysicsObjectHolder = CreateDefaultSubobject<USceneComponent>("Physics Object Holder");
	PhysicsObjectHolder->SetupAttachment(FollowCamera);
}

void ARPGSkillsBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	ARPGSkillsPlayerController* PC = Cast<ARPGSkillsPlayerController>(Controller);
	if (PC == nullptr) { return; }

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (Subsystem == nullptr) { return; }

	Subsystem->AddMappingContext(IMC_RPGSkills, 0);

	CurrentStamina = MaxStamina;

	if (OverlayClassReference)
	{
		UIReference = CreateWidget<URPGOverlayUI>(GetWorld(), OverlayClassReference);
		if (UIReference)
		{
			UIReference->PlayerReference = this;
			UIReference->AddToViewport();
		}
	}

	FilterOutAllMetalObjects();
}

void ARPGSkillsBaseCharacter::Landed(const FHitResult& Hit)
{
	if (CurrentMT == EMovementTypes::MT_EXHAUSTED)
	{
		StartStaminaRecovery();
		return;
	}
	if (CurrentMT == EMovementTypes::MT_GLIDING)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
		return;
	}

	LocomotionManager(EMovementTypes::MT_WALKING);

}

void ARPGSkillsBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MagDragObjectTick();
	UpdateIceTick(DeltaTime);
}

void ARPGSkillsBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EIComponent == nullptr) { return; }
	EIComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Move);
	EIComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::MoveReleased);
	EIComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Look);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ARPGSkillsBaseCharacter::Sprint);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::SprintReleased);
	EIComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::SprintStarted);
	EIComponent->BindAction(JumpGlideAction, ETriggerEvent::Completed, this, &ARPGSkillsBaseCharacter::JumpGlideReleased);
	EIComponent->BindAction(JumpGlideAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::JumpGlideStarted);
	EIComponent->BindAction(ToggleUIAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::ToggleUIStarted);
	EIComponent->BindAction(PrepareSkillAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::PrepareSkillStarted);
	EIComponent->BindAction(CastSkillAction, ETriggerEvent::Started, this, &ARPGSkillsBaseCharacter::CastSkillStarted);

}

void ARPGSkillsBaseCharacter::LocomotionManager(EMovementTypes NewMovementType)
{
	if (CurrentMT == NewMovementType) { return; }
	else { CurrentMT = NewMovementType; }

	if (Glider)
	{
		Glider->SetVisibility(CurrentMT == EMovementTypes::MT_GLIDING);
	}

	switch (CurrentMT)
	{
	case EMovementTypes::MT_EMAX:
		break;
	case EMovementTypes::MT_WALKING:
		SetWalking();
		break;
	case EMovementTypes::MT_EXHAUSTED:
		SetExhausted();
		break;
	case EMovementTypes::MT_SPRINTING:
		SetSprint();
		break;
	case EMovementTypes::MT_GLIDING:
		SetGliding();
		break;
	case EMovementTypes::MT_FALLING:
		SetFalling();
		break;
	default:
		break;
	}
}

void ARPGSkillsBaseCharacter::Move(const FInputActionValue& Value)
{
	FVector2D VectorData = Value.Get<FVector2D>();
	VelocityX = VectorData.X;
	VelocityY = VectorData.Y;

	if (Controller == nullptr) { return; }
	FRotator YawRotator = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);


	FVector WorldDirectionX = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
	FVector WorldDirectionY = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
	AddMovementInput(WorldDirectionX, VelocityX);
	AddMovementInput(WorldDirectionY, VelocityY);
}

void ARPGSkillsBaseCharacter::MoveReleased(const FInputActionValue& Value)
{
	VelocityX = 0.f;
	VelocityY = 0.f;
}

void ARPGSkillsBaseCharacter::Look(const FInputActionValue& Value)
{
	FVector2D ControllerRotation = Value.Get<FVector2D>();
	AddControllerYawInput(ControllerRotation.X);
	AddControllerPitchInput(ControllerRotation.Y);
}

void ARPGSkillsBaseCharacter::Sprint(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_SPRINTING && VelocityX == 0.f && VelocityY == 0.f)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
	}
}

void ARPGSkillsBaseCharacter::SprintReleased(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_SPRINTING)
	{
		LocomotionManager(EMovementTypes::MT_WALKING);
	}
}

void ARPGSkillsBaseCharacter::SprintStarted(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_EMAX || CurrentMT == EMovementTypes::MT_WALKING)
	{
		LocomotionManager(EMovementTypes::MT_SPRINTING);
	}
}

void ARPGSkillsBaseCharacter::JumpGlideStarted(const FInputActionValue& Value)
{
	if (CurrentMT == EMovementTypes::MT_EXHAUSTED) { return; }
	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		Jump();
		LocomotionManager(EMovementTypes::MT_FALLING);
		return;
	}

	if (CurrentMT == EMovementTypes::MT_GLIDING)
	{
		LocomotionManager(EMovementTypes::MT_FALLING);
		return;
	}

	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + EnableGlideDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.f, 0, 3.f);
	if (bHit)
	{

	}
	else
	{
		DeavtivateAllSkills();
		LocomotionManager(EMovementTypes::MT_GLIDING);
	}
}

void ARPGSkillsBaseCharacter::JumpGlideReleased(const FInputActionValue& Value)
{
	StopJumping();
}

void ARPGSkillsBaseCharacter::DeavtivateAllSkills()
{
	if (bRBActivated || bMAGActivated || bStasisActivated || bIceActivated)
	{
		ToggleSkillActivity();
		bReadyToThrow = false;
	}
}

void ARPGSkillsBaseCharacter::FilterOutAllMetalObjects()
{
	TArray<AActor*> TempMetalObjects;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticMeshClass, TempMetalObjects);

	for (AActor* MetalObject : TempMetalObjects)
	{
		AStaticMeshActor* MetalObjectActor = Cast<AStaticMeshActor>(MetalObject);
		EPhysicalSurface SurfaceMaterial = MetalObjectActor->GetStaticMeshComponent()->GetMaterial(0)->GetPhysicalMaterial()->SurfaceType;
		if (SurfaceMaterial == SurfaceType1)
		{
			MetalActors.AddUnique(MetalObjectActor);
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::MakeRandomColor(), MetalObjectActor->GetName());
			
		}
	}
}

void ARPGSkillsBaseCharacter::ToggleUIStarted(const FInputActionValue& Value)
{
	// TODO Cancel Cast
	ARPGSkillsPlayerController* PC = Cast<ARPGSkillsPlayerController>(Controller);

	DeavtivateAllSkills();

	if (GetWidgetSwitcherInfo() == 1)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetPause(false);
		SetWidgetSwitcherIndex(0);
	}
	else
	{
		PC->bShowMouseCursor = true;
		FInputModeGameAndUI InputHandle;
		InputHandle.SetWidgetToFocus(UIReference->TakeWidget());
		PC->SetInputMode(InputHandle);
		PC->SetPause(true);
		SetWidgetSwitcherIndex(1);

	}
}

void ARPGSkillsBaseCharacter::PrepareSkillStarted(const FInputActionValue& Value)
{
	ToggleSkillActivity();
}

void ARPGSkillsBaseCharacter::CastSkillStarted(const FInputActionValue& Value)
{
	switch (ActiveSkill)
	{
	case ESkills::SK_EMAX:
		break;
	case ESkills::SK_RBB:
		ThrowAndIgniteBomb(false);
		break;
	case ESkills::SK_RBS:
		ThrowAndIgniteBomb(true);
		break;
	case ESkills::SK_MAG:
		SelectOrReleaseMagObject();
		break;
	case ESkills::SK_STASIS:
		break;
	case ESkills::SK_ICE:
		CreateIce();
		break;
	default:
		break;
	}
}

void ARPGSkillsBaseCharacter::ToggleSkillActivity()
{
	if (ActiveSkill == ESkills::SK_EMAX) { return; }
	bShowCrosshair = !bShowCrosshair;
	CrosshairAndCameraMode(bShowCrosshair);
	LocomotionManager(EMovementTypes::MT_WALKING);
	switch (ActiveSkill)
	{
	case ESkills::SK_EMAX:
		break;
	case ESkills::SK_RBB:
		ToggleRemoteBomb();
		break;
	case ESkills::SK_RBS:
		ToggleRemoteBomb();
		break;
	case ESkills::SK_MAG:
		ToggleMagnesis();
		break;
	case ESkills::SK_STASIS:
		break;
	case ESkills::SK_ICE:
		ToggleIceMode();
		break;
	default:
		break;
	}
}

void ARPGSkillsBaseCharacter::ToggleRemoteBomb()
{
	if (bRBActivated)
	{
		bHandleBomb = false;
		bReadyToThrow = false;
		if (BombReference)
		{
			BombReference->Destroy();
			BombReference = nullptr;
		}
	}
	bRBActivated = !bRBActivated;
}

void ARPGSkillsBaseCharacter::ToggleMagnesis()
{
	if (bMAGActivated)
	{
		ReleaseMagnesis();
	}
	bMAGActivated = !bMAGActivated;
	UpdateMetalMaterial(MetalActors, nullptr);
}

void ARPGSkillsBaseCharacter::ToggleIceMode()
{
	if (bIceActivated)
	{
		DeactivateIce();
	}
	else
	{
		ActivateIce();
	}
}

void ARPGSkillsBaseCharacter::ReleaseMagnesis()
{
	PhysicsHandle->ReleaseComponent();
	PhysicsObjectHolder->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	MagnesisObject = nullptr;
	TempMagHitComp = nullptr;
	if (ParticleBeam)
	{
		ParticleBeam->DestroyComponent();
		ParticleBeam = nullptr;
	}
}

void ARPGSkillsBaseCharacter::UpdateMetalMaterial(TArray<AStaticMeshActor*> MetalArrayObject,
	UPrimitiveComponent* HoverMetalObject)
{
	for (AStaticMeshActor* MetalObjectActor : MetalArrayObject)
	{
		if (MetalObjectActor == nullptr) {return;}
		UStaticMeshComponent* MetalObjectMesh = MetalObjectActor->GetStaticMeshComponent();
		if (HoverMetalObject == MetalObjectMesh)
		{
			MetalObjectMesh->SetMaterial(0, MagnesisHoverMaterial);
		}
		else
		{
			if (bMAGActivated)
			{
				MetalObjectMesh->SetMaterial(0, MagnesisNormalMaterial);

			}
			else
			{
				MetalObjectMesh->SetMaterial(0, MagnesisDeactiveMaterial);
			}
		}
	}
}

void ARPGSkillsBaseCharacter::SelectOrReleaseMagObject()
{
	if (!bMAGActivated) { return; }
	GrabMagObject();
}

void ARPGSkillsBaseCharacter::GrabMagObject()
{
	if (MagnesisObject)
	{
		ReleaseMagnesis();
		return;
	}
	FHitResult HitResult;
	FVector Start;
	FVector End;
	CameraLineTraceDirection(Start, End, 3000.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (!HitResult.bBlockingHit) { return; }
	MagnesisObject = HitResult.GetComponent();
	if (MagnesisObject == nullptr) { return; }

	if (!MagnesisObject->IsSimulatingPhysics()) return;
	EPhysicalSurface Surface = UGameplayStatics::GetSurfaceType(HitResult);
	if (Surface != SurfaceType1) { return; }

	FName CustomName;
	FVector GrabLocation = MagnesisObject->GetComponentLocation();
	FRotator GrabRotation(0.f, MagnesisObject->GetComponentRotation().Yaw, 0.f);
	PhysicsHandle->GrabComponentAtLocationWithRotation(MagnesisObject, CustomName, GrabLocation, GrabRotation);
	PhysicsObjectHolder->SetWorldLocation(MagnesisObject->GetComponentLocation());

	ParticleBeam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MagnesisParticle, GetActorLocation());
}

void ARPGSkillsBaseCharacter::CameraLineTraceDirection(FVector& Start, FVector& End, const float Length)
{
	Start = FollowCamera->GetComponentLocation();
	End = Start + FollowCamera->GetForwardVector() * Length;
}

void ARPGSkillsBaseCharacter::MagDragObjectTick()
{
	if (!bMAGActivated) { return; }
	if (MagnesisObject)
	{
		FVector NewLocation = PhysicsObjectHolder->GetComponentLocation();
		FRotator NewRotation(0.f, PhysicsObjectHolder->GetComponentRotation().Yaw, 0.f);
		PhysicsHandle->SetTargetLocationAndRotation(NewLocation, NewRotation);

		if (ParticleBeam)
		{
			ParticleBeam->SetBeamSourcePoint(0, GetActorLocation(), 0);
			ParticleBeam->SetBeamSourcePoint(1, GetActorLocation(), 0);
			ParticleBeam->SetBeamSourcePoint(2, GetActorLocation(), 0);
			ParticleBeam->SetBeamTargetPoint(0, MagnesisObject->GetComponentLocation(), 0);
			ParticleBeam->SetBeamTargetPoint(1, MagnesisObject->GetComponentLocation(), 0);
			ParticleBeam->SetBeamTargetPoint(2, MagnesisObject->GetComponentLocation(), 0);
		}
	}
	else
	{
		FHitResult HitResult;
		FVector Start;
		FVector End;
		CameraLineTraceDirection(Start, End, 3000.f);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.bReturnPhysicalMaterial = true;
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		if (!HitResult.bBlockingHit)
		{
			TempMagHitComp = nullptr;
			UpdateMetalMaterial(MetalActors, nullptr);
		}
		else
		{
			if (HitResult.GetComponent() == nullptr) {return;}
			if (HitResult.GetComponent() == TempMagHitComp) { return; }
			TempMagHitComp = HitResult.GetComponent();
			EPhysicalSurface Surface = UGameplayStatics::GetSurfaceType(HitResult);
			if (Surface == SurfaceType1 && TempMagHitComp->IsSimulatingPhysics())
			{
				UpdateMetalMaterial(MetalActors, TempMagHitComp);
			}
			else
			{
				UpdateMetalMaterial(MetalActors, nullptr);
			}
		}
	}
}

void ARPGSkillsBaseCharacter::ActivateIce()
{
	FHitResult HitResult;
	FVector Start;
	FVector End;
	CameraLineTraceDirection(Start, End, 3000.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (!HitResult.bBlockingHit) { return; }
	AActor* TempActor = GetWorld()->SpawnActor<AActor>(IceActorClass, HitResult.Location, FRotator::ZeroRotator);
	if (!TempActor) { return; }
	IceReference = Cast<AIce>(TempActor);
	if (!IceReference) { return; }
	bIceActivated = true;

	// TODO
}

void ARPGSkillsBaseCharacter::DeactivateIce()
{
	// TODO
	if (IceReference)
	{
		IceReference->Destroy();
		IceReference = nullptr;
	}
	bIceActivated = false;
}

void ARPGSkillsBaseCharacter::UpdateIceTick(float DeltaTime)
{
	if (!IceReference || !bIceActivated) { return; }
	FHitResult HitResult;
	FVector Start;
	FVector End;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	CameraLineTraceDirection(Start, End, 5000.f);
	bool bHitAnything = GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectParams, Params);

	if (!bHitAnything)
	{
		IceReference->BaseSceneRoot->SetVisibility(false, true);
		return;
	}

	IceReference->BaseSceneRoot->SetVisibility(true, true);
	FVector TargetLocation = HitResult.Location;
	FVector FinalLocation = UKismetMathLibrary::VInterpTo(IceReference->GetActorLocation(), TargetLocation, DeltaTime, 10.f);
	IceReference->SetActorLocation(FinalLocation);
	FRotator Rotator = UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal);
	IceReference->SetActorRotation(FRotator(Rotator));

	IceReference->bCanPlace = IceReference->CheckOverlapping();
	if (IceReference->bCanPlace)
	{
		IceReference->IceMesh->SetMaterial(0, IceEnableMaterial);
	}
	else
	{
		IceReference->IceMesh->SetMaterial(0, IceDisableMaterial);		
	}
	
}

void ARPGSkillsBaseCharacter::CreateIce()
{
	if (bIceActivated && IceReference)
	{
		IceReference->SpawnIce();
	}
}

bool ARPGSkillsBaseCharacter::IsCharacterExausted()
{
	bool bEqual = CurrentMT == EMovementTypes::MT_EXHAUSTED;
	return bEqual;
}

void ARPGSkillsBaseCharacter::SetSprint()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Sprinting");
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->AirControl = 0.35f;
	ResetToWalk();
	StartStaminaDrain();
}

void ARPGSkillsBaseCharacter::ResetToWalk()
{
	GetWorldTimerManager().ClearTimer(AddGravityTimerHandle);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ARPGSkillsBaseCharacter::SetWalking()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, "Set Walking");
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->AirControl = 0.05f;
	ResetToWalk();
	StartStaminaRecovery();
}

void ARPGSkillsBaseCharacter::SetExhausted()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->AirControl = 0.35;
	ClearStaminaTimers();
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		StartStaminaRecovery();
	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		ResetToWalk();
	}
	else { return; }
}

void ARPGSkillsBaseCharacter::SetGliding()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->AirControl = 0.6f;
	StartStaminaDrain();
	GetWorldTimerManager().SetTimer(AddGravityTimerHandle, this, &ARPGSkillsBaseCharacter::AddGravityTimer, GetWorld()->GetDeltaSeconds(), true);
}

void ARPGSkillsBaseCharacter::SetFalling()
{
	ClearStaminaTimers();
	ResetToWalk();
	GetCharacterMovement()->AirControl = 0.35f;
}

void ARPGSkillsBaseCharacter::ThrowAndIgniteBomb(bool bSphere)
{
	if (!bRBActivated) {return;}
	if (BombReference)
	{
		if (bHandleBomb)
		{
			BombReference->MeshComponent->SetSimulatePhysics(true);
			BombReference->MeshComponent->SetPhysicsLinearVelocity(GetThrowDirection());
			bHandleBomb = false;
			bReadyToThrow = false;
		}
		else
		{
			BombReference->Detonate();
			BombReference = nullptr;
		}
	}
	else
	{
		TSubclassOf<ABombBase> BombInstance;
		if (bSphere)
		{
			BombInstance = SphereBombClass;
		}
		else
		{
			BombInstance = BoxBombClass;
		}
		if (BombInstance == nullptr) {return;}
		FVector SpawnLocation = BombReadyPosition->GetComponentLocation();
		BombReference = GetWorld()->SpawnActor<ABombBase>(BombInstance, SpawnLocation, FRotator::ZeroRotator);
		BombReference->AttachToComponent(BombReadyPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		bHandleBomb = true;
		bReadyToThrow = true;
	}
}

const FVector ARPGSkillsBaseCharacter::GetThrowDirection()
{
	FVector Final = FVector(FollowCamera->GetForwardVector().X, FollowCamera->GetForwardVector().Y, 0.f);
	//Final.GetSafeNormal(.001f);
	Final = Final + FVector(0, 0, 0.5f);
	Final = Final * 1000.f;
	return Final;
}

void ARPGSkillsBaseCharacter::DrainStaminaTimer()
{
	if (CurrentStamina <= 0.f)
	{
		LocomotionManager(EMovementTypes::MT_EXHAUSTED);
	}
	else
	{
		CurrentStamina = FMath::Clamp((CurrentStamina - StaminaLossRate), 0.f, MaxStamina);
	}
}

void ARPGSkillsBaseCharacter::StartStaminaDrain()
{
	ClearStaminaTimers();
	GetWorldTimerManager().SetTimer(DrainStaminaTimerHandle, this, &ARPGSkillsBaseCharacter::DrainStaminaTimer, StaminaLossRate, true);

	if (UIReference)
	{
		UIReference->ShowGaugeAnim(true);
	}
}

void ARPGSkillsBaseCharacter::RecoverStaminaTimer()
{
	if (CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp((CurrentStamina + StaminaLossRate), 0.f, MaxStamina);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
		LocomotionManager(EMovementTypes::MT_WALKING);

		if (UIReference)
		{
			UIReference->ShowGaugeAnim(false);
		}
	}
}

void ARPGSkillsBaseCharacter::StartStaminaRecovery()
{
	ClearStaminaTimers();
	GetWorldTimerManager().SetTimer(RecoverStaminaTimerHandle, this, &ARPGSkillsBaseCharacter::RecoverStaminaTimer, StaminaLossRate, true);
}

void ARPGSkillsBaseCharacter::ClearStaminaTimers()
{
	GetWorldTimerManager().ClearTimer(DrainStaminaTimerHandle);
	GetWorldTimerManager().ClearTimer(RecoverStaminaTimerHandle);
}

void ARPGSkillsBaseCharacter::AddGravityTimer()
{
	LaunchCharacter(FVector(0.f, 0.f, - 100.f), false, true);
}

