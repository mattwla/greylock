#include "lifemanager.hpp"
#include <iostream>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cells.hpp" 
#include "../subbrains/subbrain.hpp"
#include "../subbrains/subbrainhunger.hpp"

void MWBase::Life::getDebugInfo()
{
	std::cout << mId << std::endl;
	std::cout << std::to_string(mRefNum) << std::endl;
	std::cout << "Hunger: " + std::to_string(mVitals.mHunger) << std::endl;
	//std::cout << "original cell: " + mOwnerCell->getCell  << std::endl;
}

void MWBase::Life::update(float duration)
{
	metabolize(duration);
	mAwareness->refresh();
	mSubBrainsManager->calculate(mAwareness);
}

void MWBase::Life::metabolize(float duration)
{
	mVitals.mHunger += duration / 100.0f;
}

void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	
	MWBase::SensoryLinkStore * store = awareness->getSensoryLinkStore();

	for (std::vector<MWBase::SubBrain*>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
		(*it)->calculate(awareness);
	}

}

MWBase::SubBrainsManager::SubBrainsManager(MWBase::Life * life)
{
	SubBrain * sb = new SubBrainHunger(life);
	mSubBrains.push_back(sb);
}
