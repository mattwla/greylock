#include "subbraincircadianrhythm.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwbase/world.hpp"
#include "../mwmechanics/npcstats.hpp"
#include <memory>



MWBase::SubBrainCircadianRhythm::SubBrainCircadianRhythm(MWBase::Life * life)
{

	mOwnerLife = life;
	mSleepOnGroundBO = new BOSleepOnGround(1);
	//mGOAPNodes.push_back(mEatBO->getGOAPNode());
	GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "sleepiness", -1);
	std::shared_ptr<GOAPDesire> pDesire = std::make_shared<GOAPDesire>(desirestatus, 1);
	mSleepDesire = pDesire;
	pDesire->debugInfo = "sleep desire";
	mOwnerLife->submitDesirePtr(pDesire);
}

void MWBase::SubBrainCircadianRhythm::calculate(MWBase::Awareness * awareness)
{
	mSleepDesire->mValence = mOwnerLife->mVitals.mSleepiness;
}

std::string MWBase::SubBrainCircadianRhythm::getID()
{
	return "sbsleepiness";
}

void MWBase::SubBrainCircadianRhythm::getDebugInfo()
{

}

std::vector < std::shared_ptr < MWBase::GOAPNodeData >> MWBase::SubBrainCircadianRhythm::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	
	std::vector < std::shared_ptr < MWBase::GOAPNodeData >> results;
	
	MWBase::GOAPStatus gs;
	gs.mAmount = -1;
	gs.mStatusType = GOAPStatus::VITALS;
	gs.mExtraData = "sleepiness";

	if (status == gs)
	{

	//actually search inventory for thing to eat?
	//std::vector<std::shared_ptr<MWBase::GOAPNodeData>> results;
	MWBase::GOAPStatus statusinput(MWBase::GOAPStatus::STATUS_VOID, "", 0);
	MWBase::GOAPStatus statusoutput(MWBase::GOAPStatus::VITALS, "sleepiness", -1);
	ESM::RefNum refnum;
	refnum.unset();

	std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mSleepOnGroundBO, refnum, 1, "sleep on ground node"));
	results.push_back(node);


	}
	return results;
}



MWBase::BOSleepOnGround::BOSleepOnGround(int valence) //input urge strength
{
	
	//again a mess mwx fix me
	std::cout << "made sleep on ground BO" << std::endl;
	//MWBase::GOAPStatus statusinput(GOAPStatus::HAS_OBJECT_STATUS_IN_INVENTORY, "edible", 1);
	//std::shared_ptr<GOAPNodeData> gd(new GOAPNodeData());
	//mGOAPNodeData = gd;
	//mGOAPNodeData->mInputs.push_back(statusinput);
	//MWBase::GOAPStatus statusoutput(GOAPStatus::VITALS, "hunger", -1);
	//mGOAPNodeData->mOutputs.push_back(statusoutput);
	//mGOAPNodeData->mBehaviorObject = this;
	//mGOAPNodeData->mId = "BO EAT";

}

void MWBase::BOSleepOnGround::getDebugInfo()
{
	std::cout << "I am a sleep on ground BO" << std::endl;
}

MWBase::BOReturn MWBase::BOSleepOnGround::update(float time, MWWorld::Ptr ownerptr)
{

	if (mStopRequested)
	{
		return COMPLETE;
	}


	auto seq = ownerptr.getClass().getCreatureStats(ownerptr).getAiSequence();
	seq.clear();
	if (MWBase::Environment::get().getLifeManager()->inActiveRange(ownerptr))
		if (!MWBase::Environment::get().getMechanicsManager()->checkAnimationPlaying(ownerptr, "lay"))
			MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(ownerptr, "lay", 0, 1);


	mOwnerLife->mVitals.mSleepiness -= time / 100.0f;

	if (mOwnerLife->mVitals.mSleepiness <= 0.0)
	{
		return MWBase::COMPLETE;
	}


	return MWBase::IN_PROGRESS;
}

MWBase::BOReturn MWBase::BOSleepOnGround::start()
{
	return BOReturn::IN_PROGRESS;
}