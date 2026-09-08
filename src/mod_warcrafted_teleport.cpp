#include "Chat.h"
#include "ConditionMgr.h"
#include "Config.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "SharedDefines.h"
#include "StringFormat.h"

#include <algorithm>
#include <string>
#include <unordered_map>

namespace
{
    constexpr char const* MODULE_NAME = "mod-warcrafted-teleport";

    enum WarcraftedTeleportStrings
    {
        STR_LOGIN_ANNOUNCE   = 0,
        STR_COOLDOWN         = 1,
        STR_NOT_ENOUGH_MONEY = 2,
        STR_CHARGED          = 3,
        STR_COUNTDOWN        = 4,
        STR_INTERRUPTED      = 5,
        STR_GM_MODE          = 6
    };

    // mod-individual-progression marca "el personaje ha alcanzado el tier N" como la quest
    // oculta 66000+N ya recompensada. Se lee del jugador en vivo, así que subir de tier
    // desbloquea destinos en cuanto vuelve a abrir el menú.
    constexpr uint32 IP_TIER_QUEST_BASE = 66000;

    // Distancia que puede alejarse el jugador durante la cuenta atrás antes de cancelar el viaje.
    constexpr float TRAVEL_MOVE_TOLERANCE = 5.0f;

    struct Destination
    {
        uint32 Map;
        float X;
        float Y;
        float Z;
        float O;
    };

    struct TierRequirement
    {
        uint32 MenuId;
        uint32 OptionId;
        uint32 Tier;
    };

    // No llevan tier a propósito: las zonas iniciales draenei y elfo de sangre (accesibles
    // antes del Portal Oscuro) y Núcleo de Magma / Guarida de Onyxia (contenido de tier 0).
    constexpr TierRequirement TIER_GATE[] =
    {
        { 51904,  1,  2 }, // Blackwing Lair
        { 51904, 21,  3 }, // Zul'Gurub
        { 51904, 12,  4 }, // Ruins of Ahn'Qiraj
        { 51904, 15,  4 }, // Temple of Ahn'Qiraj
        { 51900, 10,  8 }, // Shattrath
        { 51900, 15,  8 }, // submenú Terrallende
        { 51900, 18,  8 }, // submenú mazmorras TBC
        { 51902,  0,  8 }, // Auchindoun
        { 51902,  2,  8 }, // Coilfang Reservoir
        { 51902,  3,  8 }, // Hellfire Citadel
        { 51902,  5,  8 }, // The Mechanar
        { 51902,  6,  8 }, // The Botanica
        { 51904,  5,  8 }, // Gruul's Lair
        { 51904,  6,  8 }, // Magtheridon's Lair
        { 51904,  8,  8 }, // Karazhan
        { 51907,  0,  8 }, // Hellfire Peninsula
        { 51907,  1,  8 }, // Zangarmarsh
        { 51907,  2,  8 }, // Terokkar Forest
        { 51907,  3,  8 }, // Nagrand
        { 51907,  4,  8 }, // Blade's Edge Mountains
        { 51907,  5,  8 }, // Netherstorm
        { 51907,  6,  8 }, // Shadowmoon Valley
        { 51904,  3,  9 }, // Serpentshrine Cavern
        { 51904, 14,  9 }, // The Eye
        { 51904, 22,  9 }, // Zul'Aman
        { 51904,  0, 10 }, // Black Temple
        { 51904,  2, 10 }, // Hyjal Summit
        { 51902,  4, 12 }, // Magisters' Terrace
        { 51904, 13, 12 }, // Sunwell Plateau
        { 51905, 23, 12 }, // Isle of Quel'Danas
        { 51900,  9, 13 }, // Dalaran
        { 51900, 16, 13 }, // submenú Rasganorte
        { 51900, 19, 13 }, // submenú mazmorras WotLK
        { 51903,  0, 13 }, // Azjol-Nerub
        { 51903,  1, 13 }, // The Culling of Stratholme
        { 51903,  3, 13 }, // Drak'Tharon Keep
        { 51903,  4, 13 }, // Gundrak
        { 51903,  6, 13 }, // The Nexus
        { 51903,  7, 13 }, // The Violet Hold
        { 51903,  8, 13 }, // Halls of Lightning
        { 51903,  9, 13 }, // Halls of Stone
        { 51903, 10, 13 }, // Utgarde Keep
        { 51903, 11, 13 }, // Utgarde Pinnacle
        { 51904, 10, 13 }, // Naxxramas
        { 51904, 17, 13 }, // The Obsidian Sanctum
        { 51904, 19, 13 }, // Vault of Archavon
        { 51908,  0, 13 }, // Borean Tundra
        { 51908,  1, 13 }, // Howling Fjord
        { 51908,  2, 13 }, // Dragonblight
        { 51908,  3, 13 }, // Grizzly Hills
        { 51908,  4, 13 }, // Zul'Drak
        { 51908,  5, 13 }, // Sholazar Basin
        { 51908,  6, 13 }, // Crystalsong Forest
        { 51908,  7, 13 }, // The Storm Peaks
        { 51908,  8, 13 }, // Icecrown
        { 51908,  9, 13 }, // Wintergrasp
        { 51904, 18, 14 }, // Ulduar
        { 51903,  2, 15 }, // Trial of the Champion
        { 51904,  4, 15 }, // Trial of the Crusader
        { 51903,  5, 16 }, // The Frozen Halls
        { 51904,  7, 16 }  // Icecrown Citadel
    };

