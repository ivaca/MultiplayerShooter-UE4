// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGame/Public/SCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/CoopGame.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	HealthCompon = CreateDefaultSubobject<USHealthComponent>("HealthCompon");

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthCompon->OnHealthChange.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerWeapon = Cast<ASWeapon>(GetWorld()->SpawnActor(WeaponClass));
	PlayerWeapon->SetOwner(this);
	PlayerWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
	                                FName("weapon_socket"));
	DefaultFOV = CameraComponent->FieldOfView;
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

float ease = 95.0f;
// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float InterpFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComponent->SetFieldOfView(InterpFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &ASCharacter::BeginADS);
	PlayerInputComponent->BindAction("ADS", IE_Released, this, &ASCharacter::EndADS);


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

void ASCharacter::EndADS()
{
	bWantsToZoom = false;
}

void ASCharacter::BeginADS()
{
	bWantsToZoom = true;
}


void ASCharacter::StartFire()
{
	if (PlayerWeapon)
		PlayerWeapon->StartFire();
}

void ASCharacter::StopFire()
{
	if (PlayerWeapon)
		PlayerWeapon->StopFire();
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComponent)
		return CameraComponent->GetComponentLocation();

	return Super::GetPawnViewLocation();
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta,
                                  const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);
	}
}

void ASCharacter::BeginCrouch()
{
	Crouch();
	UE_LOG(LogTemp, Warning, TEXT("BeginCrouch"));
}

void ASCharacter::EndCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("UnCrouch"));
	UnCrouch();
}
