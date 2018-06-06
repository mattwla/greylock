#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityImpulseShroomTemplate : public MWBase::SmartEntityTemplate {

public:
	SmartEntityImpulseShroomTemplate::SmartEntityImpulseShroomTemplate();

	virtual ~SmartEntityImpulseShroomTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

};

class SmartEntityImpulseShroomInstance : public MWBase::SmartEntityInstance {
public:

	virtual float getActivationDistance()
	{
		return 1600.0f;
	}

	SmartEntityImpulseShroomInstance::SmartEntityImpulseShroomInstance(const MWWorld::Ptr &ptr);

	SmartEntityImpulseShroomInstance::SmartEntityImpulseShroomInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityImpulseShroomInstance() {

	};

	virtual std::string getSaveString();

	virtual bool isAvailableForUse();

	virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);

	virtual void onImpact(MWWorld::Ptr impactwith = nullptr);

	virtual void startCharge(MWBase::Life * user);

	virtual void releaseCharge(MWBase::Life * user);
};