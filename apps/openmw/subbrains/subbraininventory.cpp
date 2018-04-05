#include "subbraininventory.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"


MWBase::SubBrainInventory::SubBrainInventory(MWBase::Life * life)
{
	mOwnerLife = life;
}

void MWBase::SubBrainInventory::calculate(MWBase::Awareness * awareness)
{
	//mOwnerLife->mPtr.getClass()
	//Run through all items in npc inventory, fill a basestatus with those items and what they offer.
	MWWorld::InventoryStore store = mOwnerLife->mPtr.getClass().getInventoryStore(mOwnerLife->mPtr);
	
	//get smart entities in store.
	
}

std::string MWBase::SubBrainInventory::getID()
{
	return std::string();
}
