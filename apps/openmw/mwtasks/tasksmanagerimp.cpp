#include "tasksmanagerimp.hpp"

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
#include "../mwbase/aischedulemanager.hpp"
#include "../mwbase/environment.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwtasks/task.hpp"
#include "../mwtasks/life.hpp"
#include "../mwtasks/journey.hpp"
#include "../mwtasks/get.hpp"
#include "../mwtasks/hunt.hpp"
#include "../mwtasks/sleep.hpp"
#include "../mwtasks/dance.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <map>


namespace MWTasks
{
	std::map<std::string, MWTasks::Task*> TasksManager::buildNpcMap()
	{
		
		std::map<std::string, MWTasks::Task*> npcmap;
		
		std::string list = "schedules/npclist.csv";

		std::ifstream in(list.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << list << std::endl;

		std::string npc;

		while (getline(in, npc))
		{
			MWTasks::Task* newlife = new MWTasks::Life(npc);
			
			npcmap[npc] = newlife;
			//CREATE AND ASSIGN NEW LIFE TASK HERE MWX
		}

		

		return npcmap;
	}


	TasksManager::TasksManager() :
		mLastTimeReported(0.0f)
		, mTimePassed(0.0f)
		, mTimeAccumulator(0.0f)
	{
		mNpcMap = buildNpcMap();
	}

	void TasksManager::update(float hours, bool incremental)
	{
		int ticks = 0;
		//std::cout << "updating task manager" << std::endl;
		float min = 1.0 / 60.0; //define a minute
		
		if (mLastTimeReported == 0.0f) //our first update of current session, just log the time.
		{
			mLastTimeReported = hours;
			return;
		}
		mTimePassed = hours - mLastTimeReported;
		mLastTimeReported = hours;
		mTimeAccumulator += mTimePassed;
		//std::cout << mTimeAccumulator<< std::endl;
		if (mTimeAccumulator > min)
		{
			ticks = mTimeAccumulator / min; //How many minutes have passed?
			mTimeAccumulator -= ticks * min;
			//std::cout << "minutes passed: " << ticks << std::endl;
		}
		
		

		//process hours into 'ticks', for each tick poke our tasks. Tasks never have to worry about getting more than one tick in an update, ticks always delivered one at a time. Always = to 1 minute of activity.
		
		while (ticks > 0)
		{
			for (auto& sm_pair : mNpcMap)
			{
				sm_pair.second->update();
				std::cout << isInActiveRange(sm_pair.first) << std::endl;
				//Make a seperate vector just for updating?.... not a horrible idea.
			}
			ticks -= 1;
		}

		//thanks JLBorges at cplusplus.com
	}

	Task * TasksManager::getScheduledTask(std::string npcId)
	{
		std::string stask = MWBase::Environment::get().getAIScheduleManager()->fetchCurrentScheduledTask(npcId);

		if (stask == "journey")
		{
			std::string destID = "tnode1";
			//MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("tnode4", false);
			MWTasks::Journey * rtask = new MWTasks::Journey(destID, npcId); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
			
			//JOURNEY
		}
		if (stask == "hunt")
		{

			std::string destId = "tnode4";
			//MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("tnode4", false);
			MWTasks::Hunt * rtask = new MWTasks::Hunt(destId, npcId); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
		}
		if (stask == "sleep")
		{
			
			std::string destId = MWBase::Environment::get().getAIScheduleManager()->getBed(npcId);
			std::cout << npcId + " wants to sleep." << std::endl;
			MWTasks::Sleep * rtask = new MWTasks::Sleep(destId, npcId);
			return rtask;
		}
		if (stask == "dance")
		{
			std::string destId = "tnode1";
			MWTasks::Dance * rtask = new MWTasks::Dance(destId, npcId);
			return rtask;
		}


		
		return nullptr;
	}


	bool TasksManager::isInActiveRange(std::string npcId)
	{
		//mwx fix me some bad redundency here against actors.cpp
		const float aiProcessingDistance = 7168;
	
		const float sqrAiProcessingDistance = aiProcessingDistance*aiProcessingDistance;
		MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
		MWWorld::Ptr npc = MWBase::Environment::get().getWorld()->searchPtr(npcId, false);
		
		bool inProcessingRange = (player.getRefData().getPosition().asVec3() - npc.getRefData().getPosition().asVec3()).length2() <= sqrAiProcessingDistance;

		
		//If player is resting, no one is in active range....
		if(inProcessingRange)
			inProcessingRange = !MWBase::Environment::get().getWindowManager()->getPlayerSleepingOrWaiting();
		

		return inProcessingRange;

	}
	


}


