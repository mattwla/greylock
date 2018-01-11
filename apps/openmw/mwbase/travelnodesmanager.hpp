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


		TravelNodesManager() {}

		virtual ~TravelNodesManager() {}

		std::map<std::string, int> mIdToItx;

		struct TravelNode
		{
			//ALL SUBJECT TO CHANGE

			int id; //An id, static, must be used for astar to function. (use to make a map?)
			std::string marker; //An id for an in game location, for spawning NPCs
			ESM::Pathgrid::Point point; //A point for astar pathfinding
			std::vector<MWWorld::Ptr> npcs; //A list of npcs at node
		};

		std::map<int, TravelNode*>  mtravelNodeMap;

		std::map<MWWorld::CellStore*, TravelNode*> mCellToNodeMap;


		
		ESM::Pathgrid mtravelPathGrid;

		MWMechanics::PathgridGraph mtravelPathGridGraph;

		virtual std::map<int, TravelNode*> buildTravelNodes() = 0;

		virtual void buildPathGrid(ESM::Pathgrid *grid) = 0;
		
		virtual int idToIdx(std::string id) = 0;


	};


}



#endif
