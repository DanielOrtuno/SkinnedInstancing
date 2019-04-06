#pragma once
#include "Component.h"

class CInventory :
	public IComponent
{
	int m_nPassive, m_nActive, m_nCurrConsume;
	int* m_pnConsumables;
public:
	CInventory(IEntity* pcOwner);

	/***************
	* Accessors
	***************/

	// Get the current passive item
	int		GetPassive();

	// Get the current active item
	int		GetActive();

	// Sets a pointer to the array of consumables
	// nSet: pointer address to set the array to
	void	GetConsumables(int** nSet);

	// Get the current consumable item
	int		GetCurrentConsumable();

	/***************
	* Mutators
	***************/

	// Add a consumable item to the count
	// If the item does not exist, this does nothing
	// If the current consumable is none, it swaps to that item
	void	AddConsumableItem(int nItem);

	// Uses the current consumable item
	// Returns what item to be used
	// If none can be used, returns 0
	int		UseCurrentConsumable();

	// Set the consumable item to be used
	// when UseCurrentConsumable is called
	// If none can be set, defaults to no item
	// nItem: Consumable item to set to
	void	SetCurrentConsumable(int nItem);

	// Sets what passive item is equipped
	// If the item does not exist, defaults to none
	// nPassive: Passive item to set to
	void	SetPassive(int nPassive);

	// Sets what active item is equipped
	// If the item does not exist, defaults to none
	// nActive: Active item to set to
	void	SetActive(int nActive);

	CInventory & operator=(CInventory& cCopy);

	~CInventory();
};

