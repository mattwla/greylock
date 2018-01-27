#include "task.hpp"
#include <string>

MWTasks::Task::Task()
{
}

MWTasks::Task::Task(MWWorld::Ptr npcPtr)
{
}

//MWTasks::Task::Task(std::string npcId) : mNpcId(npcId)
//{
//}

MWTasks::Task::~Task()
{
}

MWWorld::Ptr MWTasks::Task::update()
{
	return MWWorld::Ptr();
}
