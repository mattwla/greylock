#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBreadTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityBreadTemplate::SmartEntityBreadTemplate();

		virtual MWBase::SmartEntityInstance * getInstance(const MWWorld::Ptr &ptr);

};

class SmartEntityBreadInstance : public MWBase::SmartEntityInstance {


	public:

		SmartEntityBreadInstance::SmartEntityBreadInstance(const MWWorld::Ptr &ptr);
};