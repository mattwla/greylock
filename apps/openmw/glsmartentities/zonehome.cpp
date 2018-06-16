#include "zonehome.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/aifollow.hpp"
#include "../mwmechanics/aicombat.hpp"
#include "../mwworld/class.hpp"
#include <osg/Group>
#include <osg/ComputeBoundsVisitor>
#include <components/sceneutil/positionattitudetransform.hpp>
SmartZoneHomeTemplate::SmartZoneHomeTemplate()
{
	mIngameIDs.push_back("cargo_container");
	//mIngameIDs.push_back("z_cave");
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(const MWWorld::Ptr & ptr)
{

	return new SmartZoneHomeInstance(ptr);
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	return new SmartZoneHomeInstance(id, refNum);
}

MWBase::SmartEntityInstance * SmartZoneHomeTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{

	return new SmartZoneHomeInstance(refid, refnum);
}

SmartZoneHomeInstance::SmartZoneHomeInstance(const MWWorld::Ptr & ptr)
{
	std::cout << "new home ptr method" << std::endl;
	mIsSmartZone = true;
	
	

}

SmartZoneHomeInstance::SmartZoneHomeInstance(std::string id, ESM::RefNum)
{
	std::cout << "new home id refnum method" << std::endl;
	mIsSmartZone = true;
}

std::string SmartZoneHomeInstance::getSaveString()
{
	return std::string();
}

bool SmartZoneHomeInstance::isAvailableForUse()
{
	return false;
}

MWBase::BehaviorObject * SmartZoneHomeInstance::useWorldInstance(MWBase::Life * user)
{
	return nullptr;
}

void SmartZoneHomeInstance::buildBoundingBox()
{
	std::cout << "building bbox" << std::endl;
	osg::ComputeBoundsVisitor computeBounds;

	mPtr.getRefData().getBaseNode()->accept(computeBounds);
	mBoundingBox = computeBounds.getBoundingBox();
}

bool SmartZoneHomeInstance::containsPtr(MWWorld::Ptr ptr)
{
	osg::Vec3f pos = ptr.getRefData().getPosition().asVec3();

	return mBoundingBox.contains(pos);
}

MWBase::SubBrain * SmartZoneHomeInstance::getSubBrain(MWBase::Life * life)
{
	
		return new HomeSubBrain(life, this);
	

}

HomeSubBrain::HomeSubBrain(MWBase::Life * life, MWBase::SmartEntityInstance * home)
{
	
		mOwnerLife = life;
		mHomeSEI = home;
		BOConfrontHomeTrespasser * bot = new BOConfrontHomeTrespasser(99);
		bot->mOwnerZone = mHomeSEI;
		bot->setParentSubBrain(this);
		
		mConfrontTrespasserBO = bot;
		

}

void HomeSubBrain::calculate(MWBase::Awareness * awareness)
{
	{


		typedef std::unordered_map<ESM::RefNum, MWBase::SensoryLink> linklist;

		//search memory for items that match status, create a node for each and cost dependant on distance to npc.

		///MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
		linklist currentlinks = awareness->getSensoryLinksThisFrame()->mSensoryLinks;
		//std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;



		for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
		{
			if (mHomeSEI->containsPtr(it->second.mSEInstance->getPtr()))
			{
				if (it->second.mSEInstance->isHumanLife())
				{
					bool allowed = mHomeSEI->isAllowedTerritory(it->second.mSEInstance->getLife());
					if (!allowed && !mRemovePersonDesire)
					{
						MWBase::GOAPStatus desirestatus(MWBase::REMOVE_PERSON_FROM_ZONE, "", 1, it->second.mSEInstance);
						std::shared_ptr<MWBase::GOAPDesire> pDesire = std::make_shared<MWBase::GOAPDesire>(desirestatus, 900);
						mOwnerLife->submitDesirePtr(pDesire);
						mRemovePersonDesire = pDesire;
						/*if (!mOwnerLife->mCurrentSpeech)
							mOwnerLife->say("GET OUTTA MY HOUSE!!!!");*/
					}

				}
			}
		}





		//std::cout << "home sub brain calculating" << std::endl;
		if (mHomeSEI->containsPtr(mOwnerLife->mPtr))
		{
			if (!mOwnerLife->mCurrentSpeech && !mWasInHomeLastUpdate)
				mOwnerLife->say("Good to be home");

			mWasInHomeLastUpdate = true;
		}
		else
		{
			mWasInHomeLastUpdate = false;
		}


	};

}

std::vector<std::shared_ptr<MWBase::GOAPNodeData>> HomeSubBrain::getMatchingBehaviorObjects(MWBase::GOAPStatus status)
{

	std::vector<std::shared_ptr<MWBase::GOAPNodeData>> results;

	if (status.mStatusType == MWBase::REMOVE_PERSON_FROM_ZONE)
	{
		enum MWBase::AffectType affect = mOwnerLife->mAffect.getDominantAffect();
		if (affect == MWBase::AFFECT_TYPE_ANGER)
		{

			MWBase::GOAPStatus statusinput(MWBase::STATUS_VOID, "", 1);
			MWBase::GOAPStatus statusoutput(MWBase::REMOVE_PERSON_FROM_ZONE, "", 1);
			ESM::RefNum refnum = status.mTarget->getRefNum();

			std::shared_ptr < MWBase:: GOAPNodeData > node(new MWBase::GOAPNodeData(statusinput, statusoutput, mConfrontTrespasserBO, refnum, 1, "confront trespasser node"));
			results.push_back(node);
			std::cout << "made confront node" << std::endl;
			//return confrontation node
		}



		//if affect fear, find guard or just be meek?
	}




	return results;
}

BOConfrontHomeTrespasser::BOConfrontHomeTrespasser(int intensity)
{
}

void BOConfrontHomeTrespasser::getDebugInfo()
{
}

MWBase::BOReturn BOConfrontHomeTrespasser::update(float time, MWWorld::Ptr ownerptr)
{
	MWBase::SmartEntityInstance * sei = mOwnerLife->mAwareness->getSensoryLinkStore()->mSensoryLinks[mTargetRefNum].mSEInstance;
	MWWorld::Ptr ownerPtr = mOwnerLife->mPtr;
	MWMechanics::AiSequence& seq = ownerPtr.getClass().getCreatureStats(ownerPtr).getAiSequence();
	//if (seq.getTypeId() == 0)
	{
		
	}

	if (!mOwnerLife->mCurrentSpeech)
	{
		if (mWarningStage == 0)
		{
			mOwnerLife->say("GET OUT!");
			seq.stack(MWMechanics::AiFollow("player"), ownerPtr);
			mWarningStage += 1;
		}
		else if (mWarningStage == 1)
		{
			mWarningStage += 1;
			mOwnerLife->say("LAST WARNING!");
		}
		else if (mWarningStage == 2)
		{
			seq.clear();
			//seq.stack(MWMechanics::AiCombat(sei->getPtr()), mOwnerLife->mPtr);
			//More like request fight or flight BO
			mWarningStage += 1;
			
			MWBase::GOAPStatus desirestatus(MWBase::VITALS, "health", -1, sei);
			std::shared_ptr<MWBase::GOAPDesire> pDesire = std::make_shared<MWBase::GOAPDesire>(desirestatus, 900);
			mSubPlan = mOwnerLife->selectIntentionPlan(pDesire);
			//mWarningStage += 1;
			
			//Need an mSubPlan for BOs, BO can request a plan to accomplish something.
			//Evidence for abstracting the running of a plan into a plan object
		}
		else
		{
			mSubPlan.progress(time);
		}
	
	
	}

	
	/*if (sei)
	{
		std::cout << "I have a target and it is:" + sei->getRefId() << std::endl;
	}*/

	if (!mOwnerZone->containsPtr(sei->getPtr()))
	{

		mOwnerLife->say("And stay out!");
		mParentSubBrain->clearDesires();
		return MWBase::COMPLETE;
	}

	return MWBase::IN_PROGRESS;
}

MWBase::BOReturn BOConfrontHomeTrespasser::start()
{
	
	return MWBase::IN_PROGRESS;
}
