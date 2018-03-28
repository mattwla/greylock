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
	
	std::string path_s = path.string();
	path_s += "EMS";
	path = path_s;

	SmartInstanceMap::iterator it = mSmartInstanceMap.begin();

	std::vector<std::string>serializedinstances;

	while (it != mSmartInstanceMap.end())
	{
		std::string savestring;
		savestring += std::to_string(it->first);
		savestring += ",";
		savestring += it->second->getRefId();
		savestring += ",";
		savestring += std::to_string(it->second->getPings());
		
		serializedinstances.push_back(savestring);

		it++;
	}

	boost::filesystem::ofstream filestream(path, std::ios::binary);


	std::vector<std::string>::iterator itS = serializedinstances.begin();
	while (itS != serializedinstances.end())
	{
		filestream << *itS + "\n";
		//filestream << std::endl << std::endl;
		//
		itS++;
	}


	
	
	//filestream << savestring;
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(const MWWorld::Ptr &ptr)
{
	
	std::string id = ptr.getCellRef().getRefId();
	int refnum = ptr.getCellRef().getRefNum().mIndex;

	if (!mSmartTemplateMap.count(id))
		return nullptr;

	SmartEntityInstance * foundInstance = mSmartTemplateMap[id]->getInstance(ptr);

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

std::string MWBase::SmartEntityInstance::getRefId()
{
	return mRefId;
}
