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
#include "../mwbase/statusmanager.hpp"

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
#include "../mwtasks/pestle.hpp"
#include "../mwtasks/fish.hpp"
#include "../mwtasks/sitground.hpp"
#include "../mwtasks/guard.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <map>
typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

namespace MWTasks
{
	TasksManager::TasksManager() :
		mLastTimeReported(0.0f)
		, mTimePassed(0.0f)
		, mTimeAccumulator(0.0f)
	{
		buildZoneAvailabilities(); 
	}

	Task * TasksManager::taskEnumToTask(MWTasks::Task* lifetask, MWTasks::Task::TypeID task)
	{
		//MWX fix me potential for templates here.

		if (task == MWTasks::Task::TypeIDJourney)
		{
			std::string destID = "tn_sl";
			MWTasks::Journey * rtask = new MWTasks::Journey(destID, lifetask); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDHunt)
		{
			MWTasks::Hunt * rtask = new MWTasks::Hunt(lifetask); //Make a journey task, fill it in with destination, let task being delivered to do rest. MWX for now
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDSleep)
		{
			MWTasks::Sleep * rtask = new MWTasks::Sleep(lifetask);
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDDance)
		{
			MWTasks::Dance * rtask = new MWTasks::Dance(lifetask);
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDPestle)
		{
			MWTasks::Pestle * rtask = new MWTasks::Pestle(lifetask);
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDFish)
		{
			MWTasks::Fish * rtask = new MWTasks::Fish(lifetask);
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDSitground)
		{
			MWTasks::Sitground * rtask = new MWTasks::Sitground(lifetask);
			return rtask;
		}
		if (task == MWTasks::Task::TypeIDGuard)
		{
			MWTasks::Guard * rtask = new MWTasks::Guard(lifetask);
			return rtask;
		}
		return nullptr;
	}


	

	std::string TasksManager::getZoneId(std::string npcId, std::string task)
	{
		auto npcmap = mNpcIdToZones[npcId];
		auto taskmap = npcmap[task];
		return taskmap[0]->mZoneId;
	}

	void TasksManager::endLife(std::string npcId)
	{
		std::cout << npcId + " has died"  << std::endl;
		mNpcMap.erase(npcId);
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
			if (line == "STATUSSTART")
				break;
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
		mNpcIdToZones[npcId] = zmap;
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

	
}
