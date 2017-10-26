#include "aischedule.hpp"


#include <components/misc/rng.hpp>

#include <components/esm/loadmgef.hpp>
#include <components/esm/loadnpc.hpp>
#include <components/esm/npcstate.hpp>
#include <components/settings/settings.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/mechanicsmanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/dialoguemanager.hpp"
#include "../mwbase/soundmanager.hpp"

#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/npcstats.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwmechanics/spellcasting.hpp"
#include "../mwmechanics/disease.hpp"
#include "../mwmechanics/combat.hpp"
#include "../mwmechanics/autocalcspell.hpp"
#include "../mwmechanics/difficultyscaling.hpp"
#include "../mwmechanics/character.hpp"
#include "../mwmechanics/actorutil.hpp"

#include "../mwworld/ptr.hpp"
#include "../mwworld/actiontalk.hpp"
#include "../mwworld/actiondistanttalk.hpp"
#include "../mwworld/actionopen.hpp"
#include "../mwworld/failedaction.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/customdata.hpp"
#include "../mwphysics/physicssystem.hpp"
#include "../mwworld/cellstore.hpp"

#include "../mwrender/objects.hpp"
#include "../mwrender/renderinginterface.hpp"
#include "../mwrender/npcanimation.hpp"

#include "../mwgui/tooltips.hpp"
