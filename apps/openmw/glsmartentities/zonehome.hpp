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

	virtual MWBase::SubBrain * getSubBrain(MWBase::Life * life);


};



	class HomeSubBrain : public MWBase::SubBrain
	{
		//ZoneBrain * mParentBrain;
		MWBase::SmartEntityInstance * mHomeSEI;
		MWBase::BehaviorObject * mConfrontTrespasserBO;
		std::shared_ptr<MWBase::GOAPDesire> mRemovePersonDesire = 0;
		std::shared_ptr<MWBase::GOAPDesire> mKillThiefDesire = 0;
		bool mWasInHomeLastUpdate = true;
	
		public:
		
			HomeSubBrain(MWBase::Life * life, MWBase::SmartEntityInstance * home);
		
			virtual void calculate(MWBase::Awareness * awareness);

			virtual std::string getID()
			{
				return "Home SubBrain";
			};

			virtual void getDebugInfo()
			{

			};

			virtual std::vector<std::shared_ptr<MWBase::GOAPNodeData>> getMatchingBehaviorObjects(MWBase::GOAPStatus status);

			virtual void clearDesires()
			{
				//mwx fix me memory leak
				if (mRemovePersonDesire)
				{
					mRemovePersonDesire->mIntensity = 0;
					mRemovePersonDesire = 0;
				}
			}


	};


	class BOConfrontHomeTrespasser : public MWBase::BehaviorObject
	{

	public:

		MWBase::SmartEntityInstance * mOwnerZone;

		BOConfrontHomeTrespasser* Clone(MWBase::Life * life, ESM::RefNum refnum)
		{
			BOConfrontHomeTrespasser * newbo = new BOConfrontHomeTrespasser(*this);
			newbo->mOwnerLife = life;
			newbo->mTargetRefNum = refnum;
			return newbo;
		};

		int mWarningStage = 0;

		BOConfrontHomeTrespasser::BOConfrontHomeTrespasser(int intensity);

		virtual void getDebugInfo();

		virtual MWBase::BOReturn update(float time, MWWorld::Ptr ownerptr);

		virtual MWBase::BOReturn start();

		virtual ~BOConfrontHomeTrespasser() {

		};

		virtual bool stop()
		{
			mStopRequested = true;
			//by default no stop logic;
			//std::cout << "BO with no stop logic recieved stop request - BOEAT" << std::endl;
			return true;
		};


	private:

		BOConfrontHomeTrespasser(BOConfrontHomeTrespasser const &instance)
		{
			mOwnerZone = instance.mOwnerZone;
			mParentSubBrain = instance.mParentSubBrain;

		};

		MWBase::IntentionPlan mSubPlan;

	};
	
