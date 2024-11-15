// Fill out your copyright notice in the Description page of Project Settings.


#include "HookAttackComp.h"

// Sets default values for this component's properties
UHookAttackComp::UHookAttackComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
}


// Called when the game starts
void UHookAttackComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHookAttackComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHookState(DeltaTime);

}




void UHookAttackComp::SetToPunch(float DeltaTime)
{
	//FVector direction = m_FistComponent->GetForwardVector();           //goes to his right
	//FVector direction = m_FistComponent->GetRightVector();             //goes to his back

	//Roate core back
	float offsetRotation       =  m_cSetRotationSpeed* DeltaTime;
	FRotator directionRotation = FRotator(0.f, -1.0f, 0.f);          
	m_OffsetCoreRotation      += offsetRotation * directionRotation;
	m_CoreComponent->AddLocalRotation(offsetRotation * directionRotation, true);
	
	////Move fist away
	float offsetFist        =  m_cSetFistSpeed* DeltaTime;
	FVector directionFist   = FVector(1.f, 0.90f, 0.f);          
	m_OffsetFist           += offsetFist * directionFist;
	m_FistComponent->AddLocalOffset(offsetFist * directionFist, true);

	//Rotate fist more to opponent


}

bool UHookAttackComp::IsReadyToPunch()const
{
	return m_OffsetCoreRotation.Yaw <= m_cMaxSetOffset.Yaw;
}

void UHookAttackComp::Punch(float DeltaTime)
{
	//Punch by rotating core
	float offsetRotation          = m_cPunchSpeed * DeltaTime;
	FRotator directionRotation    = FRotator(0.f, 1.0f, 0.f);
	m_OffsetCoreRotation         += offsetRotation * directionRotation;
	m_CoreComponent->AddLocalRotation(offsetRotation * directionRotation, true);

}

bool UHookAttackComp::IsFinishedPunch()const
{
	return m_OffsetCoreRotation.Yaw >= m_cMaxPunchRotation.Yaw;
}

void UHookAttackComp::Retract(float DeltaTime)
{
	//Roate Head back
	float offsetRotation        = m_cRetractRotationSpeed * DeltaTime;
	FRotator directionRotation  = FRotator(0.f, -1.0f, 0.f);
	m_OffsetCoreRotation       += offsetRotation * directionRotation;
	m_CoreComponent->AddLocalRotation(offsetRotation * directionRotation, true);

	//Move fist back
	float offsetFist           = m_cRetractFistSpeed * DeltaTime;
	FVector directionFist      = FVector(-1.f, -.90f, 0.f);
	m_OffsetFist              += offsetFist * directionFist;
	m_FistComponent->AddLocalOffset(offsetFist * directionFist, true);
}

bool UHookAttackComp::IsFinishedRetracting()const
{
	return m_OffsetCoreRotation.Yaw <= 0.f;
}

void UHookAttackComp::UpdateHookState(float DeltaTime)
{
	switch (m_State)
	{
	case HookState::Reset:
		break;

	case HookState::Set:
		SetToPunch(DeltaTime);
		if (IsReadyToPunch())
			m_State = HookState::Punch;
		break;

	case HookState::Punch:
		Punch(DeltaTime);
		if (IsFinishedPunch())
			m_State = HookState::Pause;
		break;

	case HookState::Pause:
		m_TimeFullyRotated -= DeltaTime;
		if (m_TimeFullyRotated <= 0.f)
		{
			m_State = HookState::Retract;
			m_TimeFullyRotated = m_cPauseTime;
		}
		break;

	case HookState::Retract:
		Retract(DeltaTime);
		if (IsFinishedRetracting())
		{
			m_State = HookState::Reset;
			ResetPos();
		}
		break;

	default:
		break;
	}
}

void UHookAttackComp::ResetPos()
{
	//Reset position components
	FRotator negOffset = FRotator(m_OffsetCoreRotation.Pitch, -m_OffsetCoreRotation.Yaw, m_OffsetCoreRotation.Roll);
	m_CoreComponent->AddLocalRotation((negOffset));
	m_FistComponent->AddLocalOffset(-m_OffsetFist, true);

	//Reset the offset Values
	m_OffsetCoreRotation = FRotator(0.f, 0.f, 0.f);
	m_OffsetFist         = FVector(0.f, 0.f, 0.f);

	//Reset state if called after hit
	m_State = HookState::Reset;
}

float UHookAttackComp::GetAttackTime() const
{
	return m_cMaxSetOffset.Yaw/m_cSetRotationSpeed + m_cPauseTime + m_cMaxPunchRotation.Yaw/m_cRetractFistSpeed;
}

void UHookAttackComp::InitializeComponents( UStaticMeshComponent* core, UStaticMeshComponent* fist)
{
	m_CoreComponent = core;
	m_FistComponent = fist;
}

void UHookAttackComp::Hook()
{
	if (m_State != HookState::Reset)
		return;
	m_State = HookState::Set;
}

bool UHookAttackComp::IsThrowing() const
{
	return m_State == HookState::Punch;
}

