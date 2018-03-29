#include "life.hpp"


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
#include "../mwworld/refdata.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/customdata.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <string>

namespace MWTasks
{

	Life::Life(std::string npcId)
	{
		
	}

	Life::~Life()
	{
		if (mSubTask)
			delete mSubTask;

		MWBase::Environment::get().getTasksManager()->endLife(mNpcId);
	}

	int Life::getTypeId() const
	{
		return TypeIDLife;
	}

	MWWorld::Ptr Life::update()
	{
		return nullptr;
	}

	std::string Life::getZoneId(std::string task)
	{
		return "";
	}

	

}