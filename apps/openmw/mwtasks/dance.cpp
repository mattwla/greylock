#include "dance.hpp"

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
#include "../mwbase/lifemanager.hpp"
#include "../mwtasks/journey.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{
	Dance::Dance(MWTasks::Task* lifetask)
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask -> mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mStep = 0;
		mDone = false;
	}

	Dance::~Dance()
	{
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);

		std::cout << "deleting dance subtask" << std::endl;
		if (mSubTask)
		{
			delete mSubTask;
		}
		
		if (MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(mNpcPtr, "rock"))
		{
			MWBase::Environment::get().getMechanicsManager()->skipAnimation(mNpcPtr);
			MWBase::Environment::get().getMechanicsManager()->forceStateUpdate(mNpcPtr);
		}

		if (mZoneSlotIdx != -1)
			MWBase::Environment::get().getTasksManager()->freeZoneSlot(mZoneId, mZoneSlotIdx);
	}

	MWWorld::Ptr Dance::update()
	{
		if (mStep == 0)
		{
			//find a place to dance
			//get dance zone
			mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "dance");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
				//request a spot in the dance zone
			if (mZoneSlotIdx == -1) //MWX FIX ME, hack to stop crashing when all zoneslots are full.
			{
				mDone = true;
				return mNpcPtr;
			}
			//make a journey to that quest
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
			std::cout << mNpcId + "wants to dance" << std::endl;
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "rock", 0, 1);
			mSubTask = new MWTasks::Journey(mDestId, mLifeTask);

			//instead, find a d
			mStep += 1;
		}
		else if (mStep == 1)
		{
			mNpcPtr = mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
			}
		}
		else if (mStep == 2)
		{
			auto seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence();
			
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			
			if (seq.getTypeId() != 5) //if not in combat, do this task.
			{
				seq.clear();
				if (MWBase::Environment::get().getLifeManager()->inActiveRange(mNpcPtr))
					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(mNpcPtr, "rock", 0, 1);
			}
			
		}


		return mNpcPtr;
	}


	int Dance::getTypeId() const
	{
		return TypeIDDance;
	}

}

