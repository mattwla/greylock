#include "distantdialogue.hpp"

#include <MyGUI_LanguageManager.h>
#include <MyGUI_Window.h>
#include <MyGUI_ProgressBar.h>

#include <components/widgets/list.hpp>
#include <components/translation/translation.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/dialoguemanager.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"

#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/actorutil.hpp"

#include "widgets.hpp"
#include "bookpage.hpp"
#include "textcolours.hpp"

#include "journalbooks.hpp" // to_utf8_span