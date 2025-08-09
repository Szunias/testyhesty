#include "TurnOrderItem.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

void UTurnOrderItem::SetImageColor(const FLinearColor& color)
{
	Item_Img->SetColorAndOpacity(color);
}

void UTurnOrderItem::SetImage(UTexture2D* image)
{
	Item_Img->SetBrushFromTexture(image);
	
}

void UTurnOrderItem::SetBoxSize(float size)
{
	UCanvasPanelSlot* canvasSlot{ UWidgetLayoutLibrary::SlotAsCanvasSlot(SizeBox) };
	canvasSlot->SetSize(FVector2D{ size, canvasSlot->GetSize().Y });
}

void UTurnOrderItem::SetImageSize(float size)
{
	FSlateBrush brush{ Item_Img->GetBrush() };
	brush.SetImageSize(FVector2D{ size, size });
	Item_Img->SetBrush(brush);
}
