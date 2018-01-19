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
	
	Get::Get(std::string destId, std::string npcId) :
		mDestId(destId)
	{
		mNpcId = npcId;
		mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		mStep = 0;
		mDone = false;
	}

	void Get::update()
	{
		if (mStep == 0)
		{
			mSubTask = new MWTasks::Journey(mDestId, mNpcId);
			mStep += 1;
		}
		else if (mStep == 1)
		{
			mSubTask->update();
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


		
	}

	bool Get::pickupItem()
	{
		MWWorld::Ptr itemPtr = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWMechanics::AiSequence& seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence();
		bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId);
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

