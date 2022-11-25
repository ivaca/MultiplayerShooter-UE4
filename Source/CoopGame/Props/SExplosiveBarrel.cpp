// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = StaticMesh;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("RadialForceComponent");
	RadialForceComponent->SetupAttachment(StaticMesh);
	RadialForceComponent->bAutoActivate = false;

	HealthComp = CreateDefaultSubobject<USHealthComponent>("HealthComponent");
	bIsDead = false;
	if (GetLocalRole() == ROLE_Authority)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
	}


	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASExplosiveBarrel::OnRep_IsDead()
{
	StaticMesh->SetMaterial(0, ExplodedBarrelMaterial);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta,
                                        const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnRep_IsDead();
		const FVector ImpulseDirection = FVector::UpVector * ExplosionImpulse;
		StaticMesh->AddImpulse(ImpulseDirection, NAME_None, true);
		RadialForceComponent->FireImpulse();
	}
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASExplosiveBarrel, bIsDead);
}
