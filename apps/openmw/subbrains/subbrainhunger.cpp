#include "subbrainhunger.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"



MWBase::SubBrainHunger::SubBrainHunger(MWBase::Life * life)
{
	mOwnerLife = life;
	mEatBO = new BOEat(1);
	mGOAPNodes.push_back(mEatBO->getGOAPNode());
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

		GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "hunger", -1);
		GOAPDesire desire(desirestatus, 1);
		mGOAPDesires.push_back(desire);
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
	std::shared_ptr<GOAPData> gd(new GOAPData());
	mGOAPData = gd;
	mGOAPData->mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::VITALS, "hunger", -1);
	mGOAPData->mOutputs.push_back(statusoutput);
	mGOAPData->mBehaviorObject = this;
	mGOAPData->mId = "BO EAT";
}

void MWBase::BOEat::getDebugInfo()
{
	std::cout << "I am a eat BO" << std::endl;
}

MWBase::BOReturn MWBase::BOEat::update(float time, MWWorld::Ptr ownerptr)
{
	return MWBase::IN_PROGRESS;
}
