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

	/**Fish::Fish(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp starttime) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mStartTime(starttime)
	{
	}

	Fish::Fish(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mOnCompleteTask(task)
	{
	}

	*/

	Fish::Fish()
	{
		std::cout << mNpcId + " wants to Fish" << std::endl;
	}

	Fish::Fish(MWWorld::Ptr dest) :
		mDestination(dest)
	{
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		mDone = false;
	}

	Fish::Fish(std::string destId, std::string npcId) :
		mDestId(destId)
	{
		mNpcId = npcId;
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		init();
		mReadyForUpdate = true;
		mDone = false;
	}

	Fish::~Fish()
	{
		MWBase::Environment::get().getTasksManager()->freeZoneSlot(mZoneId, mZoneSlotIdx);
	}

	void Fish::update()
	{
		/*bool currentlyActive = MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId);
		if (!currentlyActive)
			return;*/
		
		if (mStep == 0)
		{
			MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "rock", 0, 1);
			mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "fish");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
			//request a spot in the dance zone

			//make a journey to that quest
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
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
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			//markerPos.rot[2];
			MWBase::Environment::get().getWorld()->rotateObject(npcPtr, 0, 0, markerPos.rot[2]); //face direction of zoneslot
			if (MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId)) {
				if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(npcPtr, "fish"))
				{

					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "fish", 0, 1);
				}
			}
		}


		
	}




	int Fish::getTypeId() const
	{
		return TypeIDFish;
	}


	bool Fish::init()
	{
		return true;
	}
}

