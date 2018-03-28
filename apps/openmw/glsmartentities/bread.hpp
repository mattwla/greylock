#include "../mwbase/smartentitiesmanager.hpp"

class SmartEntityBreadTemplate : public MWBase::SmartEntityTemplate {


	public:
		SmartEntityBreadTemplate::SmartEntityBreadTemplate();

		virtual MWBase::SmartEntityInstance * getInstance();

};

class SmartEntityBreadInstance : public MWBase::SmartEntityInstance {


	public:

		SmartEntityBreadInstance::SmartEntityBreadInstance();
};