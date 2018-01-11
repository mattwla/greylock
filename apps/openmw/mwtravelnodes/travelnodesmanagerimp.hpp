#ifndef GAME_MWMECHANICS_TRAVELNODESMANAGERIMP_H
#define GAME_MWMECHANICS_TRAVELNODESMANAGERIMP_H

#include "../mwbase/travelnodesmanager.hpp"

#include "../mwworld/ptr.hpp"

namespace MWWorld
{
	class CellStore;
}

namespace MWTravelNodes
{
	class TravelNodesManager : public MWBase::TravelNodesManager
	{

	public:

		TravelNodesManager();

		~TravelNodesManager();



		virtual std::map<int, TravelNode*> buildTravelNodes();

		virtual void buildPathGrid(ESM::Pathgrid *grid);

		virtual int idToIdx(std::string id);

	};
}


#endif