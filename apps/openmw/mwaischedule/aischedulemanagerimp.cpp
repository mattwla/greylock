#include "aischedulemanagerimp.hpp"

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

#include "../mwworld/timestamp.hpp"
//
//#include "aicombat.hpp"
//#include "aipursue.hpp"
//#include "spellcasting.hpp"
//#include "autocalcspell.hpp"
//#include "npcstats.hpp"
//#include "actorutil.hpp"
//#include "combat.hpp"
//#include "aicalledover.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


namespace MWAISchedule
{
	AIScheduleManager::AIScheduleManager()
	{

	}
	
	
	
	std::ifstream AIScheduleManager::fetchSchedule()
	{
		std::string schedule;
		std::string pod = MWBase::Environment::get().getWorld()->getTimeStamp().getPartOfDay();

		if (pod == "day")
		{
			schedule = ("schedules/summerday.csv");
		}
		else if (pod == "night")
		{
			schedule = ("schedules/summernight.csv");
		}
		else if (pod == "evening")
		{
			schedule = ("schedules/summerevening.csv");
		}
		else if (pod == "morning")
		{
			schedule = ("schedules/summermorning.csv");
		}
		else
		{
			schedule = "";
		}
		std::ifstream in(schedule.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << schedule <<  std::endl;

		return in;
	}

	bool AIScheduleManager::checkScheduleGlobal(std::string global) {
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

			int testg = MWBase::Environment::get().getWorld()->getGlobalInt(split[0]);
			int testi = std::stoi(split[1]);

		if ( MWBase::Environment::get().getWorld()->getGlobalInt(split[0]) == std::stoi(split[1]))
		{
			return true;
		}

		
		return false;
	}

	std::map<std::string, std::string> AIScheduleManager::mapSchedule(std::vector<std::vector<std::string>> vecvec)
	{
		
		std::map<std::string, std::string> schedule;

		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			if (schedule.count(vecvec[i][0]) == 1)
			{
				continue; //if we already have a task for this npc, skip to next line
			}
			else
			{
				bool passed = true;
				
				for (unsigned int i2 = 2; i2 < vecvec[i].size(); i2++) //global requirements start at third element and go until end of vector
				{
					if (!checkScheduleGlobal(vecvec[i][i2])) {
						passed = false; //did not meet global requirements, don't check any more break.
						break;
					}
				}

				if (passed)
				{
					schedule[vecvec[i][0]] = vecvec[i][1]; //We passed all tests, store the npcs name and the npcs aipackage string
				}

			}
		}

		return schedule;
	}


	
	void AIScheduleManager::updateSchedules()
	{
		
		std::ifstream in = fetchSchedule(); //find our csv of AI schedules, returns one for appropriate season and time of day
		
		//Scan through csv with boost's tokenizer, values on a line make up elements of a vector vec, each line is in turn stored in a vector vecvec
		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;
		std::vector<std::vector<std::string>> vecvec;
		
		std::string line;

		while (getline(in, line))
		{
			std::vector<std::string> vec;
			Tokenizer tok(line);
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it)
			{
				vec.push_back(*it);
			}
			vecvec.push_back(vec);
		}

		//parse our vector of vectors, get a map back of what each NPC should be doing.
		std::map<std::string, std::string> schedule = mapSchedule(vecvec);

		for (auto const& x : schedule)
		{
			
			
			
			
			
			MWWorld::Ptr ptr = MWBase::Environment::get().getWorld()->searchPtr(x.first, false);
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("xbarmarker", false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			MWWorld::CellStore* store = MWBase::Environment::get().getWorld()->getInterior("Seyda Neen, Arrille's Tradehouse");
			MWBase::Environment::get().getWorld()->moveObject(ptr, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);

				
			
			//MWMechanics::AiSequence& seq = ptr.getClass().getCreatureStats(ptr).getAiSequence();

			//seq.stack(MWMechanics::AiCalledOver("player"), ptr);
		}
	}

	
}
