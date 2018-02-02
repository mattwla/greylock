#ifndef GAME_MWMECHANICS_AWARENESSREACTIONSMANAGER_H
#define GAME_MWMECHANICS_AWARENESSREACTIONSMANAGER_H

#include "../mwbase/awarenessreactionsmanager.hpp"

#include "../mwworld/ptr.hpp"

#include <string>


namespace MWWorld
{
	class CellStore;
}

namespace MWAwarenessReactions
{
	class AwarenessReactionsManager : public MWBase::AwarenessReactionsManager
	{

		std::vector<MWWorld::Ptr> mLiveCellAffordances; //all ways to interact with world in live cells

	public:
		AwarenessReactionsManager();

		virtual void updateActiveAffordances();

		

	};
}

#endif
