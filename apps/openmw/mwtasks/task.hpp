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
			TypeIDPestle = 6,
			TypeIDFish = 7,
			TypeIDSitground = 8,
			TypeIDFight = 9,
			TypeIDGuard = 10,
			TypeIDConfrontTrespasser = 11
	
		};
		
		Task();

		Task(MWWorld::Ptr npcPtr);

		MWWorld::Ptr mNpcPtr;

		virtual ~Task();

		std::string mNpcId;

		virtual int getTypeId() const = 0;

		bool mDone;

		virtual MWWorld::Ptr update() = 0; //update returns the latest ptr, as an update can do something like change cells and we need to keep track of what ptr is still useful.

		virtual void resume();

		Task* mSubTask = nullptr;

		Task* mLifeTask = nullptr; //keep track of the life task this task is attached too.

		int intensity = 4; //default low priority
		//0 life threatening
		//1 highly desirable
		//2 Noteworhty
		//3 Noticable
		//4 Ambivalent

	};
}




#endif