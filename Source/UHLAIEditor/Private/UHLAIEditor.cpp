// Pavel Penkov 2025 All Rights Reserved.

#include "UHLAIEditor.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName UHLAIEditorTabName("UHLAIEditor");

#define LOCTEXT_NAMESPACE "FUHLAIEditorModule"

void FUHLAIEditorModule::StartupModule()
{
}

void FUHLAIEditorModule::ShutdownModule()
{
}


#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FUHLAIEditorModule, UHLAIEditor)
