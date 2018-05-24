#ifndef GAME_MWWORLD_ACTIONTAKE_H
#define GAME_MWWORLD_ACTIONTAKE_H

#include "action.hpp"
#include "ptr.hpp"

namespace MWWorld
{
    class ActionTake : public Action
    {
            virtual void executeImp (const Ptr& actor);

        public:

            ActionTake (const MWWorld::Ptr& object);
    };

	class ActionHold : public Action
	{
		virtual void executeImp(const Ptr& actor);

	public:

		ActionHold(const MWWorld::Ptr& object);
	};
}

#endif
