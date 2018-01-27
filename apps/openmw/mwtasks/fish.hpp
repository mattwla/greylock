#ifndef GAME_TASKS_FISH_H
#define GAME_TASKS_FISH_H

#include "task.hpp"
#include "../mwworld/cellstore.hpp" // for Doors
#include "../mwbase/world.hpp"
namespace MWTasks
{

	class Fish : public Task
	{
	private:


	public:
		Fish(MWTasks::Task* lifetask);
		~Fish();
		unsigned int mStep;
		virtual MWWorld::Ptr update();
		std::string mZoneId;
		int mZoneSlotIdx;
		virtual int getTypeId() const;
		std::string mDestId;		
	};

}

#endif