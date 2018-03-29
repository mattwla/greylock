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

		virtual bool awarenessCheck(const MWWorld::Ptr & ptr, const MWWorld::Ptr & observer);

	/*	virtual std::vector<MWWorld::Ptr> calculateAwareness(MWWorld::Ptr ptr);*/

		//virtual bool awarenessCheck(const MWWorld::Ptr & ptr, const MWWorld::Ptr & observer);

		//bool turnTo(MWWorld::Ptr actor, MWWorld::Ptr target);

	};
	
}

#endif
