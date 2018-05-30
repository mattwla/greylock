#include "demoquestmetabrain.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"


MWBase::DemoQuestMetaBrain::DemoQuestMetaBrain()
{

	
	//MWWorld::Ptr arxptr = MWBase::Environment::get().getWorld()->searchPtr("arx", false);
	//MWWorld::Ptr nadiaptr = MWBase::Environment::get().getWorld()->searchPtr("nadia", false);
	mArxLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("arx");
	mNadiaLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("nadia");

	SubBrainDemoQuest * arxsb = new SubBrainDemoQuest(mArxLife);
	arxsb->setMetaBrain(this);
	mArxLife->mSubBrainsManager->addSubBrain(arxsb);
	
	SubBrainDemoQuest * nadiasb = new SubBrainDemoQuest(mNadiaLife);
	nadiasb->setMetaBrain(this);
	mNadiaLife->mSubBrainsManager->addSubBrain(nadiasb);
	
	
	GOAPStatus desirestatus(MWBase::RUNNING_BEHAVIOR_OBJECT, "arxdemoquest", 1);
	std::shared_ptr<GOAPDesire> pDesire = std::make_shared<GOAPDesire>(desirestatus, 999);
	mArxLife->submitDesirePtr(pDesire);
	
	
	GOAPStatus desirestatusn(MWBase::RUNNING_BEHAVIOR_OBJECT, "nadiademoquest", 1);
	pDesire = std::make_shared<GOAPDesire>(desirestatusn, 999);
	mNadiaLife->submitDesirePtr(pDesire);

	mArxBO = new ArxDemoQuestBO;
	mNadiaBO = new NadiaDemoQuestBO;

	//Give Arx and Nadia subbrains which ping me for stuff.
	
}

void MWBase::DemoQuestMetaBrain::update(float duration)
{
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::DemoQuestMetaBrain::requestDesire(MWBase::Life * life)
{
	std::vector<std::shared_ptr<MWBase::GOAPNodeData>> desires;
	return desires;
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::DemoQuestMetaBrain::getMatchingBO(MWBase::GOAPStatus status, MWBase::Life * life)
{
	std::vector<std::shared_ptr<MWBase::GOAPNodeData>> result;
	
	if (status.mStatusType == MWBase::RUNNING_BEHAVIOR_OBJECT)
	{
		if (status.mExtraData == "arxdemoquest")
		{
			MWBase::GOAPStatus statusinput(MWBase::STATUS_VOID, "", 1);
			MWBase::GOAPStatus statusoutput(MWBase::RUNNING_BEHAVIOR_OBJECT, "arxdemoquest", 1);
			ESM::RefNum refnum;
			std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mArxBO, refnum, 1, "arx node"));
			node->mCost = 0;
			result.push_back(node);
		}

		if (status.mExtraData == "nadiademoquest")
		{
			MWBase::GOAPStatus statusinput(MWBase::STATUS_VOID, "", 1);
			MWBase::GOAPStatus statusoutput(MWBase::RUNNING_BEHAVIOR_OBJECT, "nadiaemoquest", 1);
			ESM::RefNum refnum;
			std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mNadiaBO, refnum, 1, "nadia node"));
			node->mCost = 0;
			result.push_back(node);
		}

	}

	return result;
}

MWBase::SubBrainDemoQuest::SubBrainDemoQuest(MWBase::Life * life)
{
}

void MWBase::SubBrainDemoQuest::calculate(MWBase::Awareness * awareness)
{
}

std::string MWBase::SubBrainDemoQuest::getID()
{
	return "sub brian demo quest";
}

void MWBase::SubBrainDemoQuest::getDebugInfo()
{
	
}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> MWBase::SubBrainDemoQuest::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{

	std::vector<std::shared_ptr<GOAPNodeData>> result = mMetaBrain->getMatchingBO(status, mOwnerLife);

	

	return result;
}


//===================ARX DEMO BO=============================

MWBase::ArxDemoQuestBO::ArxDemoQuestBO()
{

}

MWBase::BOReturn MWBase::ArxDemoQuestBO::update(float time, MWWorld::Ptr ownerptr)
{
	float dist = (mOwnerLife->mPtr.getRefData().getPosition().asVec3() - MWBase::Environment::get().getWorld()->getPlayerPtr().getRefData().getPosition().asVec3()).length2();

	if (dist > 20000.0f)
	{
		if (!mOwnerLife->mCurrentSpeech)
		{
			mOwnerLife->say("GET OVER HERE FOR YOUR QUEST");
		}
	}
	else
	{
		if (!mOwnerLife->mCurrentSpeech)
		{
			mOwnerLife->say("GET ME SOME ARTIFACTS");
		}
	}

	return MWBase::IN_PROGRESS;
}

void MWBase::ArxDemoQuestBO::getDebugInfo()
{
}

MWBase::BOReturn MWBase::ArxDemoQuestBO::start()
{
	return MWBase::IN_PROGRESS;
}


//=============NADIA QUEST BO====================



MWBase::NadiaDemoQuestBO::NadiaDemoQuestBO()
{
}

MWBase::BOReturn MWBase::NadiaDemoQuestBO::update(float time, MWWorld::Ptr ownerptr)
{
	float dist = (mOwnerLife->mPtr.getRefData().getPosition().asVec3() - MWBase::Environment::get().getWorld()->getPlayerPtr().getRefData().getPosition().asVec3()).length2();

	if (dist > 20000.0f)
	{
		if (!mOwnerLife->mCurrentSpeech)
		{
			mOwnerLife->say("HEY YO");
		}
	}
	else
	{
		if (!mOwnerLife->mCurrentSpeech)
		{
			mOwnerLife->say("DO THIS THING");
		}
	}

	return MWBase::IN_PROGRESS;
}

void MWBase::NadiaDemoQuestBO::getDebugInfo()
{
}

MWBase::BOReturn MWBase::NadiaDemoQuestBO::start()
{
	return MWBase::IN_PROGRESS;
}
