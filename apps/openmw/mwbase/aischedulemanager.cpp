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
		std::ifstream in = getCSV(npcId);

		ScheduleParserExpecting expecting = Time;

		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;
		std::vector<std::vector<std::string>> vecvec;

		std::string line;
		float time;
		TimeBlock * timeblock = NULL;

		while (getline(in, line))
		{
			if (line[0] == *"#") //skip comment lines
				continue;
			if (expecting == Time) //expecting a time, so turn the line into a float and add it to the mTimeBlocks structure
			{
				char *end;
				time = std::strtof(line.c_str(), &end); //mwx fix me error reporting would be nice here
				std::cout << "found float" << std::endl;
				std::cout << time << std::endl;
				expecting = Task;
				mTimeBlocks[time];
				continue;
			}
			if (expecting == Task)
			{
				if (line[0] == *"=")
				{
					if (timeblock)
					{
						mTimeBlocks[time] = timeblock;
						timeblock = NULL;
					}
					expecting = Time;
					continue;
				}
				timeblock = new TimeBlock;
				timeblock->mPossibleTasks.push_back(new TaskPriorityPair(line));
				//TaskPriorityPair * task = new TaskPriorityPair(line);


			}
			std::vector<std::string> vec;
			Tokenizer tok(line);
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)
			{
				vec.push_back(*it);
			}
			vecvec.push_back(vec);
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

	std::string AIScheduleManager::Schedule::getScheduledTask()
	{
		float hour = floor(MWBase::Environment::get().getWorld()->getTimeStamp().getHour());
		while (mTimeBlocks[hour] == NULL)
		{
			hour -= 1;
			if (hour < 0)
				hour = 23.0f;
		}
		return mTimeBlocks[hour]->getPossibleTask();
		//return "blah";
	}

	std::string AIScheduleManager::TimeBlock::getPossibleTask()
	{
		//checks globals for each task, returns the first one that passes the checks.
		unsigned int idx = 0;
		while (idx < mPossibleTasks.size())
		{
			return mPossibleTasks[idx]->task;
		}
	}
}
