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

		virtual bool exit();

		typedef MyGUI::delegates::CMultiDelegate0 EventHandle_Void;

		void startDistantDialogue(MWWorld::Ptr actor, std::string npcName);

	protected:
		
		void onByeClicked(MyGUI::Widget* _sender);
		
		void onCallOverClicked(MyGUI::Widget* _sender);

		void onWaveClicked(MyGUI::Widget* _sender);

		void onWindowResize(MyGUI::Window* _sender);
		
		virtual void onReferenceUnavailable();

	private:
		
		MyGUI::IntSize mCurrentWindowSize;

		bool mEnabled;

		bool mGoodbye;

		int mDisposition;

		MWWorld::Ptr mActor;

	};
}


#endif