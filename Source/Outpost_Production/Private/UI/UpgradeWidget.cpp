#include "UpgradeWidget.h"
#include "PlayerMech.h"
#include "AbilitySystemComponent.h"
#include "BaseWeapon.h"

void UUpgradeWidget::ActivateUpgrade(APlayerMech* player)
{
	// Check to make sure the upgrade was already used.
	if (m_HasBeenUsed == true) return;

	

	// Check if player is valid.
	if (!player)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUpgradeWidget : ActivateUpgrade : player not valid"));
		return;
	}

	bool canUpgrade = false;
	// If this upgrade has prerequisites
	if (m_UpgradeData.m_Prerequisites.Num() > 0)
	{
		//Cycle through all prereqs
		for (int i = 0; i < m_UpgradeData.m_Prerequisites.Num(); i++)
		{
			// If the player has one of the prerequisite upgrades, we canUpgrade0
			if (player->m_PurchasedUpgrades.Contains(m_UpgradeData.m_Prerequisites[i]))
			{
				canUpgrade = true;
			}
		}
	}
	//Otherwise we canUpgrade for sure
	else
	{
		canUpgrade = true;
	}

	if (canUpgrade)
	{
		player->Server_Upgrade(m_UpgradeData.m_AppliedEffect, m_UpgradeData.m_UpgradeCost);
		player->m_PurchasedUpgrades.Add(m_UpgradeData.m_UpgradeName);
		m_HasBeenUsed = true;
		UpdateWidgetColor();
	}		
}
