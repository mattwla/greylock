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
#include "journey.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{

	/**Dance::Dance(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp starttime) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mStartTime(starttime)
	{
	}

	Dance::Dance(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mOnCompleteTask(task)
	{
	}

	*/

	Dance::Dance()
	{
	}

	Dance::Dance(MWWorld::Ptr dest) :
		mDestination(dest)
	{
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		mDone = false;
	}

	Dance::Dance(std::string destId, std::string npcId) :
		mDestId(destId)
	{
		mNpcId = npcId;
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		init();
		mReadyForUpdate = true;
		mDone = false;
	}

	void Dance::update()
	{
		if (mStep == 0)
		{
			std::cout << "nadia wants to dance" << std::endl;
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "rock", 0, 1);
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
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "rock", 0, 1);
		}


		
	}

	bool Dance::pickupItem()
	{

		MWWorld::Ptr itemPtr = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
		MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		MWMechanics::AiSequence& seq = npcPtr.getClass().getCreatureStats(npcPtr).getAiSequence();

		bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId);
		if (currentlyActive)
		{
			seq.stack(MWMechanics::AiActivate(mDestId), npcPtr);
			std::cout << "activated" << std::endl;
		}
		else
		{
			MWBase::Environment::get().getWorld()->activate(itemPtr, npcPtr);
		}
		//npcPtr.getClass().activate(itemPtr, npcPtr);
		return true;
	}


	int Dance::getTypeId() const
	{
		return TypeIDDance;
	}


	bool Dance::init()
	{
		return true;
	}
}

