#include "aischedulemanager.hpp"
#include "../mwbase/environment.hpp"
#include "../mwworld/timestamp.hpp"
#include "../mwbase/world.hpp"
#include <iostream>
#include <fstream>
#include <limits.h>

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

typedef MWBase::AIScheduleManager::TaskPriorityPair TaskPriorityPair;
typedef MWBase::AIScheduleManager::TimeBlock TimeBlock;

namespace MWBase
{
	AIScheduleManager::Schedule::Schedule(std::string npcId)
	{
		//logic to build schedule
		//open the npcs schedule file
		std::ifstream in = getCSV(npcId);
		std::cout << "SCHEDULE FOR =" + npcId << std::endl;
		//set up the parsers default state
		ScheduleParserExpecting expecting = Time;
		//get tokenizer ready for seperating values by commas
		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;
		std::vector<std::vector<std::string>> vecvec;
		std::string line;
		float time;
		//get a timeblock pointer ready, timeblock contains all the tasks an npc is scheduled to do at a given time
		TimeBlock * timeblock = NULL;
		//iterate through csv
		while (getline(in, line))
		{
			if (line[0] == *"#") //skip comment lines
				continue;
			if (line == "ZONESSTART")
				break;
			if (expecting == Time) //expecting a time, so turn the line into a float and add it to the mTimeBlocks structure
			{
				char *end;
				time = std::strtof(line.c_str(), &end); //mwx fix me error reporting would be nice here
				//std::cout << "found float" << std::endl;
				//std::cout << time << std::endl;
				expecting = Task;
				mTimeBlocks[time];
				continue;
			}
			if (expecting == Task)
			{
				if (line[0] == *"=") //= is used to say when it is time for the next hour
				{
					if (timeblock) //if the time block pointer is pointing to a timeblock, log it (won't be if NPC isn't scheduled to do anything)
					{
						mTimeBlocks[time] = timeblock;
						timeblock = NULL; //be sure to clear timeblock pointer for next timeslot
					}
					expecting = Time; //get ready to read the time
					continue;
				}
				if(!timeblock) //if we haven't yet made a time block, make one
					timeblock = new TimeBlock;
				timeblock->mPossibleTasks.push_back(new TaskPriorityPair(line));
				std::cout << "adding task..." + line << std::endl;
				//TaskPriorityPair * task = new TaskPriorityPair(line);
			}
		}
	}

	std::ifstream AIScheduleManager::Schedule::getCSV(std::string npcId)
	{
		std::string schedule = "schedules/" + npcId + ".csv";
		std::ifstream in(schedule.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << schedule << std::endl;

		return in;
	}

	MWTasks::Task::TypeID AIScheduleManager::Schedule::getScheduledTask()
	{
		float hour = floor(MWBase::Environment::get().getWorld()->getTimeStamp().getHour());
		while (mTimeBlocks[hour] == NULL) //if nothing scheduled for this time block, go back in time until we can find something.
		{
			hour -= 1;
			if (hour < 0)
				hour = 23.0f;
		}
		//mwx fix me what of a case where there is a time block but none of the tasks are possible?
		return mTimeBlocks[hour]->getPossibleTask();
		return MWTasks::Task::TypeID::TypeIDGet; //use get as blank task for now, it will never be in schedule.
	}

	MWTasks::Task::TypeID AIScheduleManager::TimeBlock::getPossibleTask()
	{
		//checks globals for each task, returns the first one that passes the checks.
		//for now just returns the first one
		unsigned int idx = 0;
		while (idx < mPossibleTasks.size())
		{
			if (checkScheduleGlobals(mPossibleTasks[idx]->mGlobals))
				return mPossibleTasks[idx]->task;
			else
				idx += 1;
		}
		return MWTasks::Task::TypeIDGet;
	}

	bool AIScheduleManager::TimeBlock::checkScheduleGlobals(std::vector<std::string> globals) {
		for (unsigned int i = 0; i < globals.size(); i++)
		{
			std::string global = globals[i];
			//takes a string such as JacobIsAlive=1, splits it into the varname and value, evaluates if value specified in schedule is true.
			std::vector<std::string> split;
			std::string delim = "=";
			auto start = 0U;
			auto end = global.find(delim);
			while (end != std::string::npos)
			{
				split.push_back(global.substr(start, end - start));
				start = end + delim.length();
				end = global.find(delim, start);
			}
			split.push_back(global.substr(start, end));
			if (MWBase::Environment::get().getWorld()->getGlobalInt(split[0]) != std::stoi(split[1])) //not a match, return false
			{
				return false;
			}
		}
		return true; //all passed, return true
	}
}
