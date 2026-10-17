#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Input/UIActionBindingHandle.h"
#include "UIActivatableWidget.generated.h"

/** Own enum instead of ECommonInputMode: adds "no opinion" and maps each mode to a full config (mouse capture, cursor) */
UENUM(BlueprintType)
enum class EUIInputMode : uint8
{
	Default UMETA(DisplayName = "Default (Inherit)"),
	Game,
	Menu,
	GameAndMenu UMETA(DisplayName = "Game + Menu"),
};

/**
 *  Base for every widget pushed through the UI manager. Each class declares the input mode
 *  that applies while it is the topmost active widget.
 */
UCLASS(Abstract, Blueprintable)
class ROUGELIKE_API UUIActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	/**
	 *  Input mode applied while this widget is the topmost active widget.
	 *  Default has no opinion and defers to the parent widget. If nothing in the tree has one,
	 *  CommonUI applies its own default config, which is Menu and blocks game input.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	EUIInputMode InputMode = EUIInputMode::Default;
};
