#include "lifemanager.hpp"
#include <iostream>
#include "../mwworld/cellstore.hpp"
#include "../mwworld/cells.hpp"

void MWBase::Life::getDebugInfo()
{
	std::cout << mId << std::endl;
	std::cout << std::to_string(mRefNum) << std::endl;
	//std::cout << "original cell: " + mOwnerCell->getCell  << std::endl;
}

void MWBase::SubBrainsManager::calculate(MWBase::Awareness * awareness)
{
	
	MWBase::SensoryLinkStore * store = awareness->getSensoryLinkStore();

	for (std::vector<MWBase::SubBrain>::iterator it = mSubBrains.begin(); it != mSubBrains.end(); ++it)
	{
	
	}

}
