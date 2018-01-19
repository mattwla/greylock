#include "Hunt.hpp"

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

#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/aiactivate.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "journey.hpp"
#include "get.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{
	
	Hunt::Hunt(std::string npcId) 
	{
		mNpcId = npcId;
		mStep = 0;
		mDone = false;
	}

	void Hunt::update()
	{
		if (mStep == 0)
		{
			std::cout << "getting bow..." << std::endl;
			mSubTask = new MWTasks::Get("nadia bow", mNpcId);
			mStep += 1;
		}
		else if (mStep == 1)
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
			}
		}
		else if (mStep == 2)
		{
			mDestId = "tn_slt3";
			mSubTask = new MWTasks::Journey(mDestId, mNpcId);
			mStep += 1;
		}
		else if (mStep >= 3)
		{
			mSubTask->update();
			if (mSubTask->mDone)
			{
				std::cout << "looping journey" << std::endl;
				delete mSubTask;
				mSubTask = NULL;
				mStep += 1;
				if (mStep % 2 == 0)
				{
					mSubTask = new MWTasks::Journey("tn_slt2", mNpcId);
				}
				else
				{
					mSubTask = new MWTasks::Journey(mDestId, mNpcId);
				}
			}
		}
	


	}



	int Hunt::getTypeId() const
	{
		return TypeIDHunt;
	}

}

