#ifndef GAME_TASKS_LIFE_H
#define GAME_TASKS_LIFE_H

#include "task.hpp"
#include "../mwbase/aischedulemanager.hpp"

#include "../mwworld/cellstore.hpp" // for Doors

#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Life : public Task
	{
	
	public:
		
		
		
		Life(std::string npcId);

		~Life();
		
		virtual int getTypeId() const;

		virtual MWWorld::Ptr update();

		std::string getZoneId(std::string task);

		int mTickCounter;

	
		

		

	};

}




#endif
