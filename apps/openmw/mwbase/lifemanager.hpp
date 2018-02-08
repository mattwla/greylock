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
		std::string mId;
		MWWorld::Ptr mPtr;
		float mHunger;
		float mThirst;
		float mWill;
		MWTasks::Task* mCurrentTask;
		std::vector<MWTasks::Task> mAvailableActions;
		std::vector<MWBase::Status> mStatusList;
		//MWBase::AIScheduleManager mScheduleManager;

	};

	class LifeManager
	{
		
	public:

		virtual void update(float duration, bool paused) = 0;

		virtual void initialize() = 0;


		virtual ~LifeManager() {}

	};

	
}
#endif
