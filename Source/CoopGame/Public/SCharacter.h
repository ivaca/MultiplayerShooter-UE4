// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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


	UPROPERTY(EditAnywhere, Category="Components")
	TSubclassOf<ASWeapon> WeaponClass;

	UPROPERTY(BlueprintReadOnly)
	AActor* PlayerWeapon;

	void BeginCrouch();
	void EndCrouch();

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly,Category="Player", meta=(ClampMin=0.1, ClampMax=100.0))
	float ZoomInterpSpeed;

	float DefaultFOV;

	void EndADS();
	void BeginADS();

	void Fire();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
};
