#include "Hunt.hpp"

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
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/aiactivate.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "journey.hpp"
#include "get.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{

	/**Hunt::Hunt(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, MWWorld::TimeStamp starttime) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mStartTime(starttime)
	{
	}

	Hunt::Hunt(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination, std::string task) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0), mOnCompleteTask(task)
	{
	}

	*/

	Hunt::Hunt()
	{
	}

	Hunt::Hunt(MWWorld::Ptr dest) :
		mDestination(dest)
	{
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		mDone = false;
	}

	Hunt::Hunt(std::string destId, std::string npcId) :
		mDestId(destId)
	{
		mNpcId = npcId;
		mStep = 0;
		mStartTime = MWBase::Environment::get().getWorld()->getTimeStamp();
		init();
		mReadyForUpdate = true;
		mDone = false;
	}

	void Hunt::update()
	{
		if (MWBase::Environment::get().getWorld()->getTimeStamp() > mStartTime + 9)
		{
			std::cout << "time to end hunt" << std::endl;
			
			mDone = true;
		}
		else if (mStep == 0)
		{
			std::cout << "getting bow..." << std::endl;
			mSubTask = new MWTasks::Get("nadia bow", mNpcId);
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
			std::cout << "first journey..." << std::endl;
			mSubTask = new MWTasks::Journey(mDestId, mNpcId);
			mStep += 1;
		}
		else if (mStep >= 3)
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				std::cout << "looping journey" << std::endl;
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
				if (mStep % 2 == 0)
				{
					mSubTask = new MWTasks::Journey("tnode3", mNpcId);
				}
				else
				{
					mSubTask = new MWTasks::Journey(mDestId, mNpcId);
				}
			}
		}
	


	}



	int Hunt::getTypeId() const
	{
		return TypeIDHunt;
	}


	bool Hunt::init()
	{
		return true;
	}
}

