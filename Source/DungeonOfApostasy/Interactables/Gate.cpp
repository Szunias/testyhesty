
#include "Gate.h"
#include "CameraTrigger.h"

AGate::AGate()
{
	PrimaryActorTick.bCanEverTick = false;

	GateOpenTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("GateOpenTimelineComp"));
	GateCloseTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("GateCloseTimelineComp"));

	IsOpen = false;
}

void AGate::BeginPlay()
{
	Super::BeginPlay();

	GateMesh = GetComponentByClass<UStaticMeshComponent>();
	if (!IsValid(GateMesh))
	{
		UE_LOG(LogTemp, Fatal, TEXT("GateMesh in Gate not found!!!"));
	}

	if (!IsValid(GateOpenTimelineFloatCurve))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Gate Open Curve in Gate not found!!!"));
	}
	if (!IsValid(GateCloseTimelineFloatCurve))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Gate Close Curve in Gate not found!!!"));
	}

	UpdateFunctionFloat.BindDynamic(this, &AGate::UpdateTimelineComp);
	OnTimelineFinished.BindUObject(this, &AGate::CallMoveFinished);

	GateOpenTimelineComp->AddInterpFloat(GateOpenTimelineFloatCurve, UpdateFunctionFloat);
	GateOpenTimelineComp->SetTimelineFinishedFunc(OnTimelineFinished);

	GateCloseTimelineComp->AddInterpFloat(GateCloseTimelineFloatCurve, UpdateFunctionFloat);
	GateCloseTimelineComp->SetTimelineFinishedFunc(OnTimelineFinished);

	if (IsOpen)
	{
		GateCloseTimelineComp->PlayFromStart();
		GateCloseTimelineComp->Stop();
	}
}

void AGate::MoveGate()
{
	CameraTrigger->SwitchToCamera();

	if (IsOpen)
	{
		GateCloseTimelineComp->PlayFromStart();
		IsOpen = false;
		OnGateMoveBegin.Broadcast(false);
	}
	else
	{
		GateOpenTimelineComp->PlayFromStart();
		IsOpen = true;
		OnGateMoveBegin.Broadcast(true);
	}
}

void AGate::UpdateTimelineComp(float output)
{
	FVector newLocation{ 0.0f, 0.0f, output };
	GateMesh->SetRelativeLocation(newLocation);
}

void AGate::CallMoveFinished()
{
	OnGateMoveFinished.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, "Gate is done moving");
}
