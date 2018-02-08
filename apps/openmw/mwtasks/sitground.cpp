#include "Sitground.hpp"

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

	/**Sitground::Sitground(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp starttime) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mStartTime(starttime)
	{
	}

	Sitground::Sitground(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mOnCompleteTask(task)
	{
	}

	*/

	
	Sitground::Sitground() 
	{
		mStep = 0;
		mDone = false;
	}

	Sitground::Sitground(MWTasks::Task* lifetask)
	
	{
		mLifeTask = lifetask;
		mNpcId = mLifeTask->mNpcId;
		mNpcPtr = mLifeTask->mNpcPtr;
		mStep = 0;
		mDone = false;
	}

	Sitground::~Sitground()
	{
		//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);

		std::cout << "deleting Sitground subtask" << std::endl;
		if (mSubTask)
		{
			delete mSubTask;
		}
		
		if (MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(mNpcPtr, "sitground"))
		{
			MWBase::Environment::get().getMechanicsManager()->skipAnimation(mNpcPtr);
			MWBase::Environment::get().getMechanicsManager()->forceStateUpdate(mNpcPtr);
		}


		MWBase::Environment::get().getTasksManager()->freeZoneSlot(mZoneId, mZoneSlotIdx);
	}

	MWWorld::Ptr Sitground::update()
	{
		if (mStep == 0)
		{
			//find a place to Sitground
			//get Sitground zone
			mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "sitground");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
				//request a spot in the Sitground zone

			//make a journey to that quest
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
			std::cout << mNpcId + "wants to sitground" << std::endl;
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
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			//markerPos.rot[2];
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			MWBase::Environment::get().getWorld()->rotateObject(mNpcPtr, 0, 0, markerPos.rot[2]); //face direction of zoneslot
			
			auto seq = mNpcPtr.getClass().getCreatureStats(mNpcPtr).getAiSequence();

	
			if (seq.getTypeId() != 5) //if not in combat, do this task.
			{
				seq.clear();
				if (MWBase::Environment::get().getLifeManager()->inActiveRange(mNpcPtr))
					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(mNpcPtr, "sitground", 0, 1);
			}
		}


		return mNpcPtr;
	}

	int Sitground::getTypeId() const
	{
		return TypeIDSitground;
	}


	
}

