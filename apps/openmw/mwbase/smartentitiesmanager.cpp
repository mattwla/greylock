#include "smartentitiesmanager.hpp"
#include "../glsmartentities/bread.hpp"
#include <iostream>

void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	std::vector<std::string> templateList; //a list of templates.

}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(std::string id)
{
	return nullptr;
}

MWBase::SmartEntitiesManager::SmartEntitiesManager()
{
	std::cout << "=====>Built SEManager<======" << std::endl;

	std::cout << "Initializing templates" << std::endl;
}
