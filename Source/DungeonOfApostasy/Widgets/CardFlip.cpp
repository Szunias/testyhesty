
#include "CardFlip.h"
#include "Components/Image.h"

void UCardFlip::ForceStartFlip()
{
	OnStartCardFlip.Broadcast();
}
