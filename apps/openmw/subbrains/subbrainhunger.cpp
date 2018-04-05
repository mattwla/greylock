#include "subbrainhunger.hpp"
#include "../mwbase/awarenessreactionsmanager.hpp"
#include "../mwbase/smartentitiesmanager.hpp"
#include "../mwbase/lifemanager.hpp"



MWBase::SubBrainHunger::SubBrainHunger(MWBase::Life * life)
{
	mOwnerLife = life;
}

void MWBase::SubBrainHunger::calculate(MWBase::Awareness * awareness)
{
	if (mOwnerLife->mVitals.mHunger > 100.0f)
	{
		std::cout << "HANGRY!!" << std::endl;

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

MWBase::BOEat::BOEat()
{
	MWBase::GOAPStatus statusinput(GOAPStatus::HAS_IN_INVENTORY, "edible", 1);
	mGOAPData.mInputs.push_back(statusinput);
	MWBase::GOAPStatus statusoutput(GOAPStatus::VITALS, "hunger", -100);
	mGOAPData.mOutputs.push_back(statusoutput);

}
