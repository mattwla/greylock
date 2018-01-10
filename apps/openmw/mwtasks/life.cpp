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
		mTickCounter = 0;
		mSchedule = new MWBase::AIScheduleManager::Schedule(npcId);
	}

	int Life::getTypeId() const
	{
		return TypeIDLife;
	}

	void Life::update()
	{
		//MWX fix me no logic yet really for not having a scheduled task.

	
		mTickCounter += 1;
		if (!mSubTask)
		{
			std::string task = mSchedule->getScheduledTask();
			mSubTask = MWBase::Environment::get().getTasksManager()->getScheduledTask(mNpcId, task);
			//if (mSubTask && mSubTask->getTypeId() == TypeIDJourney)
			//{
			//	mSubTask->mNpcId = mNpcId;

			//}

		}
		else if (mTickCounter > 5) 	//check for scheduled task every 5 minutes or if there is no mSubTask
		{
			//std::cout << "checking schedule" << std::endl;;
			mTickCounter = 0;
			std::string task = mSchedule->getScheduledTask();
			
			auto taskholder = MWBase::Environment::get().getTasksManager()->getScheduledTask(mNpcId, task);
			if (taskholder && mSubTask->getTypeId() != taskholder->getTypeId())
			{
				delete mSubTask; //also delete sub task of sub task of subtask of etc... mwx fix me
				mSubTask = taskholder;
			}
		}
		else
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
			}
		}

	}



}