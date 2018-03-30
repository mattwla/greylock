#include "smartentitiesmanager.hpp"
#include "../glsmartentities/bread.hpp"
#include "../mwworld/worldimp.cpp"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>
#include <iterator>
#include <algorithm>
#include <map>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cellvisitors.hpp"


void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	SmartEntityTemplate * it = new SmartEntityBreadTemplate();
	mSmartTemplateMap[it->getStringID()] = it;


}

void MWBase::SmartEntitiesManager::loadSmartEntityInstance(std::string type, int refnum, int pings)
{


	SmartEntityInstance * foundInstance = mSmartTemplateMap[type]->loadInstance(type, refnum, pings);

	mSmartInstanceMap[refnum] = foundInstance;

	//std::cout << "loaded an instance" << std::endl;
}

void MWBase::SmartEntitiesManager::loadGame(boost::filesystem::path path)
{
		std::ifstream in(path.string() + "EMS");

		if (!in.is_open())
			std::cout << "NOT OPEN" << std::endl;
		else
			std::cout << "OPEN" << std::endl;
	
		
		std::string line;
	
		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;
		
	
		while (getline(in, line))
		{
			Tokenizer tok(line);
			int idx = 0;
			std::vector<std::string> cache;
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
			{
				if (*it == "v1")
					continue;

				idx += 1;
				cache.push_back(*it);
				if (idx == 3)
				{
					loadSmartEntityInstance(cache[1], std::stoi(cache[0]), std::stoi(cache[2]));
					idx = 0;
					cache.clear();
				}
				

			}
		}
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

	filestream << "v1" << std::endl;


	std::vector<std::string>::iterator itS = serializedinstances.begin();
	while (itS != serializedinstances.end())
	{
		filestream << *itS << std::endl;
		//filestream << std::endl << std::endl;
		//
		itS++;
	}


	
	
	//filestream << savestring;
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(const MWWorld::Ptr &ptr)
{

	//check if it already has one
	if (hasSmartInstance(ptr))
		return mSmartInstanceMap[ptr.getCellRef().getRefNum().mIndex];
	
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

void MWBase::SmartEntitiesManager::addSmartInstanceToScene(const MWWorld::Ptr & ptr)
{
	bool alreadyinscene = isInstanceInScene(ptr);
	if (alreadyinscene)
	{
		std::cout << "error: smart instance requested to be added to scene, was already in scene" << std::endl;
		std::cout << "id is: " + ptr.getCellRef().getRefId() << std::endl;
		return;
	}

	//Do we assume one is already built?
	SmartEntityInstance * instance = getSmartEntityInstance(ptr);
	int refnum = ptr.getCellRef().getRefNum().mIndex;
	
	mSmartInstancesInScene[refnum] = instance;

}

bool MWBase::SmartEntitiesManager::isInstanceInScene(const MWWorld::Ptr & ptr)
{
	int refnum = ptr.getBase()->mRef.getRefNum().mIndex;
	if (!mSmartInstancesInScene.count(refnum))
		return false;
	else
		return true;
	
}

void MWBase::SmartEntitiesManager::removeSmartInstanceFromScene(const MWWorld::Ptr & ptr)
{
	bool alreadyinscene = isInstanceInScene(ptr);
	if (!alreadyinscene)
	{
		std::cout << "error: smart instance requested to be removed to scene, was not in scene" << std::endl;
		std::cout << "id is: " + ptr.getCellRef().getRefId() << std::endl;
		return;
	}

	//Do we assume one is already built?
	int refnum = ptr.getCellRef().getRefNum().mIndex;
	mSmartInstancesInScene.erase(refnum);

}

void MWBase::SmartEntitiesManager::removeSmartInstancesFromSceneViaCell(MWWorld::CellStore * cellStore)
{

	//std::cout << "removing SMART INTS" << std::endl;
	MWWorld::ListObjectsVisitor visitor;
	cellStore->forEach(visitor);

	for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
	{
		//std::cout << it->getCellRef().getRefId() << std::endl;
		if (isInstanceInScene(*it))
		{
			std::cout << "tried to remove " + it->getCellRef().getRefId() << std::endl;
		}
		int refnum = it->getCellRef().getRefNum().mIndex;

		mSmartInstancesInScene.erase(refnum);
	}

}

bool MWBase::SmartEntitiesManager::hasSmartInstance(const MWWorld::Ptr & ptr)
{
	
		int refnum = ptr.getBase()->mRef.getRefNum().mIndex;
		if (!mSmartInstanceMap.count(refnum))
			return false;
		else
			return true;
	
}

MWBase::SmartInstanceMap & MWBase::SmartEntitiesManager::getLiveSmartInstances()
{
	return mSmartInstancesInScene;
	
	// TODO: insert return statement here
}

void MWBase::SmartEntitiesManager::outputInSceneInstancesToLog()
{
	std::map<int, MWBase::SmartEntityInstance*>::iterator it = mSmartInstancesInScene.begin();

	while (it != mSmartInstancesInScene.end())
	{
		std::cout << std::to_string(it->first) + " " + it->second->getRefId() << std::endl;
		it++;
	}

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

MWWorld::Ptr & MWBase::SmartEntityInstance::getPtr()
{
	
	
	
	return mPtr;
}

void MWBase::SmartEntityInstance::updatePtr(MWWorld::Ptr & ptr)
{

	mPtr = ptr;
}
