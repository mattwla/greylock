#include "subbrain.hpp"

std::vector<MWBase::BehaviorObject> MWBase::SubBrain::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	for (std::vector<MWBase::GOAPData*>::iterator it = mGOAPNodes.begin(); it != mGOAPNodes.end(); it++)
	{
		if ((*it)->mOutputs[0] == status)
			std::cout << "found matching BO node" << std::endl;
	}
	
	mGOAPNodes;

	return std::vector<BehaviorObject>();
}

bool MWBase::GOAPStatus::operator==(GOAPStatus status)
{
	if ((mStatusType == status.mStatusType) && (mExtraData == status.mExtraData))
		return true;
	else
		return false;
}
