#ifndef GAME_TASKS_SLEEP_H
#define GAME_TASKS_SLEEP_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Sleep : public Task
	{
	private:


	public:
		Sleep(MWTasks::Task* lifetask);
		unsigned int mStep;
		virtual MWWorld::Ptr update();
		virtual int getTypeId() const;
		std::string mDestId;
		
	};

}





#endif