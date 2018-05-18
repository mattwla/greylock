#include "actiontake.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"

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
		MWBase::SmartEntityInstance * sei = MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(getTarget().getCellRef().getRefId(), getTarget().getCellRef().getRefNum());
		sei->updatePtr(newitem);
        MWBase::Environment::get().getWorld()->deleteObject (getTarget());
        setTarget(newitem);
    }
}
