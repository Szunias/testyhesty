#pragma once

#include "CoreMinimal.h"
#include "../Other/InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UTexture2D;
class UNiagaraComponent;

UCLASS()
class DUNGEONOFAPOSTASY_API ABaseItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ABaseItem();

	virtual void Use();
	virtual void Throw();

	virtual void Interact() override;

	void ActivatePhysics(bool activate);

	virtual bool CanBeThrown() const;

	bool HasDelayedItemEffect();

	void SetPickupAble(bool isPickupAble);

	void LaunchItem(const FVector& launchVelocity);

	void TeleportItem(const FVector& newLocation);

	UFUNCTION(BlueprintCallable)
	const FString& GetItemName();

	UFUNCTION(BlueprintCallable)
	const FString& GetItemDescription();

	UFUNCTION(BlueprintCallable)
	int GetItemWeight();

	UFUNCTION(BlueprintCallable)
	virtual UTexture2D* GetItemImage();

	TSubclassOf<AActor> GetItemMesh() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemDescription;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemUsed);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Item")
	FOnItemUsed OnItemUsed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemThrown);
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Item")
	FOnItemThrown OnItemThrown;

protected:
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* ItemImage;

	UPROPERTY()
	UStaticMeshComponent* Mesh;

	bool HasDelayedEffect;

	bool CanItemBeThrown;

	virtual void BeginPlay() override;

	void ShowItem(bool visible = true);

	virtual void HandleDestroy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MeshToEquip;

private:

	void ClearCollisions();

	UPROPERTY()
	UNiagaraComponent* InteractParticles;

	bool PickupAble;

	bool IsBeingDestroyed;
};
