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

	class AwarenessReactionsManager
	{
		/*AwarenessReactionsManager(const AwarenessReactionsManager&);
		///< not implemented

		AwarenessReactionsManager& operator= (const AwarenessReactionsManager&);
		///< not implemented
*/
	public:
		std::vector<MWWorld::Ptr> mLiveCellAffordances; //all ways to interact with world in live cells

		virtual void updateActiveAffordances() = 0;

		virtual void calculateAwareness(MWWorld::Ptr ptr) = 0;
	};

	
}
#endif
