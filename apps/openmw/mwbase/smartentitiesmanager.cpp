#include "smartentitiesmanager.hpp"
#include "../glsmartentities/bread.hpp"
#include <iostream>
#include <boost/filesystem/fstream.hpp>

void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	SmartEntityTemplate * it = new SmartEntityBreadTemplate();
	mSmartTemplateMap[it->getStringID()] = it;


}

void MWBase::SmartEntitiesManager::saveGame(boost::filesystem::path path)
{
	std::string savestring;
	std::string path_s = path.string();
	path_s += "EMS";
	path = path_s;

	std::map<int, MWBase::SmartEntityInstance*>::iterator it = mSmartInstanceMap.begin();

	while (it != mSmartInstanceMap.end())
	{
		savestring += std::to_string(it->second->getPings());

		it++;
	}



	boost::filesystem::ofstream filestream(path, std::ios::binary);
	filestream << savestring;
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
