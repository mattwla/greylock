#ifndef GAME_MWBASE_STATUSMANAGER_H
#define GAME_MWBASE_STATUSMANAGER_H

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
	enum Status {
		Shaman = 0,
		Elder = 1,
		Guarding = 2,
		Fighting = 3,
		InConversation = 4,
		ConfrontingTrespasser = 5,
		InWallHold = 6,
		InWallJump = 7

	};


	class StatusManager
	{
		/*StatusManager(const StatusManager&);
		///< not implemented

		StatusManager& operator= (const StatusManager&);
		///< not implemented*/

	public:
		
	

		virtual ~StatusManager() {}

		std::map<MWWorld::Ptr, std::vector<Status>> mStatusMap;

		virtual void initNpcStatus(std::string npcid) = 0;

		virtual bool hasStatus(MWWorld::Ptr ptr, MWBase::Status status) = 0;

		virtual void giveStatus(MWWorld::Ptr, MWBase::Status status) = 0;

		virtual void updatePtr(MWWorld::Ptr old, MWWorld::Ptr newptr) = 0;

		virtual void removeStatus(MWWorld::Ptr ptr, MWBase::Status status) = 0;


	};

	
}

#endif
