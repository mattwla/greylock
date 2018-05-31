#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBounceShroomTemplate : public MWBase::SmartEntityTemplate {

public:
	SmartEntityBounceShroomTemplate::SmartEntityBounceShroomTemplate();

	virtual ~SmartEntityBounceShroomTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

};

class SmartEntityBounceShroomInstance : public MWBase::SmartEntityInstance {
public:

	virtual float getActivationDistance()
	{
		return 1600.0f;
	}

	SmartEntityBounceShroomInstance::SmartEntityBounceShroomInstance(const MWWorld::Ptr &ptr);

	SmartEntityBounceShroomInstance::SmartEntityBounceShroomInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityBounceShroomInstance() {

	};

	virtual std::string getSaveString();

	virtual bool isAvailableForUse();

	virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);

	virtual void onImpact();
};