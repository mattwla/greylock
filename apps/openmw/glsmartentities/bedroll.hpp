#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBedrollTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityBedrollTemplate::SmartEntityBedrollTemplate();

		virtual ~SmartEntityBedrollTemplate() {

		};

		virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

		virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

		virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, int pings);

		//virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, int refnum, int pings);

};

class SmartEntityBedrollInstance : public MWBase::SmartEntityInstance {


	public:

		virtual float getActivationDistance()
		{
			return 1600.0f;
		}

		SmartEntityBedrollInstance::SmartEntityBedrollInstance(const MWWorld::Ptr &ptr);

		SmartEntityBedrollInstance::SmartEntityBedrollInstance(std::string refid, ESM::RefNum refnum, int pings);

		virtual ~SmartEntityBedrollInstance() {

		};

		virtual std::string getSaveString();

		virtual bool isAvailableForUse();
	
		virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);



	

		
};

class BOBedrollSleep : public MWBase::BehaviorObject
{


public:

	//BOBedrollSit::BOBedrollSit();
	
	BOBedrollSleep* Clone(MWBase::Life * life, ESM::RefNum refnum)
	{

		BOBedrollSleep * newbo = new BOBedrollSleep(*this);
		newbo->mOwnerLife = life;
		newbo->mTargetRefNum = refnum;
		return newbo;
	};


	BOBedrollSleep::BOBedrollSleep(int valence);

	//void getDebugInfo();

	virtual void getDebugInfo();

	virtual MWBase::BOReturn update(float time, MWWorld::Ptr ownerptr);

	virtual MWBase::BOReturn start();

	virtual ~BOBedrollSleep() {

	};

	virtual bool stop();

private:

	BOBedrollSleep(BOBedrollSleep const &instance)
	{

	};

};