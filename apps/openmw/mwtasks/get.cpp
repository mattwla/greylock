#include "Get.hpp"

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
	
	Get::Get(std::string destId, MWTasks::Task* lifetask) :
		mDestId(destId)
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask->mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mStep = 0;
		mDone = false;
	}

	MWWorld::Ptr Get::update()
	{
		if (mStep == 0)
		{
			mSubTask = new MWTasks::Journey(mDestId, mLifeTask);
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
			if(pickupItem())
			{
				mDone = true;
			}
		}

		return mNpcPtr;
		
	}

	bool Get::pickupItem()
	{
		MWWorld::Ptr itemPtr = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWMechanics::AiSequence& seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence();
		bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcPtr);
		if (currentlyActive)
		{
			seq.stack(MWMechanics::AiActivate(mDestId), mNpcPtr);
			std::cout << "activated" << std::endl;
		}
		else
		{
			MWBase::Environment::get().getWorld()->activate(itemPtr, mNpcPtr);
		}
		return true; //mwx fix me... what if we didn't get item?
	}


	int Get::getTypeId() const
	{
		return TypeIDGet;
	}

}

