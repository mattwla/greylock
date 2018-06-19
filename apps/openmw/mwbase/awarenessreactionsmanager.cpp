#include "awarenessreactionsmanager.hpp"
#include "smartentitiesmanager.hpp"
#include <iostream>

void MWBase::Awareness::refresh()
{
	MWBase::SmartInstanceMap instances = MWBase::Environment::get().getSmartEntitiesManager()->getLiveSmartInstances();
	MWBase::SmartInstanceMap::iterator it = instances.begin();
	delete mSensoryLinksThisFrame;
	mSensoryLinksThisFrame = new SensoryLinkStore;
		
	//	new SensoryLinkStore; //mwx fix me, better to maintain one.

	

	while (it != instances.end())
	{
		bool isaware = MWBase::Environment::get().getAwarenessReactionsManager()->awarenessCheck(it->second->getPtr(), mPtr);
		if (isaware)
		{
			ESM::RefNum refnum = it->second->getPtr().getCellRef().getRefNum();
			mSensoryLinkStore->addSensoryLink(refnum, MWBase::SensoryLink(it->second->getPtr(), it->second));
			mSensoryLinksThisFrame->addSensoryLink(refnum, MWBase::SensoryLink(it->second->getPtr(), it->second));
		}
		it++;
	}


	//likely inefficient mwx fix me
	//if (mSensoryLinkStore)
	//{
	//	SensoryLinkStore * oldStore = mSensoryLinkStore;
	//	//delete mSensoryLinkStore;
	//}

	//moveSensoryLinksToMemory();

	//mSensoryLinkStore = newStore;



}

void MWBase::Awareness::getDebugInfo()
{
	std::cout << "I am aware of..." << std::endl;
	unsigned int itx = 0;
	
	for (auto i : mSensoryLinksThisFrame->mSensoryLinks)
	{
		std::cout << i.second.mSEInstance->getPtr().getCellRef().getRefId() << std::endl;
	}

	/*while (itx < mSensoryLinkStore->mCurrentSensoryLinks.size())
	{
		std::cout << mSensoryLinkStore->mCurrentSensoryLinks[itx].mSEInstance->getPtr().getCellRef().getRefId() + " " + std::to_string(mSensoryLinkStore->mCurrentSensoryLinks[itx].mSEInstance->getPtr().getCellRef().getRefNum().mIndex) << std::endl;
		itx += 1;
	}*/
}

void MWBase::Awareness::moveSensoryLinksToMemory()
{


}

MWBase::Awareness::Awareness(MWWorld::Ptr ptr)
{
	mPtr = ptr;
	mSensoryLinkStore = new SensoryLinkStore;
	mSensoryLinksThisFrame = new SensoryLinkStore;
}

MWBase::SensoryLinkStore * MWBase::Awareness::getSensoryLinkStore()
{
	return mSensoryLinkStore;
}

MWBase::SensoryLinkStore * MWBase::Awareness::getSensoryLinksThisFrame()
{
	return mSensoryLinksThisFrame;
}

bool MWBase::SensoryLinkStore::hasLinkWithStatus(std::string status)
{
	return false;
}

void MWBase::SensoryLinkStore::addSensoryLink(ESM::RefNum refnum, SensoryLink sensorylink)
{
	//mCurrentSensoryLinks[refnum] = (sensorylink);

	mSensoryLinks[refnum] = sensorylink;

	

}

void MWBase::SensoryLinkStore::removeSensoryLink(ESM::RefNum refnum)
{
	mSensoryLinks.erase(refnum);
}

MWBase::SensoryLinkStore::~SensoryLinkStore()
{
	
}
