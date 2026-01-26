#pragma once

UENUM(BlueprintType)
enum class EBlasterCombatState : uint8
{
	Unoccupied UMETA(DisplayName="Unoccupied"),
	Reloading UMETA(DisplayName="Reloading"),
	
	MAX UMETA(DisplayName="DefaultMAX")
};