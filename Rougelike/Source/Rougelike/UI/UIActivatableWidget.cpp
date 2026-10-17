#include "UI/UIActivatableWidget.h"
#include "CommonInputModeTypes.h"
#include "Engine/EngineBaseTypes.h"

TOptional<FUIInputConfig> UUIActivatableWidget::GetDesiredInputConfig() const
{
	// The cursor is the aim device, so no mode hides it. Game mode only captures while a button is held:
	// permanent capture makes CommonUI switch to raw mouse input, where the OS cursor position isn't reliable.
	switch (InputMode)
	{
	case EUIInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CaptureDuringMouseDown, false);
	case EUIInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);
	case EUIInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture, false);
	default:
		// Keeps a Blueprint "Get Desired Input Config" override working
		return Super::GetDesiredInputConfig();
	}
}
