#include "UI_TipInterface.h"
#include "MobyGame/Table/SlotAttribute.h"
#include "MobyGame/Table/SlotTable.h"
#include "../../Tip/UI_Tip.h"
#include "MobyGame/MobyGamePlayerState.h"

IUI_TipInterface::IUI_TipInterface()
{

}

UWidget* IUI_TipInterface::VerifyTip(AMobyGamePlayerState* InPlayerState, UUI_Tip* InTip, int32 InSlotID, const FSlotData* ClientSlotData)
{
	if (InPlayerState && InTip && InSlotID != INDEX_NONE)
	{
		if (!ClientSlotData)
		{
			ClientSlotData = InPlayerState->GetSlotData(InSlotID);
		}

		if (ClientSlotData)
		{
			if (ClientSlotData->SlotID != INDEX_NONE)
			{
				if (const FSlotTable* InSlotTable = InPlayerState->GetSlotTableTemplate(ClientSlotData->SlotID))
				{
					if (const FSlotAttribute* InSlotAttribute = InPlayerState->GetSlotAttributeTemplate(InSlotTable->ID))
					{
						//¸³Öµ½éÉÜ
						InTip->SetDescribeText(InSlotTable->SlotIntroduction);

						MethodUnit::TipRegistration(InTip, InSlotAttribute);
					}
				}

				return InTip;
			}
		}
	}

	return NULL;
}

