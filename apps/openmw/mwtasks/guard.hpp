#ifndef GAME_TASKS_GUARD_H
#define GAME_TASKS_GUARD_h


#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Guard : public Task
	{
	private:


	public:

		Guard();

		Guard(MWTasks::Task* lifetask);

		~Guard();

		virtual MWWorld::Ptr update();

		virtual int getTypeId() const;

		unsigned int mStep;

		MWWorld::Ptr mDestination;

		int mZoneSlotIdx;

		std::string mZoneId;

		std::string mDestId;

	};

}








#endif