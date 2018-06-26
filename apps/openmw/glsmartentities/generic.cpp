#include "generic.hpp"
#include "../mwbase/lifemanager.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/action.hpp"
#include "../subbrains/subbrain.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include <iostream>

SmartEntityGenericTemplate::SmartEntityGenericTemplate()
{
	mIngameIDs.push_back("battery1_pack");
	mIngameIDs.push_back("cargoxtestgreen");
	mIngameIDs.push_back("wooden staff");
	mIngameIDs.push_back("furn_de_firepit");
	mIngameIDs.push_back("in_velothismall_ndoor_01");
	//mIngameIDs.push_back("light_logpile_177");
	mIngameIDs.push_back("light_fire");

}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::getInstance(const MWWorld::Ptr & ptr)
{
	auto sei = new SmartEntityGenericInstance(ptr);
	giveGenericStatus(sei);
	return sei;
}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::getInstance(std::string id, ESM::RefNum refNum)
{
	auto sei = new SmartEntityGenericInstance(id, refNum, 0);
	giveGenericStatus(sei);
	return sei;
}

MWBase::SmartEntityInstance * SmartEntityGenericTemplate::loadInstance(std::string refid, ESM::RefNum refnum, std::string savestate)
{
	return nullptr;
}

void SmartEntityGenericTemplate::giveGenericStatus(MWBase::SmartEntityInstance * sei)
{
	std::cout << "checking for generic status" << std::endl;
	typedef std::map<std::string, std::vector<MWBase::Status>> idtostatusmap;
	typedef std::vector<MWBase::Status> statusvec;
	
	idtostatusmap themap;

	themap["in_velothismall_ndoor_01"] = { MWBase::IsFlammable };
	themap["gl_sitting_stump"] = { MWBase::IsFlammable };
	themap["cargoxtestgreen"] = { MWBase::NotClimbable };
	themap["cargoxtestblue"] = { MWBase::NotClimbable };
	themap["cargoxtest"] = { MWBase::NotClimbable };
	themap["cargoxtestred"] = { MWBase::NotClimbable };
	themap["furn_com_r_table_01"] = { MWBase::SmallWooden, MWBase::IsFlammable};
	themap["wooden staff"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_com_bucket_boe_unia"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_com_bucket_boe_unib"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_de_lute_01"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_de_drum_01"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_de_drum_02"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	themap["misc_com_broom_01"] = { MWBase::SmallWooden,  MWBase::IsFlammable };
	//themap["furn_de_firepit"] = { MWBase::OnFire };
	//themap["light_logpile_177"] = { MWBase::OnFire };
	themap["misc_de_muck_shovel_01"] = { MWBase::IsFlammable };
	themap["misc_de_basket_01"] = { MWBase::IsFlammable };
	themap["light_fire"] = { MWBase::OnFire };
	



	if (!themap.count(sei->getRefId()))
		return;

	std::cout << "found something in the map" << std::endl;

	statusvec stati = themap[sei->getRefId()];
	sei->ensureStatusManager();

	for (statusvec::iterator it = stati.begin(); it != stati.end(); it++)
	{
		sei->getStatusManager()->giveStatus((*it));
	}

	return;

}

SmartEntityGenericInstance::SmartEntityGenericInstance(const MWWorld::Ptr & ptr)
{
	mLiveCellRef = ptr.getBase();
	std::cout << "made generic smartentity" << std::endl;
	mPingCount = 0;
	mRefId = ptr.getCellRef().getRefId();
	mPtr = ptr;
	SmartEntityGenericTemplate::giveGenericStatus(this);
}

SmartEntityGenericInstance::SmartEntityGenericInstance(std::string refid, ESM::RefNum refnum, int pings)
{
	std::cout << "new generic" << std::endl;
	mPingCount = pings;
	mRefId = refid;
	SmartEntityGenericTemplate::giveGenericStatus(this);
}

std::string SmartEntityGenericInstance::getSaveString()
{
	return std::string();
}
