#include "../mwbase/smartentitiesmanager.hpp"

class SmartZoneHomeTemplate : public MWBase::SmartEntityTemplate
{
public:
	SmartZoneHomeTemplate();

	virtual ~SmartZoneHomeTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

};


	class HomeSubBrain : public MWBase::SubBrain
	{
		//ZoneBrain * mParentBrain;
		MWBase::SmartEntityInstance * mHomeSEI;
		bool mWasInHomeLastUpdate = true;
	
		public:
		
			HomeSubBrain(MWBase::Life * life, MWBase::SmartEntityInstance * home)
			{
				mOwnerLife = life;
				mHomeSEI = home;
			}
	
			virtual void calculate(MWBase::Awareness * awareness) {
			

				typedef std::unordered_map<ESM::RefNum, MWBase::SensoryLink> linklist;

				//search memory for items that match status, create a node for each and cost dependant on distance to npc.

				MWBase::SensoryLinkStore * sensorystore = mOwnerLife->mAwareness->getSensoryLinkStore();
				linklist currentlinks = sensorystore->mSensoryLinks;
				//std::vector<SensoryLink> currentlinks = sensorystore->mCurrentSensoryLinks;



				for (linklist::iterator it = currentlinks.begin(); it != currentlinks.end(); it++)
				{
					if (mHomeSEI->containsPtr(it->second.mSEInstance->getPtr()))
					{
						if (it->second.mSEInstance->isHumanLife())
						{
							bool allowed = mHomeSEI->isAllowedTerritory(it->second.mSEInstance->getLife());
							if (!allowed)
							{
								if (!mOwnerLife->mCurrentSpeech)
									mOwnerLife->say("GET OUTTA MY HOUSE!!!!");
							}

						}
					}
				}





				//std::cout << "home sub brain calculating" << std::endl;
				if (mHomeSEI->containsPtr(mOwnerLife->mPtr))
				{
					if(!mOwnerLife->mCurrentSpeech && !mWasInHomeLastUpdate)
						mOwnerLife->say("Good to be home");
					
					mWasInHomeLastUpdate = true;
				}
				else
				{
					mWasInHomeLastUpdate = false;
				}
				

			};

			virtual std::string getID()
			{
				return "Home SubBrain";
			};

			virtual void getDebugInfo()
			{

			};

			virtual std::vector<std::shared_ptr<MWBase::GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus status) 
			{
				std::vector<std::shared_ptr<MWBase::GOAPNodeData>> blank;
				return blank;
			};

	};

	



class SmartZoneHomeInstance : public MWBase::SmartEntityInstance {
	virtual ~SmartZoneHomeInstance() {

	};

public:

	SmartZoneHomeInstance(const MWWorld::Ptr &ptr);

	SmartZoneHomeInstance(std::string id, ESM::RefNum);

	virtual std::string getSaveString();

	virtual bool isAvailableForUse();

	virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);

	virtual void buildBoundingBox();

	virtual bool containsPtr(MWWorld::Ptr ptr);

	virtual MWBase::SubBrain * getSubBrain(MWBase::Life * life)
	{
		return new HomeSubBrain(life, this);
	}


};
