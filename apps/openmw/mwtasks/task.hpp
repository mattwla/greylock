#ifndef GAME_MWTASKS_TASK_H
#define GAME_MWTASKS_TASK_H
#include <components/esm/defs.hpp>
#include "../mwbase/travelnodesmanager.hpp"
#include "../mwbase/environment.hpp"

#include <string>

namespace MWWorld
{
	class Ptr;
}


namespace MWTasks
{

	class Task
	{
	public:

		enum TypeID {
			TypeIDLife = 0,
			TypeIDJourney = 1,
			TypeIDGet = 2,
			TypeIDHunt = 3,
			TypeIDSleep = 4,
			TypeIDDance = 5,
			TypeIDPestle = 6
		};
		
		Task();

		Task(std::string npcId);

		virtual ~Task();

		std::string mNpcId;

		virtual int getTypeId() const = 0;

		bool mDone;

		virtual void update() = 0;

		Task* mSubTask = nullptr;

		int priority = 4; //default low priority

	};
}




#endif