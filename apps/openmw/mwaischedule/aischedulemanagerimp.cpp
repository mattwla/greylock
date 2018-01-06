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

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"

//
//#include "aicombat.hpp"
//#include "aipursue.hpp"
//#include "spellcasting.hpp"
//#include "autocalcspell.hpp"
//#include "npcstats.hpp"
//#include "actorutil.hpp"
//#include "combat.hpp"
//#include "aicalledover.hpp"

//Should I include pathfinding?

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


namespace MWAISchedule
{
	AIScheduleManager::AIScheduleManager()
	{
		
		
	}


	
	

	
	//ESM::Pathgrid::Point dest = target.getRefData().getPosition().pos;

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
		/*if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << schedule <<  std::endl;*/

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

			//int testg = MWBase::Environment::get().getWorld()->getGlobalInt(split[0]);
			//int testi = std::stoi(split[1]);

		if ( MWBase::Environment::get().getWorld()->getGlobalInt(split[0]) == std::stoi(split[1]))
		{
			return true;
		}

		
		return false;
	}

	std::vector<AIScheduleManager::TaskPriorityPair*> AIScheduleManager::mapSchedule(std::vector<std::vector<std::string>> vecvec)
	{
		//mwx fix me pointer madness
		std::vector<TaskPriorityPair*> schedule;

		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			bool alreadyassigned = false;
			
			for (unsigned int npcidsearch = 0; npcidsearch < schedule.size(); npcidsearch++) //loop to see if npc has already been assigned a task
			{
				if (schedule[npcidsearch]->npcId == vecvec[i][0])
					alreadyassigned = true;
			}

			if (alreadyassigned)
				continue;
			
			//if (schedule.count(vecvec[i][0]) == 1)
			//{
			//	continue; //if we already have a task for this npc, skip to next line
			//}
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
					//schedule[vecvec[i][0]] = vecvec[i][1]; //We passed all tests, store the npcs name and the npcs aipackage string
					schedule.push_back(new TaskPriorityPair(vecvec[i][0], vecvec[i][1], 4));


				}

			}
		}

		return schedule;
	}


	
	void AIScheduleManager::updateSchedules()
	{
		
		/*mtravelNodeMap = buildTravelNodes();
		buildPathGrid(&mtravelPathGrid);
		mtravelPathGridGraph = MWMechanics::PathgridGraph(&mtravelPathGrid);
		mtravelPathGridGraph.load(); */
		//HACK FIX TO DEAL WITH CHANGING POINTERS

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
		std::vector<AIScheduleManager::TaskPriorityPair*> schedule = mapSchedule(vecvec);

		for (auto const& x : schedule)
		{
			
			taskRouter(x->npcId, x->task, x->priority);
			
		}
		//updateJourneys();
	}

	void AIScheduleManager::taskRouter(std::string npcID, std::string task, int priority)
	{
		MWWorld::Ptr ptr = MWBase::Environment::get().getWorld()->searchPtr(npcID, false);

		if (task == "home")
		{
			goHome(ptr);
		}
		else if (task == "bar")
		{
			goBar(ptr);
		}
		else if (task == "outside")
		{
			goOutside(ptr);
		}
		else if (task == "balmora")
		{
			goBalmora(ptr);
		}
		else if (task == "crossbalmora")
		{
			crossBalmora(ptr);
		}


	}

	/*void AIScheduleManager::updateJourneys()
	{
		
		for (unsigned int i = 0; i < mActiveJourneys.size(); i++)
		{
			if (mActiveJourneys[i]->readyForUpdate())
			{
				mActiveJourneys[i]->update();
			}
		}
	}

	void AIScheduleManager::clearJourneys()
	{
		mActiveJourneys.clear();
	
	}*/



	MWWorld::Ptr AIScheduleManager::getHome(MWWorld::Ptr npc)
	{
		//NPCs all have global variable called idhome where is their id. There are home markers all called homeint where int is a label for which home it is. NPC homes are looked up by checking the int registered under their id+home than looking up home+ the int found under their global var
		//MWX redo by simply looking up interior name? makes tonsa sense yo. see ref x
		std::string name = npc.getCellRef().getRefId();
		std::string houseNumber = std::to_string(MWBase::Environment::get().getWorld()->getGlobalInt(name+"home"));
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("home_" + houseNumber, false); //MWX search by cell name? ref x
		return marker;
	}

	std::string AIScheduleManager::getBed(std::string npcId)
	{
		//issue here with using ptr?
		std::string bedNumber = std::to_string(MWBase::Environment::get().getWorld()->getGlobalInt(npcId + "bed"));
		return "bed_" + bedNumber;
	}

	

	bool AIScheduleManager::goHome(MWWorld::Ptr npc)
	{
		
		
		MWWorld::Ptr marker = getHome(npc);
		

		//= MWBase::Environment::get().getWorld()->searchPtr("xbarmarker", false);
		ESM::Position markerPos = marker.getRefData().getPosition();
		MWWorld::CellStore* store = marker.getCell();
		MWBase::Environment::get().getWorld()->moveObject(npc, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
		return true;

	}

	bool AIScheduleManager::goBar(MWWorld::Ptr npc)
	{
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("xbarmarker", false);
		
		ESM::Position markerPos = marker.getRefData().getPosition();
		MWWorld::CellStore* store = marker.getCell();
		MWBase::Environment::get().getWorld()->moveObject(npc, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
		return true;
	}

	bool AIScheduleManager::goOutside(MWWorld::Ptr npc)
	{
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("eldafireoutside", false);
		ESM::Position markerPos = marker.getRefData().getPosition();
		MWWorld::CellStore* store = marker.getCell();
		MWBase::Environment::get().getWorld()->moveObject(npc, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
		
		MWMechanics::AiSequence& seq = npc.getClass().getCreatureStats(npc).getAiSequence();
		seq.stack(MWMechanics::AiWave("player", true), npc);
		
		return true;
	}

	bool AIScheduleManager::goBalmora(MWWorld::Ptr npc)
	{
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("xbalmora1", false);
		//travel(npc, marker);
		//ESM::Position markerPos = marker.getRefData().getPosition();
		//MWWorld::CellStore* store = marker.getCell();
		//MWBase::Environment::get().getWorld()->moveObject(npc, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
		return true;
	}

	bool AIScheduleManager::crossBalmora(MWWorld::Ptr npc)
	{
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("xbalmora3", false);
		
		ESM::Position markerPos = marker.getRefData().getPosition();

		MWMechanics::AiSequence& seq = npc.getClass().getCreatureStats(npc).getAiSequence();
		seq.stack(MWMechanics::AiTravel(markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]), npc);
		return true;
	}

	std::string AIScheduleManager::fetchCurrentScheduledTask(std::string npcId)
	{

		//MWX WILL NEED REFACTORING

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
		std::vector<TaskPriorityPair*> schedule = mapSchedule(vecvec);

		for (auto const& x : schedule)
		{

			if (x->npcId == npcId)
			{
				return x->task;
			}
			
			//taskRouter(x.first, x.second);

		}
		
		
		return "";
	}

	

	
	

	


	
}
