#include "smartentitiesmanagerimp.hpp"
#include <String>
#include <iostream>

GLSmartEntitiesManager::SmartEntitiesManager::SmartEntitiesManager()
{
	std::cout << "made SEManager" << std::endl;

	MWBase::SmartEntityTemplate* bread = new MWBase::SmartEntityTemplate;
	mTemplateList["god_loaf"] = bread;
}

MWBase::SmartEntityInstance * GLSmartEntitiesManager::SmartEntitiesManager::getSmartEntityInstance(std::string id)
{
	return nullptr;
}
