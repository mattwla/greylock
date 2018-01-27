#ifndef GAME_TASKS_GET_H
#define GAME_TASKS_GET_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Get : public Task
	{
	private:

		bool pickupItem();

	public:

		Get(std::string destId, MWTasks::Task* lifetask);

		unsigned int mStep;

		virtual MWWorld::Ptr update();

		virtual int getTypeId() const;

		std::string mDestId;

	

	};

}



#endif