    struct Travel
    {
        Destination Dest;
        float StartX;
        float StartY;
        float StartZ;
        uint32 Refund;
    };

    std::unordered_map<uint32, Destination> destinations;
    std::unordered_map<ObjectGuid, time_t> cooldownUntil;
    std::unordered_map<ObjectGuid, Travel> travelling;

    constexpr uint32 MenuKey(uint32 menuId, uint32 optionId)
    {
        return (menuId << 16) | (optionId & 0xFFFF);
    }

    bool IsEnabled()
    {
        return sConfigMgr->GetOption<bool>("WarcraftedTeleport.Enable", true);
    }

    bool TierGateActive()
    {
        std::string mode = sConfigMgr->GetOption<std::string>("WarcraftedTeleport.ForceTierGate", "auto");
        std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

        if (mode == "on")
            return true;
        if (mode == "off")
            return false;

        return sConfigMgr->GetOption<bool>("IndividualProgression.Enable", false);
    }

    bool HasGmBypass(Player* player)
    {
        return player->IsGameMaster() && sConfigMgr->GetOption<bool>("WarcraftedTeleport.GmBypass", false);
    }

    uint32 RequiredTier(uint32 menuId, uint32 optionId)
    {
        for (TierRequirement const& req : TIER_GATE)
            if (req.MenuId == menuId && req.OptionId == optionId)
                return req.Tier;

        return 0;
    }

    bool CanSee(Player* player, Creature* creature, GossipMenuItems const& item)
    {
        if (HasGmBypass(player))
            return true;

        if (!sConditionMgr->IsObjectMeetToConditions(player, creature, item.Conditions))
            return false;

        uint32 tier = RequiredTier(item.MenuID, item.OptionID);
        if (!tier || !TierGateActive())
            return true;

        return player->GetQuestRewardStatus(IP_TIER_QUEST_BASE + tier);
    }

    GossipMenuItems const* FindMenuItem(uint32 menuId, uint32 optionId)
    {
        GossipMenuItemsMapBounds bounds = sObjectMgr->GetGossipMenuItemsMapBounds(menuId);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
            if (itr->second.OptionID == optionId)
                return &itr->second;

        return nullptr;
    }

    uint32 TeleportCost(Player* player)
    {
        if (HasGmBypass(player))
            return 0;

        uint32 base = sConfigMgr->GetOption<uint32>("WarcraftedTeleport.Cost.Base", 0);
        uint32 perLevel = sConfigMgr->GetOption<uint32>("WarcraftedTeleport.Cost.PerLevel", 0);

        return base + perLevel * player->GetLevel();
    }

    std::string FormatMoney(uint32 copper)
    {
        uint32 gold = copper / 10000;
        uint32 silver = (copper % 10000) / 100;
        uint32 bronze = copper % 100;

        std::string result;
        if (gold)
            result += Acore::StringFormat("{}|TInterface\\MoneyFrame\\UI-GoldIcon:0|t ", gold);
        if (silver || gold)
            result += Acore::StringFormat("{}|TInterface\\MoneyFrame\\UI-SilverIcon:0|t ", silver);
        result += Acore::StringFormat("{}|TInterface\\MoneyFrame\\UI-CopperIcon:0|t", bronze);

        return result;
    }

