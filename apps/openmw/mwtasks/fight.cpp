#include "fight.hpp"

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
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/statusmanager.hpp"

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
	Fight::Fight()
	{
		mStep = 0;
		mDone = false;
	}

	Fight::Fight(MWTasks::Task* lifetask, MWWorld::Ptr target)
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask->mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mStep = 0;
		mDone = false;
		mTarget = target;
	}

	Fight::~Fight()
	{
	
		if (mSubTask)
		{
			delete mSubTask;
		}

	}

	MWWorld::Ptr Fight::update()
	{

		//Check if target is within reach, if so either start fight or continue fight
		//If not within reach, either start journey or continue journey

		if (mStep == 0)
		{
			MWBase::Environment::get().getMechanicsManager()->startCombat(mNpcPtr, mTarget);
			
			/*mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "guard");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
			std::cout << mNpcId + "wants to sitground" << std::endl;
			mSubTask = new MWTasks::Journey(mDestId, mLifeTask);
			mStep += 1;*/
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
		
		}


		return mNpcPtr;
	}

	int Fight::getTypeId() const
	{
		return TypeIDFight;
	}



}

