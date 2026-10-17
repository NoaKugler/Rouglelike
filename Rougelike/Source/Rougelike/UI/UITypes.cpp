#include "UI/UITypes.h"

DEFINE_LOG_CATEGORY(LogUI);

namespace UITags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Layer_Game, "UI.Layer.Game", "HUD and in-game overlays. Lowest layer.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Layer_Menu, "UI.Layer.Menu", "Full-screen menus and sub-menus.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Layer_Modal, "UI.Layer.Modal", "Dialogs and popups above menus.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Layer_Loading, "UI.Layer.Loading", "Loading screens. Topmost layer.");
}
