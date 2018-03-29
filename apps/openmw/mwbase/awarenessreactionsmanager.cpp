#include "awarenessreactionsmanager.hpp"
#include "smartentitiesmanager.hpp"
#include <iostream>

void MWBase::Awareness::refresh()
{
	MWBase::SmartInstanceMap instances = MWBase::Environment::get().getSmartEntitiesManager()->getLiveSmartInstances();

	MWBase::SmartInstanceMap::iterator it = instances.begin();
	SensoryLinkStore * newStore = new SensoryLinkStore;

	while (it != instances.end())
	{
		bool isaware = MWBase::Environment::get().getAwarenessReactionsManager()->awarenessCheck(it->second->getPtr(), mPtr);
		
		
		
		if (isaware)
		{
			newStore->addSensoryLink(MWBase::SensoryLink(it->second->getPtr()));
		}
		it++;
	}

	if (mSensoryLinkStore)
	{
		delete mSensoryLinkStore;
	}
	mSensoryLinkStore = newStore;



}

void MWBase::Awareness::getDebugInfo()
{
	std::cout << "I am aware of..." << std::endl;
	unsigned int itx = 0;
	
	while (itx < mSensoryLinkStore->mCurrentSensoryLinks.size())
	{
		std::cout << mSensoryLinkStore->mCurrentSensoryLinks[itx].mPtr.getCellRef().getRefId() << std::endl;
		itx += 1;
	}
}

MWBase::Awareness::Awareness(MWWorld::Ptr ptr)
{
	mPtr = ptr;
	mSensoryLinkStore = new SensoryLinkStore;
}

void MWBase::SensoryLinkStore::addSensoryLink(SensoryLink sensorylink)
{
	mCurrentSensoryLinks.push_back(sensorylink);
}

MWBase::SensoryLinkStore::~SensoryLinkStore()
{
	
}
