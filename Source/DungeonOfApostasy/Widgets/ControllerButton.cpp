
#include "ControllerButton.h"
#include "Framework/Application/SlateApplication.h"

void UControllerButton::HoverButton()
{
    auto cachedWidget{ GetCachedWidget() };
    if (cachedWidget.IsValid())
    {
        cachedWidget->OnMouseEnter(GetCachedGeometry(), FPointerEvent{});
    }
}

void UControllerButton::UnhoverButton()
{
    auto cachedWidget{ GetCachedWidget() };
    if (cachedWidget.IsValid())
    {
        cachedWidget->OnMouseLeave(FPointerEvent{});
    }
}

void UControllerButton::ClickButton()
{
    OnClicked.Broadcast();
}

void UControllerButton::ButtonDown()
{
    //Get our slate application
    FSlateApplication& slateApp = FSlateApplication::Get();

    //create a pointer event
    FPointerEvent mouseDownEvent{
        0,
        slateApp.CursorPointerIndex,
        slateApp.GetCursorPos(),
        slateApp.GetLastCursorPos(),
        slateApp.GetPressedMouseButtons(),
        EKeys::LeftMouseButton,
        0,
        slateApp.GetPlatformApplication()->GetModifierKeys()
    };

    //send the mouse event to the slate handler
    TSharedPtr<FGenericWindow> genWindow;
    slateApp.ProcessMouseButtonDownEvent(genWindow, mouseDownEvent);
}

void UControllerButton::ButtonUp()
{
    //Get our slate application
    FSlateApplication& slateApp = FSlateApplication::Get();

    //create a pointer event
    FPointerEvent mouseUpEvent{
         0,
         slateApp.CursorPointerIndex,
         slateApp.GetCursorPos(),
         slateApp.GetLastCursorPos(),
         slateApp.GetPressedMouseButtons(),
         EKeys::LeftMouseButton,
         0,
         slateApp.GetPlatformApplication()->GetModifierKeys()
    };

   

    //send the mouse event to the slate handler
    slateApp.ProcessMouseButtonUpEvent(mouseUpEvent);
}
