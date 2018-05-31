#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityFireShroomTemplate : public MWBase::SmartEntityTemplate {

public:
	SmartEntityFireShroomTemplate::SmartEntityFireShroomTemplate();

	virtual ~SmartEntityFireShroomTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

};

class SmartEntityFireShroomInstance : public MWBase::SmartEntityInstance {
public:

	virtual float getActivationDistance()
	{
		return 1600.0f;
	}

	SmartEntityFireShroomInstance::SmartEntityFireShroomInstance(const MWWorld::Ptr &ptr);

	SmartEntityFireShroomInstance::SmartEntityFireShroomInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityFireShroomInstance() {

	};

	virtual std::string getSaveString();

	virtual bool isAvailableForUse();

	virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);

	virtual void onImpact(MWWorld::Ptr impactwith = nullptr);
};