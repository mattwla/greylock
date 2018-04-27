#include "subbrainhunger.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <memory>



MWBase::SubBrainHunger::SubBrainHunger(MWBase::Life * life)
{
	mOwnerLife = life;
	mEatBO = new BOEat(1);
	mGOAPNodes.push_back(mEatBO->getGOAPNode());
	GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "hunger", -1);
	std::shared_ptr<GOAPDesire> pDesire = std::make_shared<GOAPDesire>(desirestatus, 1);
	mHungerDesire = pDesire;
	mOwnerLife->submitDesirePtr(pDesire);
}

void MWBase::SubBrainHunger::calculate(MWBase::Awareness * awareness)
{
	mGOAPDesires.clear();
	mWorldState.clear();
	if (mOwnerLife->mVitals.mHunger > 100.0f)
	{
		/*WorldstateAtom ws("hungry", true);
		mWorldState.push_back(ws);*/

		//A list of desires that is maintained?
		
		
		
		//mHungerDesire = pDesire;


	
		//GOAPDesire desire(desirestatus, 1);
		//desire.debugInfo = "eat desire";
		//mGOAPDesires.push_back(desire);
	}
}

std::string MWBase::SubBrainHunger::getID()
{
	return "sbhunger";
}

void MWBase::SubBrainHunger::getDebugInfo()
{

}

MWBase::BOEat::BOEat(int valence) //input urge strength
{
	
	//again a mess mwx fix me
	std::cout << "made eat BO" << std::endl;
	MWBase::GOAPStatus statusinput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "edible", 1);
	std::shared_ptr<GOAPNodeData> gd(new GOAPNodeData());
	mGOAPNodeData = gd;
	mGOAPNodeData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::VITALS, "hunger", -1);
	mGOAPNodeData->mOutputs.push_back(statusoutput);
	mGOAPNodeData->mBehaviorObject = this;
	mGOAPNodeData->mId = "BO EAT";

}

void MWBase::BOEat::getDebugInfo()
{
	std::cout << "I am a eat BO" << std::endl;
}

MWBase::BOReturn MWBase::BOEat::update(float time, MWWorld::Ptr ownerptr)
{

	MWBase::SmartEntityInstance * sei = mOwnerLife->getSEIWithStatusFromInventory("edible");
	if (!sei)
	{
		std::cout << "could not find anything edible in inventory" << std::endl;
		return MWBase::FAILED;
	}

	//std::cout << "yum" << std::endl;
	//mOwnerLife->mVitals.mHunger = 0.0f;
	sei->use(mOwnerLife);
	//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(ownerptr, "wave", 0, 1);
	return MWBase::COMPLETE;
}

MWBase::BOReturn MWBase::BOEat::start()
{
	return BOReturn::IN_PROGRESS;
}
