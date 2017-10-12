#include "actiondistanttalk.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"

namespace MWWorld
{
	ActionDistantTalk::ActionDistantTalk(const Ptr& actor) : Action(false, actor) {}



	void ActionDistantTalk::executeImp(const Ptr& actor)
	{
		//MWBase::Environment::get().getDialogueManager()->startDistantDialogue(getTarget());
		MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_DistantDialogue, getTarget());
	}
}
