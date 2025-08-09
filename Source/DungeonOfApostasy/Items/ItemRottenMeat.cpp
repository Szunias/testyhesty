
#include "ItemRottenMeat.h"
#include "../Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/StatsComponent.h"
#include "../Characters/Enemies/BaseEnemy.h"
#include "Components/SphereComponent.h"

AItemRottenMeat::AItemRottenMeat()
{
	ItemName = "Rotten Meat";
	IsBeingEaten = false;
}

void AItemRottenMeat::Use()
{
	APlayerCharacter* pc{ Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) };
	if (IsValid(pc))
	{
		pc->GetStatsComponent()->UpdateHealth(-ConsumeDamage);
		OnItemUsed.Broadcast();
	}
}

void AItemRottenMeat::Throw()
{
	/*no effect in here, its effect will be passive*/
	Mesh->OnComponentBeginOverlap.Clear();
	ShowItem();
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AItemRottenMeat::OnThrowOverlapBegin);
	Mesh->OnComponentHit.AddDynamic(this, &AItemRottenMeat::OnThrowHit);
}

void AItemRottenMeat::BeginPlay()
{
	Super::BeginPlay();
}

void AItemRottenMeat::OnThrowOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (IsBeingEaten)
	{
		return;
	}

	if (otherActor && (otherActor != this))
	{
		if (!otherActor->IsA<APlayerCharacter>())
		{
			StopOnLowVelocity();
		}
		if (otherActor->IsA<ABaseEnemy>() && !otherComp->IsA<USphereComponent>())
		{
			ActivateMeat();
		}
	}
}

void AItemRottenMeat::OnThrowHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	if (IsBeingEaten)
	{
		return;
	}

	if (otherActor && (otherActor != this))
	{		
		if (!otherActor->IsA<APlayerCharacter>())
		{
			StopOnLowVelocity();
		}
		if (otherActor->IsA<ABaseEnemy>() && !otherComponent->IsA<USphereComponent>())
		{
			ActivateMeat();
		}
	}
}

void AItemRottenMeat::StopOnLowVelocity()
{
	if (GetVelocity().IsNearlyZero(0.1f))
	{
		ActivatePhysics(false);		
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AItemRottenMeat::StopOnLowVelocity);
	}
}

void AItemRottenMeat::DestroyItem()
{
	Destroy();
}

void AItemRottenMeat::ActivateMeat()
{
	IsBeingEaten = true;

	FTimerHandle timerHandle{};
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AItemRottenMeat::DestroyItem, EnemyEatTimer);

	Mesh->SetGenerateOverlapEvents(false);
}
