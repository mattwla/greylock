#include "fish.hpp"

#include <limits.h>
#include <iostream>
#include <fstream>

#include <components/misc/rng.hpp>

#include <components/esm/esmwriter.hpp>
#include <components/esm/stolenitems.hpp>

#include <components/sceneutil/positionattitudetransform.hpp>

#include "../mwworld/esmstore.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/aiactivate.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "journey.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{
	
	Fish::Fish(std::string npcId)
	{
		mNpcId = npcId;
		mStep = 0;
		mDone = false;
	}

	Fish::~Fish()
	{
		MWBase::Environment::get().getTasksManager()->freeZoneSlot(mZoneId, mZoneSlotIdx);
		MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false); //mwx fix me... how many darn times am I doing this. How do I just capture it as a member variable? For life? And could be called on?? Oh man big.
		std::cout << "deleting fish subtask" << std::endl;
		if (mSubTask)
		{
			delete mSubTask;
		}
		if (MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(npcPtr, "fish"))
		{
			MWBase::Environment::get().getMechanicsManager()->skipAnimation(npcPtr);
			MWBase::Environment::get().getMechanicsManager()->forceStateUpdate(npcPtr);
		}
	}

	void Fish::update()
	{
		if (mStep == 0)
		{
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "fish");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
			mSubTask = new MWTasks::Journey(mDestId, mNpcId);
			mStep += 1;
		}
		if (mStep == 1)
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
			}
		}
		if (mStep == 2)
		{
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			MWBase::Environment::get().getWorld()->rotateObject(npcPtr, 0, 0, markerPos.rot[2]); //face direction of zoneslot
			if (MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId)) 
				if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(npcPtr, "fish"))
					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "fish", 0, 1);
		}
}

	int Fish::getTypeId() const
	{
		return TypeIDFish;
	}

}

