#include "lifemanagerimp.hpp"
#include "../mwbase/tasksmanager.hpp"


GLLifeManager::LifeManager::LifeManager()
{
}

void GLLifeManager::LifeManager::update(float duration, bool paused)
{

	MWBase::Environment::get().getTasksManager()->update(duration, paused);
}

void GLLifeManager::LifeManager::initialize()
{
}
