// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "HAL/IConsoleManager.h"
#include "DrawDebugHelpers.h"
#include "CoopGame/CoopGame.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

DEFINE_LOG_CATEGORY(WeaponLog);
int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing,
                                               TEXT("Draw debug lines for weapons."), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleFlashSocket";

	BaseDamage = 20.0f;
	HeadshotDamageModifier = 2.5f;
	FireRate = 600;
	CurrentBulletSpread = 0.0f;
	BulletSpread = 1.5f;
	BulletSpreadInterpSpeed = 1.0f;

	SetReplicates(true);

}


void ASWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
		return;
	}
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const auto HalfRad = FMath::DegreesToRadians(CurrentBulletSpread);

	FVector ShotDir = EyeRotation.Vector();


	HitSpread = FMath::VRandCone(ShotDir, HalfRad);


	FVector TraceEnd = EyeLocation + HitSpread * 10000;


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;
	EPhysicalSurface SurfaceType = SurfaceType_Default;

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams) || !
		HitResult.
		bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Cyan, false, 1.0f, 0, 1.0f);
		}
		if (HitResult.bBlockingHit)
		{
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= HeadshotDamageModifier;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDir, HitResult,
			                                   MyOwner->GetInstigatorController(),
			                                   this, DamageType);
			PlayImpactEffects(SurfaceType, HitResult.ImpactPoint);
			TraceEnd = HitResult.ImpactPoint;
		}
		PlayFireEffects();
		PlayerCameraShake();
		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceEnd = TraceEnd;
			HitScanTrace.SurfaceType = SurfaceType;
			
		}
	}
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentBulletSpread = FMath::FInterpTo(CurrentBulletSpread, BulletSpread, GetWorld()->DeltaTimeSeconds,
		                                       BulletSpreadInterpSpeed);
	}
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}
	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint,
		                                         ShotDirection.Rotation());
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	UE_LOG(WeaponLog, Warning, TEXT("QQQWE"));
	PlayFireEffects();
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceEnd);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

void ASWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASWeapon, HitScanTrace);
	DOREPLIFETIME(ASWeapon, HitSpread);
	DOREPLIFETIME(ASWeapon, CurrentBulletSpread);
}


void ASWeapon::StartFire()
{
	GetWorldTimerManager().SetTimer(TH_AutomaticFireTimer, this, &ASWeapon::Fire, 60.0f / FireRate, true, 0.0f);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TH_AutomaticFireTimer);
	if (GetLocalRole() == ROLE_Authority)
		CurrentBulletSpread = 0.0f;
	else
	{
		ServerStopFire();
	}
}

void ASWeapon::PlayFireEffects()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}
}

void ASWeapon::PlayerCameraShake()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (!MyOwner) return;
	APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
	if (!PlayerController)return;
	PlayerController->ClientPlayCameraShake(FireCamShake);
}
