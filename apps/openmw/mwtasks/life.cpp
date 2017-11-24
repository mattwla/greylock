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

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/tasksmanager.hpp"

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/pathgrid.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <string>

namespace MWTasks
{

	

	Life::Life(std::string npcId)
	{
		mNpcId = npcId;
	}

	int Life::getTypeId() const
	{
		return TypeIDLife;
	}

	void Life::update()
	{
		if (!mSubTask)
		{
			mSubTask = MWBase::Environment::get().getTasksManager()->getScheduledTask(mNpcId);
			if (mSubTask && mSubTask->getTypeId() == TypeIDJourney)
			{
				mSubTask->mNpcId = mNpcId;
				
			}

		}
		else
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
			}
		}

	}



}