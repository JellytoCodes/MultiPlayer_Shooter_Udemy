// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ACasing;
class UNiagaraComponent;
class UWidgetComponent;
class USphereComponent;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	void ShowPickupWidget(const bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Fire(const FVector& HitTarget);

	/** ~Begin Getter & Setter */
	void SetWeaponState(const EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	/** ~End Getter & Setter */

	/**
	 * Textures for the weapon crosshairs
	 */
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsCenter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsLeft = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsRight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsTop = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsBottom = nullptr;

	/**
	 * Zoomed FOV while aiming
	 */
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/**
	 * Automatic fire
	 */

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Fire Effect")
	TObjectPtr<UNiagaraComponent> MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Effect")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Effect")
	FName MuzzleName = "Muzzle";

private :
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere,  Category = "Weapon Properties")
	EWeaponState WeaponState = EWeaponState::EWS_Initial;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass;

};
