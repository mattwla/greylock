#include "demoquestmetabrain.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwmechanics/steering.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/drawstate.hpp"

MWBase::DemoQuestMetaBrain::DemoQuestMetaBrain()
{
	mArxLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("arx");
	mNadiaLife = MWBase::Environment::get().getLifeManager()->getLifeFromID("nadia");

	SubBrainDemoQuest * arxsb = new SubBrainDemoQuest(mArxLife);
	arxsb->setMetaBrain(this);
	mArxLife->mSubBrainsManager->addSubBrain(arxsb);
	
	SubBrainDemoQuest * nadiasb = new SubBrainDemoQuest(mNadiaLife);
	nadiasb->setMetaBrain(this);
	mNadiaLife->mSubBrainsManager->addSubBrain(nadiasb);
	
	
	GOAPStatus desirestatus(MWBase::RUNNING_BEHAVIOR_OBJECT, "arxdemoquest", 1);
	mArxDesire = std::make_shared<GOAPDesire>(desirestatus, 999);
	mArxLife->submitDesirePtr(mArxDesire);
	
	
	GOAPStatus desirestatusn(MWBase::RUNNING_BEHAVIOR_OBJECT, "nadiademoquest", 1);
	mNadiaDesire = std::make_shared<GOAPDesire>(desirestatusn, 999);
	mNadiaLife->submitDesirePtr(mNadiaDesire);


	mArxBO = new ArxDemoQuestBO;
	mNadiaBO = new NadiaDemoQuestBO;
	mArxBO->setParentSubBrain(arxsb);
	mNadiaBO->setParentSubBrain(nadiasb);

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
			refnum.unset();
			std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mArxBO, refnum, 1, "arx node"));
			node->mCost = 0;
			result.push_back(node);
		}

		if (status.mExtraData == "nadiademoquest")
		{
			MWBase::GOAPStatus statusinput(MWBase::STATUS_VOID, "", 1);
			MWBase::GOAPStatus statusoutput(MWBase::RUNNING_BEHAVIOR_OBJECT, "nadiaemoquest", 1);
			ESM::RefNum refnum;
			refnum.unset();
			std::shared_ptr<GOAPNodeData> node(new GOAPNodeData(statusinput, statusoutput, mNadiaBO, refnum, 1, "nadia node"));
			node->mCost = 0;
			result.push_back(node);
		}

	}

	return result;
}

void MWBase::DemoQuestMetaBrain::behaviorUpdate(MWBase::Life * life)
{
	float dist = (life->mPtr.getRefData().getPosition().asVec3() - MWBase::Environment::get().getWorld()->getPlayerPtr().getRefData().getPosition().asVec3()).length2();
	bool isArx = life == mArxLife;
	bool isNadia = life == mNadiaLife;
	turnTo(life->mPtr, MWBase::Environment::get().getWorld()->getPlayerPtr());
	
	if (mStage == 0)
	{
		if (isArx && life->mCurrentSpeech == 0)
		{
			life->say("Come over here for your quest!");
			//mStage += 1;
		}

		if (isArx && dist < 20000.0)
			mStage += 1;
	}
	else if (mStage == 1)
	{
		if (isArx)
		{
			life->say("I need you to acquire 3 ancient artifacts for me.");
			mStage += 1;
		}
	}
	else if (mStage == 2)
	{
		if (isArx && life->mCurrentSpeech == 0)
			mStage += 1;
	}

	else if (mStage == 3)
	{
		if (isNadia)
		{
			life->say("And by acquire he means steal.");
			mStage += 1;
		}
	}
	else if (mStage == 4)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 5)
	{
		if (isArx)
		{
			life->say("Yes. That is probably true. But in the name of SCIENCE!");
			mStage += 1;
		}
	}
	else if (mStage == 6)
	{
		if (isArx && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 7)
	{
		


		if (isArx)
		{
			MWWorld::InventoryStore &inventoryStore = life->mPtr.getClass().getInventoryStore(life->mPtr);
			MWWorld::ContainerStoreIterator battery = inventoryStore.end();
			for (MWWorld::ContainerStoreIterator it = inventoryStore.begin(); it != inventoryStore.end(); ++it)
			{
				if (it->getCellRef().getRefId() == "battery1_pack")
				{
					std::cout << "found battery" << std::endl;
					battery = it;
					break;
				}
			}

			
			inventoryStore.equip(16, battery, life->mPtr);
			life->mPtr.getClass().getCreatureStats(life->mPtr).setDrawState(MWMechanics::DrawState_Weapon);
			
			//MWBase::Environment::get().getWorld()->getPlayer().setDrawState(MWMechanics::DrawState_Weapon);




			life->say("See this thing? I need you to get me 3 more.");
			mStage += 1;
		}
	}
	else if (mStage == 8)
	{
		if (isArx && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 9)
	{
		if (isNadia)
		{
			life->say("People in the village use them as decorations. They have horrible taste.");
			mStage += 1;
		}
	}
	else if (mStage == 10)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 11)
	{
		if (isNadia)
		{
			life->say("You'll have to 'acquire' them from houses in the village.");
			mStage += 1;
		}
	}
	else if (mStage == 12)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 13)
	{
		if (isNadia)
		{
			life->say("To get there I would hop off this cliff, and glide down to the bounce shroom. (tap control while in air to glide).");
			mStage += 1;
		}
	}
	else if (mStage == 14)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 15)
	{
		if (isNadia)
		{
			life->say("Give the bounce shroom a good punch, and it'll launch you in the air. Use the height to glide to the village.");
			mStage += 1;
		}
	}
	else if (mStage == 16)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 17)
	{
		if (isNadia)
		{
			life->say("Oh, and I left some fire shrooms by our campfire, they might help with burning anything that gets in your way.");
			mStage += 1;
		}
		/*mArxDesire->mIntensity = 0;
		mNadiaDesire->mIntensity = 0;*/
		/*mArxDesire = 0;
		mNadiaDesire = 0;*/

	}
	else if (mStage == 18)
	{
		if (isNadia && life->mCurrentSpeech == 0)
			mStage += 1;
	}
	else if (mStage == 19)
	{
		if (isArx)
		{
			life->say("Good luck!");
			mStage += 1;
		}
		
		/*mArxDesire = 0;
		mNadiaDesire = 0;*/

	}
	else if (mStage == 20)
	{
		if (isArx && life->mCurrentSpeech == 0)
		{
			mArxDesire->mIntensity = 0;
			mNadiaDesire->mIntensity = 0;
		}
	}






}

