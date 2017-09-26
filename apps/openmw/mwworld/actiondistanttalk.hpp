#ifndef GAME_MWWORLD_ACTIONDISTANTTALK_H
#define GAME_MWWORLD_ACTIONDISTANTTALK_H

#include "ptr.hpp"
#include "action.hpp"

namespace MWWorld
{
	class ActionDistantTalk : public Action
	{
		virtual void executeImp(const Ptr& actor);

	public:

		ActionDistantTalk(const Ptr& actor);
		///< \param actor The actor the player is talking to

		
	};
}

#endif
