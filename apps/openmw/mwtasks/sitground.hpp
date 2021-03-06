#ifndef GAME_TASKS_SITGROUND_H
#define GAME_TASKS_SITGROUND_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Sitground : public Task
	{
	private:

		
	public:

		Sitground();

		Sitground(MWTasks::Task* lifetask);

		~Sitground();

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