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
		//std::cout << "HANGRY!!" << std::endl;
		/*if (mDesires.size() < 1)*/
		{
			//BehaviorObject * bo = new BOEat(1);
			
			WorldstateAtom ws("hungry", true);
			mWorldState.push_back(ws);
			GOAPStatus desirestatus(MWBase::GOAPStatus::VITALS, "hunger", -1);
			GOAPDesire desire(desirestatus, 1);
			mGOAPDesires.push_back(desire);
		}
		

		//push lower hunger urge subbrians output
		//Maybe relative to hunger?
	}

	std::vector<MWBase::SensoryLink> sensorylinks = awareness->getSensoryLinkStore()->mCurrentSensoryLinks;
	
	unsigned int itx = 0;
	while (itx < sensorylinks.size())
	{
		MWBase::SensoryLink sl = sensorylinks[itx];
		MWBase::SmartEntityInstance * sei = sl.mSEInstance;
		std::string id = sei->getRefId();
		if (id == "god_loaf") //if offers 
		{
			//std::cout << "Oh man I see food" << std::endl;

		}
		

		itx += 1;
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
