#ifndef GAME_MWBASE_TRAVELNODESMANAGER_H
#define GAME_MWBASE_TRAVELNODESMANAGER_H

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
	class TravelNodesManager
	{
		TravelNodesManager(const TravelNodesManager&);
		///< not implemented

		TravelNodesManager& operator= (const TravelNodesManager&);
		///< not implemented

	public:


		TravelNodesManager() {};


	};


}



#endif
