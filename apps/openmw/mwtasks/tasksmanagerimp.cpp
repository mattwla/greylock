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
typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

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
			mNpcIdToZones[npc] = buildZoneMap(npc);
			//CREATE AND ASSIGN NEW LIFE TASK HERE MWX
		}

		

		return npcmap;
	}
	

	TasksManager::TasksManager() :
		mLastTimeReported(0.0f)
		, mTimePassed(0.0f)
		, mTimeAccumulator(0.0f)
	{
		mNpcMap = buildNpcMap(); //maps npcs to their life task, also builds npc zone preference map
		buildZoneAvailabilities(); 
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
		if (mTimePassed < 0)
		{
			mTimePassed += 24.0f;
			std::cout << "midnight time boost, mtimepassed now = " + std::to_string(mTimePassed) << std::endl;
		}
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
			//std::cout << "ticking..." << std::endl;
			for (auto& sm_pair : mNpcMap)
			{
				sm_pair.second->update();
				//std::cout << isInActiveRange(sm_pair.first) << std::endl;
				//Make a seperate vector just for updating?.... not a horrible idea.
			}
			ticks -= 1;
		}

		//thanks JLBorges at cplusplus.com
	}

	Task * TasksManager::getScheduledTask(std::string npcId, std::string stask)
	{
		//std::string stask = MWBase::Environment::get().getAIScheduleManager()->fetchCurrentScheduledTask(npcId);

		if (stask == "journey")
		{
			std::string destID = "tn_sl";
			//MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("tnode4", false);
			MWTasks::Journey * rtask = new MWTasks::Journey(destID, npcId); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
			
			//JOURNEY
		}
		if (stask == "hunt")
		{

			std::string destId = "tn_slt3";
			//MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("tnode4", false);
			MWTasks::Hunt * rtask = new MWTasks::Hunt(destId, npcId); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
		}
		if (stask == "sleep")
		{
			
			std::string destId = MWBase::Environment::get().getAIScheduleManager()->getBed(npcId);
			//std::cout << npcId + " wants to sleep." << std::endl;
			MWTasks::Sleep * rtask = new MWTasks::Sleep(destId, npcId);
			return rtask;
		}
		if (stask == "dance")
		{
			std::string destId = "tn_v1";
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

	std::string TasksManager::getZoneId(std::string npcId, std::string task)
	{
		//mNpcMap[npcId]->
		auto npcmap = mNpcIdToZones[npcId];
		auto taskmap = npcmap[task];
		return taskmap[0]->mZoneId;
		//mwx fix me doesn't go globals yet
		//return std::string();
	}
	

	std::map<std::string, std::vector<TasksManager::ZoneGlobalPair*>> TasksManager::buildZoneMap(std::string npcId)
	{

		
		//open the npcs schedule file
		std::ifstream in = getCSV(npcId);
		//set up the parsers default state, first we need to skip all the schedule info preceding zone info
		ZoneParserExpecting expecting = Skip;
		//get tokenizer ready for seperating values by commas
		
		std::vector<std::vector<std::string>> vecvec;
		std::string line;
		//Make z zgp pointer, for use in caching a line before storing it
		//ZoneGlobalPair * zoneglobalpair = NULL;
		std::string currentTask;
		std::map<std::string, std::vector<TasksManager::ZoneGlobalPair*>> zmap;

		//iterate through csv
		while (getline(in, line))
		{
			if (expecting == Skip)
			{
				if (line != "ZONESSTART") //skip all lines until we get to the starting marker
					continue;
				else
				{
					expecting = TaskName;
					continue;
				}
			}
			if (expecting == TaskName) //expecting a task, so store this in our map of tasks
			{
				zmap[line];
				currentTask = line;
				expecting = Zone;
				continue;
			}
			if (expecting == Zone)
			{
				if (line[0] == *"=") //= is used to say when it is time for the next task
				{
					expecting = TaskName;
					continue;
				}
				ZoneGlobalPair * zgp = new ZoneGlobalPair;
				int idx = 0;
				Tokenizer tok(line);
				for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
				{
					if (idx == 0) //first item in line is the zone
						zgp->mZoneId = *it;
					else //everything else are the conditions needed to use that zone.
						zgp->mGlobals.push_back(*it);
					idx += 1;
				}
				zmap[currentTask].push_back(zgp); //We have made a struct which contains a zone and the globals needed to use that zone, push it to the map that links tasks with the possible zones.


												

			}
		}

		return zmap;

	}

	int TasksManager::getZoneAvailability(std::string zoneId)
	{
		//mwx fix me, what if no slots available?
		auto zone = mZoneAvailabilities[zoneId];
		unsigned int idx = 0;
		while (idx < zone->mAvailable.size())
		{
			if (zone->mAvailable[idx] == true)
			{
				zone->mAvailable[idx] = false;
				return idx + 1;
					//zoneId + "_" + std::to_string(idx + 1);
			}
			idx += 1;
		}

		//return "full";
		return -1;
	}

	void TasksManager::freeZoneSlot(std::string zoneId, int idx)
	{
		auto zone = mZoneAvailabilities[zoneId];
		zone->mAvailable[idx - 1] = true;
	}

	void TasksManager::buildZoneAvailabilities()
	{
		std::string zones = "schedules/zones.csv";
		std::ifstream in(zones.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << zones << std::endl;
		
		std::string line;
		while (getline(in, line))
		{
	
			int idx = 0;
			ZoneAvailability* za = new ZoneAvailability;
			Tokenizer tok(line);
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
			{
				if (idx == 0)
				{
					mZoneAvailabilities[*it];
					za->mZoneId = *it;
					idx += 1;
				}
				else
				{
					int n = std::stoi(*it);
					za->mZoneSlots = n;
					int idx2 = 0;
					while (idx2 < n)
					{
						za->mAvailable.push_back(true);
						idx2 += 1;
					}

				}
			}
			std::cout << "zone" + za->mZoneId + "has " << std::endl;
			std::cout << za->mZoneSlots << std::endl;
			
			mZoneAvailabilities[za->mZoneId] = za;
		}
		
	}



	std::ifstream TasksManager::getCSV(std::string npcId)
	{
		std::string schedule = "schedules/" + npcId + ".csv";
		std::ifstream in(schedule.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << schedule << std::endl;

		return in;
	}

	


	//std::map<std::string, std::vector<TasksManager::ZoneGlobalPair*>> TasksManager::buildZoneMap(std::string npcId)
	//{
	//	return std::map<std::string, std::vector<ZoneGlobalPair*>>();
	//}

}


//to do
//add zone info for all npcs
//Add nadia fire pit dance node
//Add method of looking up and assigning node