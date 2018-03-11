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
	bool AIScheduleManager::checkScheduleGlobal(std::string global)
	{ 
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
		if ( MWBase::Environment::get().getWorld()->getGlobalInt(split[0]) == std::stoi(split[1]))
		{
			return true;
		}	
		return false;
	}

	std::string AIScheduleManager::getBed(std::string npcId)
	{
		//issue here with using ptr?
		std::string bedNumber = std::to_string(MWBase::Environment::get().getWorld()->getGlobalInt(npcId + "bed"));
		return "bed_" + bedNumber;
	}
}