bool MWBase::DemoQuestMetaBrain::turnTo(MWWorld::Ptr actor, MWWorld::Ptr target) {
	
		osg::Vec3f targetPos(target.getRefData().getPosition().asVec3());
		osg::Vec3f actorPos(actor.getRefData().getPosition().asVec3());

		osg::Vec3f dir = targetPos - actorPos;

		float faceAngleRadians = std::atan2(dir.x(), dir.y());

		if (true)
		{
			if (MWMechanics::zTurn(actor, faceAngleRadians))
				bool rotate = false;
			else
				return false;
		}
		
	
}



MWBase::SubBrainDemoQuest::SubBrainDemoQuest(MWBase::Life * life)
{
}

void MWBase::SubBrainDemoQuest::calculate(MWBase::Awareness * awareness)
{
}

std::string MWBase::SubBrainDemoQuest::getID()
{
	return "sub brain demo quest";
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
	if (mStopRequested)
		return MWBase::COMPLETE;
	mParentSubBrain->getMetaBrain()->behaviorUpdate(mOwnerLife);

	//request from metabrain

	//float dist = (mOwnerLife->mPtr.getRefData().getPosition().asVec3() - MWBase::Environment::get().getWorld()->getPlayerPtr().getRefData().getPosition().asVec3()).length2();

	//if (dist > 20000.0f)
	//{
	//	if (!mOwnerLife->mCurrentSpeech)
	//	{
	//		mOwnerLife->say("GET OVER HERE FOR YOUR QUEST");
	//	}
	//}
	//else
	//{
	//	if (!mOwnerLife->mCurrentSpeech)
	//	{
	//		mOwnerLife->say("GET ME SOME ARTIFACTS");
	//	}
	//}

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

	if (mStopRequested)
		return MWBase::COMPLETE;

	mParentSubBrain->getMetaBrain()->behaviorUpdate(mOwnerLife);



	//float dist = (mOwnerLife->mPtr.getRefData().getPosition().asVec3() - MWBase::Environment::get().getWorld()->getPlayerPtr().getRefData().getPosition().asVec3()).length2();

	//if (dist > 20000.0f)
	//{
	//	if (!mOwnerLife->mCurrentSpeech)
	//	{
	//		mOwnerLife->say("HEY YO");
	//	}
	//}
	//else
	//{
	//	if (!mOwnerLife->mCurrentSpeech)
	//	{
	//		mOwnerLife->say("DO THIS THING");
	//	}
	//}
	return MWBase::IN_PROGRESS;
}

void MWBase::NadiaDemoQuestBO::getDebugInfo()
{
}

MWBase::BOReturn MWBase::NadiaDemoQuestBO::start()
{
	return MWBase::IN_PROGRESS;
}
