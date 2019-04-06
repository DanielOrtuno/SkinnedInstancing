#pragma once
#include "Inventory.h"
#include "EnumTypes.h"



CInventory::CInventory(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::INVENTORY;
	m_nActive = m_nPassive = m_nCurrConsume = 0;
	m_pnConsumables = new int[eConsumableItems::COUNT];
	for (int i = 0; i < eConsumableItems::COUNT; i++)
	{
		m_pnConsumables[i] = 0;
	}
}

int CInventory::GetPassive()
{
	return m_nPassive;
}

int CInventory::GetActive()
{
	return m_nActive;
}

void CInventory::GetConsumables(int** nSet)
{
	*nSet = m_pnConsumables;
}

int CInventory::GetCurrentConsumable()
{
	return m_nCurrConsume;
}

void CInventory::AddConsumableItem(int nItem)
{
	if (nItem > eConsumableItems::NONE && nItem < eConsumableItems::COUNT)
	{
		m_pnConsumables[nItem]++;
		if (m_nCurrConsume == eConsumableItems::NONE)
		{
			SetCurrentConsumable(nItem);
		}
	}
}

int CInventory::UseCurrentConsumable()
{
	if (m_nCurrConsume > eConsumableItems::NONE && m_nCurrConsume < eConsumableItems::COUNT)
	{
		if (m_pnConsumables[m_nCurrConsume] > 0)
		{
			int nReturn = m_nCurrConsume;
			m_pnConsumables[m_nCurrConsume]--;
			if (m_pnConsumables[m_nCurrConsume] == 0)
			{
				// Do something so that the player doesn't do something stupid
				int nCurrent = m_nCurrConsume;
				bool bCheck = true;
				for (int i = nCurrent + 1; i < eConsumableItems::COUNT; i++)
				{
					if (m_pnConsumables[i] > 0)
					{
						SetCurrentConsumable(i);
						bCheck = false;
						break;
					}
				}
				if (bCheck)
				{
					for (int i = eConsumableItems::NONE + 1; i < nCurrent; i++)
					{
						if (m_pnConsumables[i] > 0)
						{
							SetCurrentConsumable(i);
							bCheck = false;
							break;
						}
					}
					if (bCheck)
					{
						SetCurrentConsumable(eConsumableItems::NONE);
					}
				}
			}
			return nReturn;
		}
	}
	return 0;
}

void CInventory::SetCurrentConsumable(int nItem)
{
	// Make sure it is legal for arrays
	if (nItem < eConsumableItems::COUNT && nItem > -1)
	{
		// See if there are items in the place we are switching to
		if (m_pnConsumables[nItem] > 0)
		{
			m_nCurrConsume = nItem;
		}
		// Make sure the program didn't detect this issue sooner
		else if (nItem == eConsumableItems::NONE)
		{
			m_nCurrConsume = nItem;
		}
		// Go to the next available item slot
		else
		{
			int nCurrent = nItem;
			bool bCheck = true;
			for (int i = nCurrent + 1; i < eConsumableItems::COUNT; i++)
			{
				if (m_pnConsumables[i] > 0)
				{
					m_nCurrConsume = i;
					bCheck = false;
					break;
				}
			}
			if (bCheck)
			{
				for (int i = eConsumableItems::NONE + 1; i < nCurrent; i++)
				{
					if (m_pnConsumables[i] > 0)
					{
						m_nCurrConsume = i;
						bCheck = false;
						break;
					}
				}
				if (bCheck)
				{
					m_nCurrConsume = 0;
				}
			}
		}
	}
}

void CInventory::SetPassive(int nPassive)
{
	m_nPassive = nPassive;
}

void CInventory::SetActive(int nActive)
{
	m_nActive = nActive;
}

CInventory & CInventory::operator=(CInventory& cCopy)
{
	//Be safe and get rid of this
	if (m_pnConsumables)	delete m_pnConsumables;
	cCopy.GetConsumables(&m_pnConsumables);
	m_nPassive = cCopy.GetPassive();
	m_nActive = cCopy.GetActive();
	m_nCurrConsume = cCopy.GetCurrentConsumable();
	return *this;
}

CInventory::~CInventory()
{
	if (m_pnConsumables)	delete m_pnConsumables;
	m_pnConsumables = nullptr;
}
