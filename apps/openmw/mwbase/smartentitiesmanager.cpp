#include "smartentitiesmanager.hpp"
#include "../glsmartentities/bread.hpp"
#include <iostream>

void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	SmartEntityTemplate * it = new SmartEntityBreadTemplate();
	mSmartTemplateMap[it->getStringID()] = it;


}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(const MWWorld::Ptr &ptr)
{
	std::string id = ptr.getCellRef().getRefId();
	int refnum = ptr.getCellRef().getRefNum().mIndex;

	if (!mSmartTemplateMap.count(id))
		return nullptr;

	SmartEntityInstance * foundInstance = mSmartTemplateMap[id]->getInstance();

	mSmartInstanceMap[refnum] = foundInstance;

	
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

void MWBase::SmartEntitiesManager::clear()
{

	std::map<int,MWBase::SmartEntityInstance*>::iterator it = mSmartInstanceMap.begin();

	while (it != mSmartInstanceMap.end())
	{
		delete it->second;
		it++;
	}

	mSmartInstanceMap.clear();
}

std::string MWBase::SmartEntityTemplate::getStringID()
{
	return mIngameID;
}

void MWBase::SmartEntityInstance::ping()
{
	mPingCount += 1;
}

int MWBase::SmartEntityInstance::getPings()
{
	return mPingCount;
}
