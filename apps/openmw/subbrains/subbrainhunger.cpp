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
	GOAPStatus desirestatus(MWBase::VITALS, "hunger", -1);
	std::shared_ptr<GOAPDesire> pDesire = std::make_shared<GOAPDesire>(desirestatus, 1);
	mHungerDesire = pDesire;
	pDesire->debugInfo = "hunger desire";
	mOwnerLife->submitDesirePtr(pDesire);
}

void MWBase::SubBrainHunger::calculate(MWBase::Awareness * awareness)
{
	mHungerDesire->mIntensity = mOwnerLife->mVitals.mHunger;
	//if (mHungerDesire->mIsIntention)
	//{
	//	if (!mOwnerLife->mCurrentSpeech)
	//		mOwnerLife->say("I'm hungry");
	//}
}

std::string MWBase::SubBrainHunger::getID()
{
	return "sbhunger";
}

void MWBase::SubBrainHunger::getDebugInfo()
{

}

std::vector < std::shared_ptr < MWBase::GOAPNodeData >> MWBase::SubBrainHunger::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{
	
	std::vector < std::shared_ptr < MWBase::GOAPNodeData >> results;
	
	MWBase::GOAPStatus gs;
	gs.mAmount = -1;
	gs.mStatusType = MWBase::VITALS;
	gs.mExtraData = "hunger";

	if (status == gs)
	{

	//actually search inventory for thing to eat?
	//std::vector<std::shared_ptr<MWBase::GOAPNodeData>> results;
	MWBase::GOAPStatus statusinput(MWBase::HAS_OBJECT_STATUS_IN_INVENTORY, "edible", 1);
	MWBase::GOAPStatus statusoutput(MWBase::VITALS, "hunger", -1);
	ESM::RefNum refnum;
	refnum.unset();

	std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mEatBO, refnum, 1, "eat bo node"));
	results.push_back(node);


	}
	return results;
}



MWBase::BOEat::BOEat(int intensity) //input urge strength
{

	std::cout << "made eat BO" << std::endl;
}

void MWBase::BOEat::getDebugInfo()
{
	std::cout << "I am a eat BO" << std::endl;
}

MWBase::BOReturn MWBase::BOEat::update(float time, MWWorld::Ptr ownerptr)
{
	if (mStopRequested)
	{
		return MWBase::COMPLETE;
	}

	MWBase::SmartEntityInstance * sei = mOwnerLife->getSEIWithStatusFromInventory("edible");
	if (!sei)
	{
		std::cout << "could not find anything edible in inventory" << std::endl;
		return MWBase::FAILED;
	}

	//std::cout << "yum" << std::endl;
	//mOwnerLife->mVitals.mHunger = 0.0f;
	if (mOwnerLife->mCurrentSpeech == 0)
	{
		mOwnerLife->say("Yum");
	}
	sei->use(mOwnerLife);
	//MWBase::Environment::get().getMechanicsManager()->playAnimationGroup(ownerptr, "wave", 0, 1);
	return MWBase::COMPLETE;
}

MWBase::BOReturn MWBase::BOEat::start()
{
	return BOReturn::IN_PROGRESS;
}
