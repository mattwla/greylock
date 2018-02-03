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

		

	public:
		AwarenessReactionsManager();

		virtual void updateActiveAffordances();

		virtual void calculateAwareness(MWWorld::Ptr ptr);

		bool awarenessCheck(const MWWorld::Ptr & ptr, const MWWorld::Ptr & observer);

	};
}

#endif
