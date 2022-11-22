// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "Kismet/GameplayStatics.h"

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
	HealthComp->OnHealthChange.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
	bIsDead = false;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta,
                                        const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		StaticMesh->SetMaterial(0, ExplodedBarrelMaterial);
		const FVector ImpulseDirection = FVector::UpVector * ExplosionImpulse;
		StaticMesh->AddImpulse(ImpulseDirection, NAME_None, true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		RadialForceComponent->FireImpulse();
	}
}
