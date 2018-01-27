#ifndef GAME_TASKS_HUNT_H
#define GAME_TASKS_HUNT_H

#include "task.hpp"
#include "../mwworld/cellstore.hpp" // for Doors
#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Hunt : public Task
	{
	private:

	

	public:

			
		Hunt(MWTasks::Task* lifetask);

		virtual MWWorld::Ptr update();

		virtual int getTypeId() const;

		unsigned int mStep;

		bool mInitialized = false;

		std::string mDestId;

		
	};

}








#endif