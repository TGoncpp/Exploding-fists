// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "HookAttackComp.generated.h"


enum class HookState
{
	Set,
	Punch,
	Pause,
	Retract,
	Reset
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UHookAttackComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHookAttackComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	HookState m_State{ HookState::Reset };

	//Components to move
	UStaticMeshComponent* m_CoreComponent;
	UStaticMeshComponent* m_FistComponent;

	//Component Offsets
	FRotator      m_OffsetCoreRotation;
	FVector       m_OffsetFist;
			      
	float         m_TimeFullyRotated        = m_cPauseTime;

protected:
	//SetValues
	const float    m_cSetFistSpeed          = 100.f;
	const float    m_cSetRotationSpeed      = 50.f;
	const FRotator m_cMaxSetOffset          = FRotator(0.f, -30.f, 0.f);
						         
	//AttackValues		         
	const float    m_cPunchSpeed            = 300.f;
	const float    m_cPauseTime             = 0.5f;
	const FRotator m_cMaxPunchRotation      = FRotator(0.f, 100.f, 0.f);

	//RetractValues
	const float    m_cRetractFistSpeed      = m_cSetFistSpeed ;
	const float    m_cRetractRotationSpeed  = m_cPunchSpeed *0.75f;

	//Functions
private:
	void SetToPunch(float DeltaTime);
	void Punch(float DeltaTime);
	void Retract(float DeltaTime);

	bool IsReadyToPunch()const;
	bool IsFinishedPunch()const;
	bool IsFinishedRetracting()const;

	void UpdateHookState(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable, category = "attack")
	void ResetPos();

	UFUNCTION(BlueprintCallable, category = "attack")
	float GetAttackTime()const;

	UFUNCTION(BlueprintCallable, category = "Init")
	void InitializeComponents(UStaticMeshComponent* core, UStaticMeshComponent* fist);

	//Function to start the attack
	UFUNCTION(BlueprintCallable, category = "attack")
	void Hook();

	UFUNCTION(BlueprintCallable, category = "fight")
	bool IsThrowing()const;

};
