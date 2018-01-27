#include "sleep.hpp"

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
#include "../mwbase/aischedulemanager.hpp"

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

	Sleep::Sleep(MWTasks::Task* lifetask) 
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask -> mNpcId;
		mDestId = MWBase::Environment::get().getAIScheduleManager()->getBed(mNpcId);
		mStep = 0;
		mDone = false;
		mNpcPtr = mLifeTask->mNpcPtr;
	}

	MWWorld::Ptr Sleep::update()
	{
		if (mStep == 0)
		{
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			mSubTask = new MWTasks::Journey(mDestId, mLifeTask);
			mStep += 1;
		}
		if (mStep == 1)
		{
			mNpcPtr = mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
			}
		}
		if (mStep == 2)
		{
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			if (MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcPtr)) 
				if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(mNpcPtr, "lay"))
					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(mNpcPtr, "lay", 0, 1);
			
		}
		return mNpcPtr;
	}




	int Sleep::getTypeId() const
	{
		return TypeIDSleep;
	}

}

