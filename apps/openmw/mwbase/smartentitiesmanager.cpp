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
#include "../mwworld/livecellref.hpp"


void MWBase::SmartEntitiesManager::gatherSmartEntityTemplates()
{
	SmartEntityTemplate * it = new SmartEntityBreadTemplate();
	mSmartTemplateMap[it->getStringID()] = it;
}

void MWBase::SmartEntitiesManager::loadSmartEntityInstance(std::string type, int refnum, int pings)
{
	SmartEntityInstance * foundInstance = mSmartTemplateMap[type]->loadInstance(type, refnum, pings); //template should contain save and load logic.
	mSmartInstanceMap[refnum] = foundInstance;
}

void MWBase::SmartEntitiesManager::loadGame(boost::filesystem::path path)
{
		
	//get runtime id count
	std::ifstream in(path.string() + "EMS");

		if (!in.is_open())
			std::cout << "NOT OPEN" << std::endl;
		else
			std::cout << "OPEN" << std::endl;
	
		std::string line;
	
		typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

		bool haveParsedRefNumTicker = false;
		
		while (getline(in, line))
		{
			Tokenizer tok(line);
			int idx = 0;
			std::vector<std::string> cache;
			for (Tokenizer::iterator it(tok.begin()), end(tok.end()); it != end; ++it) //iterate through the line, values seperated by commas
			{
				if (*it == "v1")
					continue;
				if (!haveParsedRefNumTicker)
				{
					mRuntimeRefNumTicker = std::stoi(*it);
					haveParsedRefNumTicker = true;
					continue;
				}


				idx += 1;
				cache.push_back(*it);
				if (idx == 3)
				{
					loadSmartEntityInstance(cache[1], std::stoi(cache[0]), std::stoi(cache[2]));
					idx = 0;
					cache.clear(); //temporary and messy logic
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
	filestream << std::to_string(mRuntimeRefNumTicker) << std::endl;
	std::vector<std::string>::iterator itS = serializedinstances.begin();
	while (itS != serializedinstances.end())
	{
		filestream << *itS << std::endl;
		itS++;
	}
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(const MWWorld::Ptr &ptr)
{
	//check if it already has one
	if (hasSmartInstance(ptr))
		return mSmartInstanceMap[ptr.getCellRef().getRefNum().mIndex];
	std::string id = ptr.getCellRef().getRefId();
	int refnum = ptr.getCellRef().getRefNum().mIndex;
	if (!hasSmartTemplate(id)) //Is there a template for this object? if not return nothing
		return nullptr;
	SmartEntityInstance * newInstance = mSmartTemplateMap[id]->getInstance(ptr);
	mSmartInstanceMap[refnum] = newInstance;
	return newInstance;
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(std::string id, int refNum)
{
	//std::cout << ">>>> checking... " + id << std::endl;
	
	//check if it already has one
	if (hasSmartInstance(refNum))
		return mSmartInstanceMap[refNum];
	if (!hasSmartTemplate(id)) //Is there a template for this object? if not return nothing
	{
		//std::cout << "returned null" << std::endl;
		return nullptr;
	}
	SmartEntityInstance * newInstance = mSmartTemplateMap[id]->getInstance(id, refNum);
	mSmartInstanceMap[refNum] = newInstance;
	return newInstance;
}

void MWBase::SmartEntitiesManager::registerHomeCell(const ESM::CellRef & cellref, const ESM::Cell * cell)
{

	int refnum = cellref.mRefNum.mIndex;

	if (!hasSmartInstance(refnum))
		return;

	SmartEntityInstance * instance = mSmartInstanceMap[refnum];
	instance->registerHomeCell(cell);
	//instance->
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::initializeInstFromLiveCellRef(MWWorld::LiveCellRefBase * livecellref)
{



	std::string id = livecellref->mRef.getRefId();
	if (id == "")
		return nullptr;
	
	if (!hasSmartTemplate(id)) //Is there a template for this object? if not return nothing
	{
		//std::cout << "returned null" << std::endl;
		return nullptr;
	}

	
	
	int refNum = livecellref->mRef.getRefNum().mIndex;
	int contentNum = livecellref->mRef.getRefNum().mContentFile;
	if (refNum == 0)
	{
		livecellref->mRef.setRefNum(mRuntimeRefNumTicker++);
		refNum = livecellref->mRef.getRefNum().mIndex;
	}

	//std::cout << ">>>> checking... " + id << std::endl;

	//check if it already has one
	if (hasSmartInstance(refNum))
		return mSmartInstanceMap[refNum];

	
	SmartEntityInstance * newInstance = mSmartTemplateMap[id]->getInstance(id, refNum);
	mSmartInstanceMap[refNum] = newInstance;
	return newInstance;
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
	int refnum = ptr.getCellRef().getRefNum().mIndex;
	mSmartInstancesInScene.erase(refnum);
}

void MWBase::SmartEntitiesManager::removeSmartInstancesFromSceneViaCell(MWWorld::CellStore * cellStore)
{
	MWWorld::ListObjectsVisitor visitor;
	cellStore->forEach(visitor);
	for (std::vector<MWWorld::Ptr>::iterator it = visitor.mObjects.begin(); it != visitor.mObjects.end(); ++it)
	{
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

bool MWBase::SmartEntitiesManager::hasSmartInstance(int refnum)
{
	if (refnum == 0)
		return false;

	if (!mSmartInstanceMap.count(refnum))
		return false;
	else
		return true;
}

bool MWBase::SmartEntitiesManager::hasSmartTemplate(std::string id)
{
	if (!mSmartTemplateMap.count(id)) //Is there a template for this object? if not return nothing
		return false;
	else
		return true;
}

MWBase::SmartInstanceMap & MWBase::SmartEntitiesManager::getLiveSmartInstances()
{
	return mSmartInstancesInScene;
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

MWBase::SmartEntitiesManager::SmartEntitiesManager() :
	mSmartInstanceMap()
{
	mRuntimeRefNumTicker = 0;
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
	mSmartInstancesInScene.clear();
}

std::string MWBase::SmartEntityTemplate::getStringID()
{
	return mIngameID;
}

void MWBase::SmartEntityInstance::ping()
{
	mPingCount += 1;
}

void MWBase::SmartEntityInstance::debugInfo()
{
	std::cout << mHomeCellX << std::endl;
	std::cout << mHomeCellY << std::endl;
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
	return mPtr; //one day, build one dynamically?
}

void MWBase::SmartEntityInstance::updatePtr(MWWorld::Ptr ptr)
{
	mPtr = ptr;
}

void MWBase::SmartEntityInstance::registerHomeCell(const ESM::Cell * cell)
{
	mHomeCellIsExterior = cell->isExterior();
	mHomeCellName = cell->mName;
	mHomeCellX = cell->getGridX();
	mHomeCellY = cell->getGridY();
}
