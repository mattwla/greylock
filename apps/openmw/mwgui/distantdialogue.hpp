#ifndef MWGUI_DISTANTDIALOGE_H
#define MWGUI_DISTANTDIALOGE_H

#include "windowbase.hpp"
#include "referenceinterface.hpp"

#include "bookpage.hpp"

#include "../mwdialogue/keywordsearch.hpp"

namespace Gui
{
	class MWList;
}

namespace MWGui
{
	class WindowManager;
}

namespace MWGui
{
	class DistantDialogueWindow : public WindowBase, public ReferenceInterface
	{
	public:
		DistantDialogueWindow();

		virtual void exit();

		typedef MyGUI::delegates::CMultiDelegate0 EventHandle_Void;

		void startDistantDialogue(MWWorld::Ptr actor, std::string npcName);

	private:
		
		MyGUI::IntSize mCurrentWindowSize;

	};
}


#endif