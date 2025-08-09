#include "RadialButton.h"
#include "Styling/SlateBrush.h"
#include "Styling/WidgetStyle.h"

void URadialButton::Initialize()
{
    OnClicked.AddDynamic(this, &URadialButton::OnButtonClicked);
    OnHovered.AddDynamic(this, &URadialButton::OnButtonHover);
    OnUnhovered.AddDynamic(this, &URadialButton::OnButtonUnhover);
}

void URadialButton::OnHover(bool notify)
{
    OnRadialHover.Broadcast(this);
    auto cachedWidget{ GetCachedWidget() };
    if (cachedWidget.IsValid())
    {
        cachedWidget->OnMouseEnter(GetCachedGeometry(), FPointerEvent{});
    }    
    SetFocus();
    SetKeyboardFocus();
}

void URadialButton::OnUnhover(bool notify)
{
    auto cachedWidget{ GetCachedWidget() };
    if (cachedWidget.IsValid())
    {
        cachedWidget->OnMouseLeave(FPointerEvent{});
    }
}

void URadialButton::OnPressed()
{   
    auto cachedWidget{ GetCachedWidget() };
    if (cachedWidget.IsValid())
    {       
        cachedWidget->OnMouseButtonDown(GetCachedGeometry(), FPointerEvent{});
        cachedWidget->OnMouseButtonUp(GetCachedGeometry(), FPointerEvent{});
    }
    
}

void URadialButton::OnButtonClicked()
{
    OnRadialClicked.Broadcast(this);
    //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, GetName() + " click Detected");
}

void URadialButton::OnButtonHover()
{
    OnRadialHover.Broadcast(this);
    //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, GetName() + " hover Detected");
}

void URadialButton::OnButtonUnhover()
{
    //GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, GetName() + " unhover Detected");
}

void URadialButton::OnItemClicked()
{
    OnClicked.Broadcast();
}

void URadialButton::SetButtonIdx(int idx)
{
    ButtonIdx = idx;
}

int URadialButton::GetButtonIdx() const
{
    return ButtonIdx;
}

void URadialButton::SetDefaultStyles(const FSlateBrush& normal, const FSlateBrush& hovered, const FSlateBrush& pressed)
{
    NormalStyle = normal;
    HoverStyle = hovered;
    PressedStyle = pressed;

    ButtonStyle = GetStyle();
    ButtonStyle.SetNormal(NormalStyle);
    ButtonStyle.SetHovered(HoverStyle);
    ButtonStyle.SetPressed(PressedStyle);
    SetStyle(ButtonStyle);
}
