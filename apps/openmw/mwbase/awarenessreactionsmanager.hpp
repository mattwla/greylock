#ifndef GAME_MWBASE_AWARENESSREACTIONS_H
#define GAME_MWBASE_AWARENESSREACTIONS_H

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
	struct Life;
	
	//enum Status
	//{
	//	Shaman = 0,
	//	Elder = 1,
	//	Wounded = 2

	//};

	class AwarenessReactionsManager
	{
		/*AwarenessReactionsManager(const AwarenessReactionsManager&);
		///< not implemented

		AwarenessReactionsManager& operator= (const AwarenessReactionsManager&);
		///< not implemented
*/
	public:
		std::vector<MWWorld::Ptr> mLiveCellAffordances; //all ways to interact with world in live cells

		std::map<MWWorld::Ptr, int> mLiveCellGuardZones; //all guard zones with associated radius

		std::map<MWWorld::Ptr, std::vector<MWWorld::Ptr>> mNpcAwareOf;

		//std::map<MWWorld::Ptr, std::vector<MWBase::Status>> mNpcStatusList;
		
		virtual MWWorld::Ptr getGuardZoneOut(MWWorld::Ptr npcPtr) = 0;
		
		virtual void updateActiveAffordances() = 0;

		virtual std::vector<MWWorld::Ptr> calculateAwareness(MWWorld::Ptr ptr) = 0;

		virtual std::map<MWTasks::Task*, int> AwarenessReactionsManager::calculateReactions(MWWorld::Ptr npc, MWBase::Life& life) = 0;


		virtual ~AwarenessReactionsManager() {}

	};

	
}
#endif
