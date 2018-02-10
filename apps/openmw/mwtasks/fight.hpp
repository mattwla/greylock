#ifndef GAME_TASKS_FIGHT_H
#define GAME_TASKS_FIGHT_H


#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Fight : public Task
	{
	private:


	public:

		Fight();

		Fight(MWTasks::Task* lifetask, MWWorld::Ptr mTarget);

		~Fight();

		MWWorld::Ptr mTarget;

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