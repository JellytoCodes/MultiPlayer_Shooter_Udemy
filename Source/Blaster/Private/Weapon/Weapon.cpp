// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Weapon.h"

#include "NiagaraComponent.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Weapon/Casing.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());

	MuzzleEffect = CreateDefaultSubobject<UNiagaraComponent>("MuzzleEffect");
	MuzzleEffect->SetAutoActivate(false);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget) PickupWidget->SetVisibility(false);

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();	
	}
}

void AWeapon::SetWeaponState(const EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped :
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

bool AWeapon::IsAmmoEmpty()
{
	return Ammo == 0;
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped :
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}

void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if (BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if (BlasterOwnerController)
		{
			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::ShowPickupWidget(const bool bShowWidget)
{
	if (PickupWidget) PickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (MuzzleEffect == nullptr || FireSound == nullptr) return;

	FTransform MuzzlePosition = WeaponMesh->GetSocketTransform(MuzzleName);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzlePosition.GetLocation(), 1.f);
	MuzzleEffect->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, MuzzleName);
	MuzzleEffect->SetActive(true);

	if (CasingClass)
	{
		if (const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject")))
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			if (UWorld* World = GetWorld())
			{
				World->SpawnActor<ACasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
			}
		}
	}
	SpendRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}
