#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "EnemyRat.generated.h"

UCLASS()
class DUNGEONOFAPOSTASY_API AEnemyRat : public ABaseEnemy
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float IdleMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunningMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RandomPointRadius;

protected:
	virtual void BindIdle() override;
	virtual void BindPlayerTooClose() override;
	virtual void IdleMovement() override;
	virtual void PlayerTooClose() override;
};
