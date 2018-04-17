#include "subbrain.hpp"

std::vector<MWBase::GOAPData*> MWBase::SubBrain::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	std::vector<GOAPData*> matchingnodes;
	
	
	for (std::vector<MWBase::GOAPData*>::iterator it = mGOAPNodes.begin(); it != mGOAPNodes.end(); it++)
	{
		if ((*it)->mOutputs[0] == status)
		{
			//std::cout << "found matching BO node" << std::endl;
			matchingnodes.push_back(*it);
		}
	}
	
	mGOAPNodes;

	return matchingnodes;
}

bool MWBase::GOAPStatus::operator==(GOAPStatus status)
{
	if ((mStatusType == status.mStatusType) && (mExtraData == status.mExtraData))
		return true;
	else
		return false;
}
