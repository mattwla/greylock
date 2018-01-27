#ifndef GAME_TASKS_DANCE_H
#define GAME_TASKS_DANCE_H

#include "task.hpp"

#include "../mwworld/cellstore.hpp" // for Doors


#include "../mwbase/world.hpp"

namespace MWTasks
{

	class Dance : public Task
	{
	private:


	public:

		Dance(MWTasks::Task* lifetask);

		~Dance();

		unsigned int mStep;

		virtual MWWorld::Ptr update();

		int mZoneSlotIdx;

		std::string mZoneId;

		virtual int getTypeId() const;

		std::string mDestId;

		bool mReadyForUpdate;

	};

}








#endif