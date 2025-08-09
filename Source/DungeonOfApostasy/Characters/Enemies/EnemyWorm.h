#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "EnemyWorm.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AEnemyWorm : public ABaseEnemy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RandomPointRadius;

protected:
	virtual void BeginPlay() override;

	virtual void BindIdle() override;
	virtual void BindPlayerTooClose() override;
	virtual void IdleMovement() override;
	virtual void PlayerTooClose() override;
};
