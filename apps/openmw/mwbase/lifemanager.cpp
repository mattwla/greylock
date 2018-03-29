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
