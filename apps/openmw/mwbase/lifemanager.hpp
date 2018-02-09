#ifndef GAME_MWBASE_LIFEMANAGER_H
#define GAME_MWBASE_LIFEMANAGER_H

#include <string>
#include <vector>
#include <list>
#include <set>
#include <stdint.h>
#include <map>

#include <list>

#include <components/esm/loadpgrd.hpp>
#include "../mwmechanics/pathgrid.hpp"
#include "../mwworld/timestamp.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwtasks/task.hpp"
#include "../mwbase/statusmanager.hpp"
#include "../mwbase/aischedulemanager.hpp"
#include "../mwtasks/life.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"


#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>



namespace osg
{
	class Vec3f;
}

namespace ESM
{
	struct Class;

	class ESMReader;
	class ESMWriter;
}

namespace MWWorld
{
	class Ptr;
	class CellStore;
	class CellRef;
}

namespace Loading
{
	class Listener;
}

namespace MWBase
{
	struct Life
	{
		
		Life(std::string id)
		{
		
		

			mId = id;
			mPtr = MWBase::Environment::get().getWorld()->searchPtr(id, false);
			//mPtr = ptr;
			mHunger = 0;
			mThirst = 0;
			mWill = 100;
			mCurrentTask = 0;
			mSchedule = new MWBase::AIScheduleManager::Schedule(id);
			mTaskChain = new MWTasks::Life(id);
			mAwareOfList = MWBase::Environment::get().getAwarenessReactionsManager()->calculateAwareness(mPtr);
			mAvailableActions = MWBase::Environment::get().getAwarenessReactionsManager()->calculateReactions(mPtr);
			MWBase::Environment::get().getStatusManager()->initNpcStatus(mId);
			
		}
	
	public:
		std::string mId;
		MWWorld::Ptr mPtr;
		float mHunger;
		float mThirst;
		float mWill;
		MWBase::AIScheduleManager::Schedule* mSchedule;
		MWTasks::Life* mTaskChain;
		std::vector<MWWorld::Ptr> mAwareOfList;
		MWTasks::Task* mCurrentTask;
		std::map<MWTasks::Task::TypeID, int> mAvailableActions; //task enum, valence of task. Actions npc can take in the environment.
		std::vector<MWBase::Status> mStatusList;
		//MWBase::AIScheduleManager mScheduleManager;

	};

	class LifeManager
	{
		
	public:

		virtual void update(float duration, bool paused) = 0;

		virtual void initialize() = 0;

		virtual ~LifeManager() {}

		std::vector<MWBase::Life*> mLifeList;

		//virtual void updateAwareness() = 0;
		
		virtual bool inActiveRange(MWWorld::Ptr npc) = 0;

		float mLastTimeReported;

		float mTimePassed;

		float mTimeAccumulator;


	};

	
}
#endif
