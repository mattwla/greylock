#include "smartentitiesmanager.hpp"
#include "../glsmartentities/bread.hpp"
#include <iostream>

void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	SmartEntityTemplate * it = new SmartEntityBreadTemplate();
	mSmartTemplateMap[it->getStringID()] = it;


}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(std::string id)
{

	if (!mSmartTemplateMap.count(id))
		return nullptr;

	SmartEntityInstance * foundInstance = mSmartTemplateMap[id]->getInstance();
	
	if (foundInstance)
		return foundInstance;
	else
		return nullptr;
	
}

MWBase::SmartEntitiesManager::SmartEntitiesManager()
{
	std::cout << "=====>Built SEManager<======" << std::endl;

	std::cout << "Initializing templates" << std::endl;

	gatherSmartEntityTemplates();
}

std::string MWBase::SmartEntityTemplate::getStringID()
{
	return mIngameID;
}
