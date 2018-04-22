#include "subbrain.hpp"
#include <memory>

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrain::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	std::vector<std::shared_ptr<GOAPNodeData>> matchingnodes;
	
	
	for (std::vector<std::shared_ptr<MWBase::GOAPNodeData>>::iterator it = mGOAPNodes.begin(); it != mGOAPNodes.end(); it++)
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
