#include "subbrainfightorflight.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aifollow.hpp"
#include "../mwmechanics/aicombat.hpp"
#include "../mwworld/class.hpp"

namespace MWBase {

	SubBrainFightOrFlight::SubBrainFightOrFlight(MWBase::Life * life)
	{
		mOwnerLife = life;
		mFightBO = new BOFight();
		mFleeBO = new BOFlee();
	}

	void MWBase::SubBrainFightOrFlight::calculate(MWBase::Awareness * awareness)
	{
	}

	std::string MWBase::SubBrainFightOrFlight::getID()
	{
		return "fight or flight sb";
	}

	void MWBase::SubBrainFightOrFlight::getDebugInfo()
	{

	}

	std::vector<std::shared_ptr<GOAPNodeData>> MWBase::SubBrainFightOrFlight::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
	{
		std::vector<std::shared_ptr<GOAPNodeData>> nodes;
		
		if (status.mStatusType == VITALS && status.mExtraData == "health" && status.mAmount == -1)
		{
			status.mTarget;

			MWBase::GOAPStatus statusinput(MWBase::STATUS_VOID, "", 0);
			MWBase::GOAPStatus statusoutput(status.mStatusType, status.mExtraData, status.mAmount);
			std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mFleeBO, status.mTarget->getRefNum(), 1, "Fight"));
			node->mCost = 1;
			nodes.push_back(node);
		}




		return nodes;
	}

	int SubBrainFightOrFlight::getCost(SmartEntityInstance * sei)
	{
		return 1;
	}

	BOFight::BOFight()
	{
		
	}

	BOReturn MWBase::BOFight::update(float time, MWWorld::Ptr ownerptr)
	{
		if (mStopRequested)
			return STOPPED;

		//std::cout << "startin a fight" << std::endl;
		return IN_PROGRESS;
	}

	void MWBase::BOFight::getDebugInfo()
	{
	}

	BOReturn MWBase::BOFight::start()
	{
		auto ownerPtr = mOwnerLife->mPtr;
		MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
		seq.clear();
		seq.stack(MWMechanics::AiCombat(mSEITarget->getPtr()), mOwnerLife->mPtr);
		std::cout << "startin a fight" << std::endl;
		return IN_PROGRESS;
	}

	bool BOFight::stop()
	{
		auto ownerPtr = mOwnerLife->mPtr;
		MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
		seq.clear();
		mStopRequested = true;
		return true;
	}

	BOFlee::BOFlee()
	{
	}

	BOReturn BOFlee::update(float time, MWWorld::Ptr ownerptr)
	{
		std::cout << "fleeing" << std::endl;

		if(mStopRequested)
			return STOPPED;

		//std::cout << "startin a fight" << std::endl;
		return IN_PROGRESS;
	}

	BOReturn BOFlee::start()
	{

		MWBase::Environment::get().getSmartEntitiesManager()->getSmartEntityInstance(mOwnerLife->mPtr)->getStatusManager()->giveStatus(MWBase::Fleeing);
		auto ownerPtr = mOwnerLife->mPtr;
		MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
		seq.clear();
		seq.stack(MWMechanics::AiCombat(mSEITarget->getPtr()), mOwnerLife->mPtr);
		std::cout << "startin a flee" << std::endl;
		return IN_PROGRESS;

		//return BOReturn();
	}

	bool BOFlee::stop()
	{
		auto ownerPtr = mOwnerLife->mPtr;
		MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
		seq.clear();
		mStopRequested = true;
		return true;
	}

}