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
	class SmartEntityInstance;
}



namespace MWBase
{
	struct Life;

	struct SensoryLink {
		MWWorld::Ptr mPtr;
		MWBase::SmartEntityInstance * mSEInstance;

		SensoryLink::SensoryLink(MWWorld::Ptr ptr, MWBase::SmartEntityInstance * sei)
		{
			mPtr = ptr;
			mSEInstance = sei;
			
		}
	};

	struct SensoryLinkStore {

		std::vector<SensoryLink> mCurrentSensoryLinks;
	
		std::vector<SensoryLink> mSensoryLinksInMemory;

		void addSensoryLink(SensoryLink sensorylink);

		~SensoryLinkStore();
	
	};


	class Awareness {

		MWWorld::Ptr mPtr;

		SensoryLinkStore* mSensoryLinkStore;

	public:
		void refresh();

		void getDebugInfo();

		Awareness::Awareness(MWWorld::Ptr ptr);

		SensoryLinkStore * getSensoryLinkStore();


	};
	
	class AwarenessReactionsManager
	{
	
	public:
			
		virtual bool AwarenessReactionsManager::awarenessCheck(const MWWorld::Ptr &ptr, const MWWorld::Ptr &observer) = 0;

		virtual ~AwarenessReactionsManager() {}

	};

}
#endif
