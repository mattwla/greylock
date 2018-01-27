#include "life.hpp"


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
#include "../mwbase/tasksmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <string>

namespace MWTasks
{

	

	Life::Life(std::string npcId)
	{
		mNpcId = npcId;
		mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		mTickCounter = 0;
		mSchedule = new MWBase::AIScheduleManager::Schedule(npcId);
	}

	int Life::getTypeId() const
	{
		return TypeIDLife;
	}

	MWWorld::Ptr Life::update()
	{
		//MWX fix me no logic yet really for not having a scheduled task.

	
		mTickCounter += 1;
		if (!mSubTask)
		{
			MWTasks::Task::TypeID task = mSchedule->getScheduledTask();
			if (task == MWTasks::Task::TypeID::TypeIDGet)
				return mNpcPtr;
			mSubTask = MWBase::Environment::get().getTasksManager()->getScheduledTask(this, task);
			//if (mSubTask && mSubTask->getTypeId() == TypeIDJourney)
			//{
			//	mSubTask->mNpcId = mNpcId;

			//}

		}
		else if (mTickCounter > 5) 	//check for scheduled task every 5 minutes or if there is no mSubTask
		{
			//std::cout << "checking schedule" << std::endl;;
			mTickCounter = 0;
			MWTasks::Task::TypeID task = mSchedule->getScheduledTask();
			auto taskholder = MWBase::Environment::get().getTasksManager()->getScheduledTask(this, task);
			if (taskholder && mSubTask->getTypeId() != taskholder->getTypeId())
			{
				delete mSubTask; //also delete sub task of sub task of subtask of etc... mwx fix me
				mSubTask = taskholder;
			}
		}
		else
		{
			mNpcPtr = mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
			}
		}
		return mNpcPtr;

	}

	std::string Life::getZoneId(std::string task)
	{
		return "";
	}

	

}