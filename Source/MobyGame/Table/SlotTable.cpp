#include "SlotTable.h"

FSlotTable::FSlotTable() : FMobyGameTableBase() , SlotGold(10)
{
	SlotType.Add(ESlotType::SLOT_ALL);
}

