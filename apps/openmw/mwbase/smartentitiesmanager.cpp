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

void MWBase::SmartEntitiesManager::loadSmartEntityInstance(std::string type, int contentnum, int index, std::string savestate)
{
	ESM::RefNum refnum;
	refnum.mIndex = index;
	refnum.mContentFile = contentnum;
	SmartEntityInstance * foundInstance = mSmartTemplateMap[type]->loadInstance(type, refnum, savestate); //template should contain save and load logic.
	mSmartInstanceMap[refnum] = foundInstance;
	//type and refnum are generic, but "pings" should instead of "statestring"
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
				if (idx == 4)
				{
					loadSmartEntityInstance(cache[2], std::stoi(cache[0]), std::stoi(cache[1]), cache[3]);
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
	ESM::RefNum refnum;
	while (it != mSmartInstanceMap.end())
	{
		refnum = it->first;
		int content = refnum.mContentFile;
		int index = refnum.mIndex;
		//First store generic ref info, than info specific to instance
		std::string savestring;
		savestring += std::to_string(content);
		savestring += ",";
		savestring += std::to_string(index);
		savestring += ",";
		savestring += it->second->getRefId();
		savestring += ",";
		savestring += it->second->getSaveString();
		//savestring += std::to_string(it->second->getPings());
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
		return mSmartInstanceMap[ptr.getCellRef().getRefNum()];
	std::string id = ptr.getCellRef().getRefId();
	ESM::RefNum refnum = ptr.getCellRef().getRefNum();
	if (!hasSmartTemplate(id)) //Is there a template for this object? if not return nothing
		return nullptr;
	SmartEntityInstance * newInstance = mSmartTemplateMap[id]->getInstance(ptr);
	mSmartInstanceMap[refnum] = newInstance;
	return newInstance;
}

MWBase::SmartEntityInstance * MWBase::SmartEntitiesManager::getSmartEntityInstance(std::string id, ESM::RefNum refNum)
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

	ESM::RefNum refnum = cellref.mRefNum;

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

	
	
	ESM::RefNum refNum = livecellref->mRef.getRefNum();
	int contentNum = livecellref->mRef.getRefNum().mContentFile;
	if (contentNum == -1 && refNum.mIndex == 0) //dynamically generated, give it a valid refnum
	{
		livecellref->mRef.setRefNum(mRuntimeRefNumTicker++);
		refNum = livecellref->mRef.getRefNum();
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
	ESM::RefNum refnum = ptr.getCellRef().getRefNum();
	mSmartInstancesInScene[refnum] = instance;
}

bool MWBase::SmartEntitiesManager::isInstanceInScene(const MWWorld::Ptr & ptr)
{
	ESM::RefNum refnum = ptr.getBase()->mRef.getRefNum();
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
	ESM::RefNum refnum = ptr.getCellRef().getRefNum();
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
		ESM::RefNum refnum = it->getCellRef().getRefNum();
		mSmartInstancesInScene.erase(refnum);
	}
}

bool MWBase::SmartEntitiesManager::hasSmartInstance(const MWWorld::Ptr & ptr)
{
		ESM::RefNum refnum = ptr.getBase()->mRef.getRefNum();
		if (!mSmartInstanceMap.count(refnum))
			return false;
		else
			return true;
}

bool MWBase::SmartEntitiesManager::hasSmartInstance(ESM::RefNum refnum)
{

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
	std::map<ESM::RefNum, MWBase::SmartEntityInstance*>::iterator it = mSmartInstancesInScene.begin();
	while (it != mSmartInstancesInScene.end())
	{
		std::cout << std::to_string(it->first.mIndex) + " " + it->second->getRefId() << std::endl;
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
	std::map<ESM::RefNum,MWBase::SmartEntityInstance*>::iterator it = mSmartInstanceMap.begin();
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