    void BuildMenu(Player* player, Creature* creature, uint32 menuId)
    {
        ClearGossipMenuFor(player);

        uint32 cost = TeleportCost(player);
        uint32 shown = 0;

        GossipMenuItemsMapBounds bounds = sObjectMgr->GetGossipMenuItemsMapBounds(menuId);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            GossipMenuItems const& item = itr->second;
            if (!CanSee(player, creature, item))
                continue;

            // GossipMenu::AddMenuItem revienta con un ASSERT por encima de GOSSIP_MAX_MENU_ITEMS.
            if (shown >= GOSSIP_MAX_MENU_ITEMS)
                break;

            bool isDestination = destinations.find(MenuKey(menuId, item.OptionID)) != destinations.end();
            AddGossipItemFor(player, item.OptionIcon, item.OptionText, menuId, item.OptionID,
                             item.BoxText, isDestination ? cost : 0, false);
            ++shown;
        }

        SendGossipMenuFor(player, player->GetGossipTextId(menuId, creature), creature->GetGUID());
    }

    void CancelTravel(Player* player, bool notify)
    {
        auto itr = travelling.find(player->GetGUID());
        if (itr == travelling.end())
            return;

        uint32 refund = itr->second.Refund;
        travelling.erase(itr);
        cooldownUntil.erase(player->GetGUID());
        player->ClearEmoteState();

        if (refund)
            player->ModifyMoney(int32(refund));

        if (notify)
            ChatHandler(player->GetSession()).PSendModuleSysMessage(MODULE_NAME, STR_INTERRUPTED);
    }

    void ScheduleTick(Player* player, uint32 remaining)
    {
        // m_Events pertenece al propio jugador y se destruye con él, así que capturar el
        // puntero no puede dejar un evento colgando sobre memoria liberada.
        player->m_Events.AddEventAtOffset([player, remaining]()
        {
            auto itr = travelling.find(player->GetGUID());
            if (itr == travelling.end())
                return;

            Travel const travel = itr->second;

            if (player->IsInCombat() ||
                player->GetDistance(travel.StartX, travel.StartY, travel.StartZ) > TRAVEL_MOVE_TOLERANCE)
            {
                CancelTravel(player, true);
                return;
            }

            if (remaining <= 1)
            {
                travelling.erase(itr);
                player->ClearEmoteState();
                player->TeleportTo(travel.Dest.Map, travel.Dest.X, travel.Dest.Y, travel.Dest.Z, travel.Dest.O);
                return;
            }

            ChatHandler(player->GetSession()).PSendModuleSysMessage(MODULE_NAME, STR_COUNTDOWN, remaining - 1);
            ScheduleTick(player, remaining - 1);
        }, Seconds(1));
    }

    void StartTravel(Player* player, Destination const& dest)
    {
        ObjectGuid guid = player->GetGUID();
        ChatHandler handler(player->GetSession());

        if (travelling.find(guid) != travelling.end())
        {
            CloseGossipMenuFor(player);
            return;
        }

        bool bypass = HasGmBypass(player);

        if (!bypass)
        {
            time_t now = GameTime::GetGameTime().count();
            auto cd = cooldownUntil.find(guid);
            if (cd != cooldownUntil.end() && cd->second > now)
            {
                handler.PSendModuleSysMessage(MODULE_NAME, STR_COOLDOWN, uint32(cd->second - now));
                CloseGossipMenuFor(player);
                return;
            }
        }

        uint32 cost = TeleportCost(player);
        if (cost)
        {
            if (!player->HasEnoughMoney(int32(cost)))
            {
                handler.PSendModuleSysMessage(MODULE_NAME, STR_NOT_ENOUGH_MONEY, FormatMoney(cost));
                CloseGossipMenuFor(player);
                return;
            }

            player->ModifyMoney(-int32(cost));
            handler.PSendModuleSysMessage(MODULE_NAME, STR_CHARGED, FormatMoney(cost));
        }

        uint32 cooldown = bypass ? 0 : sConfigMgr->GetOption<uint32>("WarcraftedTeleport.Cooldown", 0);
        if (cooldown)
            cooldownUntil[guid] = GameTime::GetGameTime().count() + cooldown;

        CloseGossipMenuFor(player);

        uint32 castTime = bypass ? 0 : sConfigMgr->GetOption<uint32>("WarcraftedTeleport.CastTime", 0);
        if (!castTime)
        {
            player->TeleportTo(dest.Map, dest.X, dest.Y, dest.Z, dest.O);
            return;
        }

        Travel& travel = travelling[guid];
        travel.Dest = dest;
        travel.Refund = cost;
        player->GetPosition(travel.StartX, travel.StartY, travel.StartZ);

        player->SetEmoteState(EMOTE_STATE_SPELL_CHANNEL_OMNI);
        handler.PSendModuleSysMessage(MODULE_NAME, STR_COUNTDOWN, castTime);
        ScheduleTick(player, castTime);
    }
}

