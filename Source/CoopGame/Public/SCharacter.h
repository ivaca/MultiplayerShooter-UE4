// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHealthComponent.h"
#include "SWeapon.h"

#include "GameFramework/Character.h"

#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USHealthComponent* HealthCompon;

	UPROPERTY(EditAnywhere, Category="Components")
	TSubclassOf<ASWeapon> WeaponClass;

	UPROPERTY(BlueprintReadOnly)
	ASWeapon* PlayerWeapon;

	
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category="Player", meta=(ClampMin=0.1, ClampMax=100.0))
	float ZoomInterpSpeed;

	float DefaultFOV;

	

	UFUNCTION()
	void OnHealthChanged(class USHealthComponent* HealthComponent, float Health, float
	                     HealthDelta, const class UDamageType* DamageType,
	                     class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(BlueprintReadOnly, Category="Player")
	bool bDied;


	void BeginCrouch();
	void EndCrouch();

	void EndADS();
	void BeginADS();
	
	void StartFire();
	void StopFire();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
};
