#include "actiontake.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwmechanics/actorutil.hpp"

#include <components/compiler/locals.hpp>

#include "inventorystore.hpp"
#include "player.hpp"
#include "class.hpp"

#include "class.hpp"
#include "containerstore.hpp"

namespace MWWorld
{
    ActionTake::ActionTake (const MWWorld::Ptr& object) : Action (true, object) {}

    void ActionTake::executeImp (const Ptr& actor)
    {
        MWBase::Environment::get().getMechanicsManager()->itemTaken(
                    actor, getTarget(), MWWorld::Ptr(), getTarget().getRefData().getCount());
        MWWorld::Ptr newitem = *actor.getClass().getContainerStore (actor).add (getTarget(), getTarget().getRefData().getCount(), actor);
		MWBase::SmartEntityInstance * sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(getTarget().getBase());
		if (sei)
			sei->updatePtr(newitem);
        MWBase::Environment::get().getWorld()->deleteObject (getTarget());
        setTarget(newitem);
		
    }
	void ActionHold::executeImp(const Ptr & actor)
	{
		MWBase::Environment::get().getMechanicsManager()->itemTaken(
			actor, getTarget(), MWWorld::Ptr(), getTarget().getRefData().getCount());
		MWWorld::Ptr newitem = *actor.getClass().getContainerStore(actor).add(getTarget(), getTarget().getRefData().getCount(), actor);
		MWBase::SmartEntityInstance * sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(getTarget().getBase());
		if (sei)
			sei->updatePtr(newitem);
		MWBase::Environment::get().getWorld()->deleteObject(getTarget());
		setTarget(newitem);

		MWWorld::Ptr object = getTarget();
		MWWorld::InventoryStore& invStore = actor.getClass().getInventoryStore(actor);

		std::pair <int, std::string> result = object.getClass().canBeEquipped(object, actor);

		// display error message if the player tried to equip something
		if (!result.second.empty() && actor == MWMechanics::getPlayer())
			MWBase::Environment::get().getWindowManager()->messageBox(result.second);

		switch (result.first)
		{
		case 0:
			return;
		default:
			break;
		}

		// slots that this item can be equipped in
		std::pair<std::vector<int>, bool> slots_ = getTarget().getClass().getEquipmentSlots(getTarget());
		if (slots_.first.empty())
			return;

		// retrieve ContainerStoreIterator to the item
		MWWorld::ContainerStoreIterator it = invStore.begin();
		for (; it != invStore.end(); ++it)
		{
			if (*it == object)
			{
				break;
			}
		}

		if (it == invStore.end())
		{
			std::stringstream error;
			error << "ActionEquip can't find item " << object.getCellRef().getRefId();
			throw std::runtime_error(error.str());
		}

		// equip the item in the first free slot
		std::vector<int>::const_iterator slot = slots_.first.begin();
		for (; slot != slots_.first.end(); ++slot)
		{
			// if the item is equipped already, nothing to do
			if (invStore.getSlot(*slot) == it)
				return;

			if (invStore.getSlot(*slot) == invStore.end())
			{
				// slot is not occupied
				invStore.equip(*slot, it, actor);
				break;
			}
		}

		// all slots are occupied -> cycle
		// move all slots one towards begin(), then equip the item in the slot that is now free
		if (slot == slots_.first.end())
		{
			for (slot = slots_.first.begin(); slot != slots_.first.end(); ++slot)
			{
				invStore.unequipSlot(*slot, actor);
				if (slot + 1 != slots_.first.end())
					invStore.equip(*slot, invStore.getSlot(*(slot + 1)), actor);
				else
					invStore.equip(*slot, it, actor);
			}
		}
		//MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Nothing);
		MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Weapon);

	}



	ActionHold::ActionHold(const MWWorld::Ptr & object) : Action(true, object) {};
}
