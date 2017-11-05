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

MWBase::AIScheduleManager::Journey::Journey(std::string mNpcId, std::vector<int> mTravelNodeItinerary, MWWorld::Ptr mDestination) :
	mNpcId(mNpcId), mTravelNodeItinerary(mTravelNodeItinerary), mDestination(mDestination), mStep(0)
{
	//update();
}



void MWBase::AIScheduleManager::Journey::update()
{
	mStep = mStep + 1;
	auto m = MWBase::Environment::get().getAIScheduleManager()->mtravelNodeMap[mTravelNodeItinerary[mStep]];

	MWWorld::Ptr markerPtr = MWBase::Environment::get().getWorld()->searchPtr(m->marker, false);
	MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);


	ESM::Position markerPos = markerPtr.getRefData().getPosition();
	MWWorld::CellStore* store = markerPtr.getCell();

	
	MWBase::Environment::get().getWorld()->moveObject(npcPtr, store, markerPos.pos[0], markerPos.pos[1], markerPos.pos[2]);
	
}


namespace MWAISchedule
{
	AIScheduleManager::AIScheduleManager()
	{
		mtravelNodeMap = buildTravelNodes();
		buildPathGrid(&mtravelPathGrid);
	    mtravelPathGridGraph = MWMechanics::PathgridGraph(&mtravelPathGrid);
		mtravelPathGridGraph.load();
		
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

			//int testg = MWBase::Environment::get().getWorld()->getGlobalInt(split[0]);
			//int testi = std::stoi(split[1]);

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
		
		mtravelNodeMap = buildTravelNodes();
		buildPathGrid(&mtravelPathGrid);
		mtravelPathGridGraph = MWMechanics::PathgridGraph(&mtravelPathGrid);
		mtravelPathGridGraph.load(); 
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
		std::map<std::string, std::string> schedule = mapSchedule(vecvec);

		for (auto const& x : schedule)
		{
			
			MWWorld::Ptr ptr = MWBase::Environment::get().getWorld()->searchPtr(x.first, false);
			
			if (x.second == "home")
			{
				goHome(ptr);
			}
			else if (x.second == "bar")
			{
				goBar(ptr);
			}
			else if (x.second == "outside")
			{
				goOutside(ptr);
			}
			else if (x.second == "balmora")
			{
				goBalmora(ptr);
			}
			else if (x.second == "crossbalmora")
			{
				crossBalmora(ptr);
			}
			
				
			
		}
		updateJourneys();
	}

	void AIScheduleManager::updateJourneys()
	{
		
		for (unsigned int i = 0; i < mActiveJourneys.size(); i++)
		{
			mActiveJourneys[i]->update();
		}
	}

	void AIScheduleManager::clearJourneys()
	{
		mActiveJourneys.clear();
	
	}



	MWWorld::Ptr AIScheduleManager::getHome(MWWorld::Ptr npc)
	{
		//NPCs all have global variable called idhome where is their id. There are home markers all called homeint where int is a label for which home it is. NPC homes are looked up by checking the int registered under their id+home than looking up home+ the int found under their global var
		
		std::string name = npc.getCellRef().getRefId();
		std::string houseNumber = std::to_string(MWBase::Environment::get().getWorld()->getGlobalInt(name+"home"));
		MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr("home" + houseNumber, false);
		return marker;
	}

	bool AIScheduleManager::travel(MWWorld::Ptr npc, MWWorld::Ptr dest)
	{
		bool actorInLive;
		bool destInLive;
		
		actorInLive = MWBase::Environment::get().getWorld()->searchPtr(npc.getCellRef().getRefId(), true);
		destInLive = MWBase::Environment::get().getWorld()->searchPtr(dest.getCellRef().getRefId(), true);

		if (actorInLive && destInLive) //NPC can just walk there, so do that..... for now
		{
			ESM::Position destPos = dest.getRefData().getPosition();
			MWMechanics::AiSequence& seq = npc.getClass().getCreatureStats(npc).getAiSequence();
			seq.stack(MWMechanics::AiTravel(destPos.pos[0], destPos.pos[1], destPos.pos[2]), npc);
			return true;
		}

		//std::cout << npc.getCellRef().getRefId() << " in live: " << actorInLive << std::endl;
		//std::cout << dest.getCellRef().getRefId() << " in live: " << destInLive << std::endl; //GAHH, WE ARE SEARCHING BY NAME HERE NOT ID :/ EDIT: fixed, do same for actor?


		//We are here because NPC needs to traverse while outside of cell, so we will use the travelNode system
		//Build a path through the nodes
		
		//lookup what node is associated with NPCs current cell.
		int currentNode = mCellToNodeMap[npc.getCell()]->id;
		int destNode = mCellToNodeMap[dest.getCell()]->id;


		auto path = mtravelPathGridGraph.aStarSearch(currentNode, destNode); //WANT CURRENT NODE END NODE.
		
		//collect the ids of which nodes we will use
		std::vector<int> travelNodeList;
		for (std::list<ESM::Pathgrid::Point>::iterator it = path.begin(); it != path.end(); it++)
		{
			travelNodeList.push_back(it->mUnknown);
		}

		//Make a journey.
		MWBase::AIScheduleManager::Journey *j = new MWBase::AIScheduleManager::Journey(npc.getCellRef().getRefId(), travelNodeList, dest);
		
		mActiveJourneys.push_back(j); //Do I want to do this through a method?


		return true;
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
		travel(npc, marker);
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

	std::map<int, MWBase::AIScheduleManager::TravelNode*> AIScheduleManager::buildTravelNodes()
	{
		std::map<int, TravelNode*> nodeMap;
		
		std::string nodelist = ("schedules/travelnodes.csv");
		std::ifstream in(nodelist.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << nodelist << std::endl;

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
		
		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			MWBase::AIScheduleManager::TravelNode *tn = new MWBase::AIScheduleManager::TravelNode;
			tn->id = i;
			tn->marker = vecvec[i][0];
			MWWorld::Ptr markerPtr = MWBase::Environment::get().getWorld()->searchPtr(vecvec[i][0], false);
			ESM::Position markerPos = markerPtr.getRefData().getPosition();
			ESM::Pathgrid::Point *point = new ESM::Pathgrid::Point;
			point->mUnknown = i; //mUnknown seems unused, being used here to store the idx of point so we can look it up in nodeMap
			point->mX = markerPos.pos[0];
			point->mY = markerPos.pos[1];
			point->mZ = markerPos.pos[2];
			tn->point = *point;
			vecvec[i];
			nodeMap[i] = tn;
			//id,index,num of connections, connected to

			MWWorld::CellStore* store = markerPtr.getCell(); //Where will this ptr go? Probably away so figure out how to make this crash.
			mCellToNodeMap[store] = tn;
			

		}
		
		return nodeMap;
	}

	void AIScheduleManager::buildPathGrid(ESM::Pathgrid * grid)
	{
		for (unsigned int i = 0; i < mtravelNodeMap.size(); i++)
		{
			grid->mPoints.push_back(mtravelNodeMap[i]->point);
		}

		std::string nodelist = ("schedules/edges.csv");
		std::ifstream in(nodelist.c_str());
		if (!in.is_open())
			std::cout << "Not open" << std::endl;
		else
			std::cout << "Open " << nodelist << std::endl;

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


		for (unsigned int i = 0; i < vecvec.size(); i++)
		{
			ESM::Pathgrid::Edge e;
			e.mV0 = std::stoi(vecvec[i][0]);
			e.mV1 = std::stoi(vecvec[i][1]);
			grid->mEdges.push_back(e);
		}

		
	}
	

	


	
}
