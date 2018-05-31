#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityFloatShroomTemplate : public MWBase::SmartEntityTemplate {

public:
	SmartEntityFloatShroomTemplate::SmartEntityFloatShroomTemplate();

	virtual ~SmartEntityFloatShroomTemplate() {

	};

	virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

	virtual MWBase::SmartEntityInstance * getInstance(std::string id, ESM::RefNum refNum);

	virtual MWBase::SmartEntityInstance * loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate);

};

class SmartEntityFloatShroomInstance : public MWBase::SmartEntityInstance {
public:

	virtual float getActivationDistance()
	{
		return 1600.0f;
	}

	SmartEntityFloatShroomInstance::SmartEntityFloatShroomInstance(const MWWorld::Ptr &ptr);

	SmartEntityFloatShroomInstance::SmartEntityFloatShroomInstance(std::string refid, ESM::RefNum refnum, int pings);

	virtual ~SmartEntityFloatShroomInstance() {

	};

	virtual std::string getSaveString();

	virtual bool isAvailableForUse();

	virtual MWBase::BehaviorObject * useWorldInstance(MWBase::Life * user);

	virtual void onImpact(MWWorld::Ptr impactwith = nullptr);
};