class npc_warcrafted_teleport : public CreatureScript
{
public:
    npc_warcrafted_teleport() : CreatureScript("npc_warcrafted_teleport") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!IsEnabled())
            return false;

        if (HasGmBypass(player))
            ChatHandler(player->GetSession()).PSendModuleSysMessage(MODULE_NAME, STR_GM_MODE);

        BuildMenu(player, creature, creature->GetGossipMenuId());
        return true;
    }

    // sender y action los fija BuildMenu, así que aquí llegan como (menu, opción). El core
    // no los rellenaría así para un menú construido desde SQL: valdrían 0 y el OptionType.
    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (!IsEnabled())
            return false;

        GossipMenuItems const* item = FindMenuItem(sender, action);
        if (!item || !CanSee(player, creature, *item))
        {
            CloseGossipMenuFor(player);
            return true;
        }

        auto dest = destinations.find(MenuKey(sender, action));
        if (dest == destinations.end())
        {
            if (item->ActionMenuID)
                BuildMenu(player, creature, item->ActionMenuID);
            else
                CloseGossipMenuFor(player);

            return true;
        }

        StartTravel(player, dest->second);
        return true;
    }
};

class WarcraftedTeleportWorldScript : public WorldScript
{
public:
    WarcraftedTeleportWorldScript() : WorldScript("WarcraftedTeleportWorldScript") { }

    void OnStartup() override
    {
        destinations.clear();

        QueryResult result = WorldDatabase.Query(
            "SELECT `menu_id`, `option_id`, `map`, `position_x`, `position_y`, `position_z`, `orientation` "
            "FROM `mod_warcrafted_teleport_destinations`");

        if (!result)
        {
            LOG_ERROR("server.loading", ">> mod-warcrafted-teleport: sin destinos, ¿falta importar data/sql/world/base/mod_teleport_base.sql?");
            return;
        }

        do
        {
            Field* fields = result->Fetch();
            uint32 menuId = fields[0].Get<uint32>();
            uint32 optionId = fields[1].Get<uint32>();

            destinations[MenuKey(menuId, optionId)] =
            {
                fields[2].Get<uint32>(),
                fields[3].Get<float>(),
                fields[4].Get<float>(),
                fields[5].Get<float>(),
                fields[6].Get<float>()
            };
        } while (result->NextRow());

        LOG_INFO("server.loading", ">> mod-warcrafted-teleport: {} destinos cargados.", destinations.size());
    }
};

class WarcraftedTeleportPlayerScript : public PlayerScript
{
public:
    WarcraftedTeleportPlayerScript() : PlayerScript("WarcraftedTeleportPlayerScript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_LOGOUT
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (IsEnabled() && sConfigMgr->GetOption<bool>("WarcraftedTeleport.Announce", true))
            ChatHandler(player->GetSession()).PSendModuleSysMessage(MODULE_NAME, STR_LOGIN_ANNOUNCE);
    }

    // Desconectarse a media cuenta atrás devuelve el importe: se cobra al empezar el viaje,
    // no al llegar.
    void OnPlayerLogout(Player* player) override
    {
        CancelTravel(player, false);
        travelling.erase(player->GetGUID());
        cooldownUntil.erase(player->GetGUID());
    }
};

void AddWarcraftedTeleportScripts()
{
    new npc_warcrafted_teleport();
    new WarcraftedTeleportWorldScript();
    new WarcraftedTeleportPlayerScript();
}
