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

};
