#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ScriptMgr.h"

namespace
{
// mod-individual-progression stores "character reached tier N" as rewarded hidden
// quest 66000+N. CONDITION_QUESTREWARDED (type 8) on 66000+N reads as "at least tier N".
// Rows below only ADD to the level/faction conditions already shipped in
// data/sql/world/base/mod_teleport_vanilla.sql — they never replace them.
//
// Deliberately not gated: Draenei/Blood Elf starting zones (the module lets any
// character reach these before the Dark Portal, via IsTBCRaceStartingZone()), and
// Molten Core / Onyxia's Lair (tier 0 content; Onyxia has its own item check).
constexpr char const* IP_TIER_CONDITIONS = R"sql(
REPLACE INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('15', '51904', '1',  '0', '0', '8', '0', '66002', '0', '0', '0', '0', '0', '', 'IPP tier 2 - Blackwing Lair'),
('15', '51904', '21', '0', '0', '8', '0', '66003', '0', '0', '0', '0', '0', '', 'IPP tier 3 - Zul''Gurub'),
('15', '51904', '12', '0', '0', '8', '0', '66004', '0', '0', '0', '0', '0', '', 'IPP tier 4 - Ruins of Ahn''Qiraj'),
('15', '51904', '15', '0', '0', '8', '0', '66004', '0', '0', '0', '0', '0', '', 'IPP tier 4 - Temple of Ahn''Qiraj'),
('15', '51900', '10', '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Shattrath'),
('15', '51900', '15', '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Submenu Outland'),
('15', '51900', '18', '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Submenu TBC dungeons'),
('15', '51902', '0',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Auchindoun'),
('15', '51902', '2',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Coilfang Reservoir'),
('15', '51902', '3',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Hellfire Citadel'),
('15', '51902', '5',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - The Mechanar'),
('15', '51902', '6',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - The Botanica'),
('15', '51904', '5',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Gruul''s Lair'),
('15', '51904', '6',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Magtheridon''s Lair'),
('15', '51904', '8',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Karazhan'),
('15', '51907', '0',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Hellfire Peninsula'),
('15', '51907', '1',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Zangarmarsh'),
('15', '51907', '2',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Terokkar Forest'),
('15', '51907', '3',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Nagrand'),
('15', '51907', '4',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Blade''s Edge Mountains'),
('15', '51907', '5',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Netherstorm'),
('15', '51907', '6',  '0', '0', '8', '0', '66008', '0', '0', '0', '0', '0', '', 'IPP tier 8 - Shadowmoon Valley'),
('15', '51904', '3',  '0', '0', '8', '0', '66009', '0', '0', '0', '0', '0', '', 'IPP tier 9 - Serpentshrine Cavern'),
('15', '51904', '14', '0', '0', '8', '0', '66009', '0', '0', '0', '0', '0', '', 'IPP tier 9 - The Eye'),
('15', '51904', '22', '0', '0', '8', '0', '66009', '0', '0', '0', '0', '0', '', 'IPP tier 9 - Zul''Aman'),
('15', '51904', '0',  '0', '0', '8', '0', '66010', '0', '0', '0', '0', '0', '', 'IPP tier 10 - Black Temple'),
('15', '51904', '2',  '0', '0', '8', '0', '66010', '0', '0', '0', '0', '0', '', 'IPP tier 10 - Hyjal Summit'),
('15', '51902', '4',  '0', '0', '8', '0', '66012', '0', '0', '0', '0', '0', '', 'IPP tier 12 - Magisters'' Terrace'),
('15', '51904', '13', '0', '0', '8', '0', '66012', '0', '0', '0', '0', '0', '', 'IPP tier 12 - Sunwell Plateau'),
('15', '51905', '23', '0', '0', '8', '0', '66012', '0', '0', '0', '0', '0', '', 'IPP tier 12 - Isle of Quel''Danas'),
('15', '51900', '9',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Dalaran'),
('15', '51900', '16', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Submenu Northrend'),
('15', '51900', '19', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Submenu WotLK dungeons'),
('15', '51903', '0',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Azjol-Nerub'),
('15', '51903', '1',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - The Culling of Stratholme'),
('15', '51903', '3',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Drak''Tharon Keep'),
('15', '51903', '4',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Gundrak'),
('15', '51903', '6',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - The Nexus'),
('15', '51903', '7',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - The Violet Hold'),
('15', '51903', '8',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Halls of Lightning'),
('15', '51903', '9',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Halls of Stone'),
('15', '51903', '10', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Utgarde Keep'),
('15', '51903', '11', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Utgarde Pinnacle'),
('15', '51904', '10', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Naxxramas'),
('15', '51904', '17', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - The Obsidian Sanctum'),
('15', '51904', '19', '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Vault of Archavon'),
('15', '51908', '0',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Borean Tundra'),
('15', '51908', '1',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Howling Fjord'),
('15', '51908', '2',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Dragonblight'),
('15', '51908', '3',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Grizzly Hills'),
('15', '51908', '4',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Zul''Drak'),
('15', '51908', '5',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Sholazar Basin'),
('15', '51908', '6',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Crystalsong Forest'),
('15', '51908', '7',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - The Storm Peaks'),
('15', '51908', '8',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Icecrown'),
('15', '51908', '9',  '0', '0', '8', '0', '66013', '0', '0', '0', '0', '0', '', 'IPP tier 13 - Wintergrasp'),
('15', '51904', '18', '0', '0', '8', '0', '66014', '0', '0', '0', '0', '0', '', 'IPP tier 14 - Ulduar'),
('15', '51903', '2',  '0', '0', '8', '0', '66015', '0', '0', '0', '0', '0', '', 'IPP tier 15 - Trial of the Champion'),
('15', '51904', '4',  '0', '0', '8', '0', '66015', '0', '0', '0', '0', '0', '', 'IPP tier 15 - Trial of the Crusader'),
('15', '51903', '5',  '0', '0', '8', '0', '66016', '0', '0', '0', '0', '0', '', 'IPP tier 16 - The Frozen Halls'),
('15', '51904', '7',  '0', '0', '8', '0', '66016', '0', '0', '0', '0', '0', '', 'IPP tier 16 - Icecrown Citadel');
)sql";
}

class WarcraftedTeleportWorldScript : public WorldScript
{
public:
    WarcraftedTeleportWorldScript() : WorldScript("WarcraftedTeleportWorldScript") { }

    void OnStartup() override
    {
        if (!sConfigMgr->GetOption<bool>("WarcraftedTeleport.Enable", true))
            return;

        bool const individualProgression = sConfigMgr->GetOption<bool>("IndividualProgression.Enable", false);

        // Type 8 (CONDITION_QUESTREWARDED) on SourceGroup 51900-51908 is exclusively used by
        // our own tier gate, so this DELETE only ever removes rows we added ourselves. It runs
        // every startup so switching Individual Progression off cleans the gate back out.
        WorldDatabase.DirectExecute(
            "DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 "
            "AND `SourceGroup` BETWEEN 51900 AND 51908 AND `ConditionTypeOrReference` = 8");

        if (individualProgression)
        {
            WorldDatabase.DirectExecute(IP_TIER_CONDITIONS);
            LOG_INFO("server.loading", "mod-warcrafted-teleport: Individual Progression detected, tier gate applied.");
        }
        else
        {
            LOG_INFO("server.loading", "mod-warcrafted-teleport: base variant applied (no tier gate).");
        }
    }
};

void AddWarcraftedTeleportScripts()
{
    new WarcraftedTeleportWorldScript();
}
