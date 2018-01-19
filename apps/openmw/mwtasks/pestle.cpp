#include "pestle.hpp"

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
#include "../mwbase/tasksmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aiwave.hpp"
#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/aiactivate.hpp"
#include "../mwmechanics/pathgrid.hpp"
#include "journey.hpp"

#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>

namespace MWTasks
{

	Pestle::Pestle(std::string npcId) 	
	{
		mNpcId = npcId;
		mNpcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
		mStep = 0;
		mDone = false;
	}

	Pestle::~Pestle()
	{
		MWBase::Environment::get().getTasksManager()->freeZoneSlot(mZoneId, mZoneSlotIdx);
	}

	void Pestle::update()
	{
	
		if (mStep == 0)
		{
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(npcPtr, "rock", 0, 1);
			mZoneId = MWBase::Environment::get().getTasksManager()->getZoneId(mNpcId, "pestle");
			mZoneSlotIdx = MWBase::Environment::get().getTasksManager()->getZoneAvailability(mZoneId);
			//request a spot in the dance zone
			//make a journey to that quest
			mDestId = mZoneId + "_" + std::to_string(mZoneSlotIdx);
			mSubTask = new MWTasks::Journey(mDestId, mNpcId);
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
			//MWWorld::Ptr npcPtr = MWBase::Environment::get().getWorld()->searchPtr(mNpcId, false);
			MWWorld::Ptr marker = MWBase::Environment::get().getWorld()->searchPtr(mDestId, false);
			ESM::Position markerPos = marker.getRefData().getPosition();
			//markerPos.rot[2];
			MWBase::Environment::get().getWorld()->rotateObject(mNpcPtr, 0, 0, markerPos.rot[2]); //face direction of zoneslot
			if (MWBase::Environment::get().getTasksManager()->isInActiveRange(mNpcId)) {
				if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(mNpcPtr, "grind"))
				{
					MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(mNpcPtr, "grind", 0, 1);
				}
			}
		}
	}

	int Pestle::getTypeId() const
	{
		return TypeIDPestle;
	}

}

