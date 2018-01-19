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

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>


namespace MWAISchedule
{
	AIScheduleManager::AIScheduleManager()
	{
	
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
}
