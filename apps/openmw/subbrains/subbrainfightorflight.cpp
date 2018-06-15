#include "subbrainfightorflight.hpp"
#include "../mwbase/lifemanager.hpp"

namespace MWBase {

	SubBrainFightOrFlight::SubBrainFightOrFlight(MWBase::Life * life)
	{
	}

	void MWBase::SubBrainFightOrFlight::calculate(MWBase::Awareness * awareness)
	{
	}

	std::string MWBase::SubBrainFightOrFlight::getID()
	{
		return std::string();
	}

	void MWBase::SubBrainFightOrFlight::getDebugInfo()
	{
	}

	std::vector<std::shared_ptr<GOAPNodeData>> MWBase::SubBrainFightOrFlight::getMatchingBehaviorObjects(MWBase::GOAPStatus)
	{
		return std::vector<std::shared_ptr<GOAPNodeData>>();
	}

	int SubBrainFightOrFlight::getCost(SmartEntityInstance * sei)
	{
		return 0;
	}

	BOFight::BOFight()
	{
	}

	BOReturn MWBase::BOFight::update(float time, MWWorld::Ptr ownerptr)
	{
		return BOReturn();
	}

	void MWBase::BOFight::getDebugInfo()
	{
	}

	BOReturn MWBase::BOFight::start()
	{
		return BOReturn();
	}

}