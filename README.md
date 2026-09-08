<p align="center">
  <a href="https://github.com/warcrafted-server">
    <img src="https://raw.githubusercontent.com/warcrafted-server/WarCrafted-ControlP/main/Logo_github.jpg" alt="WarCrafted Universe Header" />
  </a>
</p>

<p align="center">
  🌐 <b>Idiomas / Languages:</b> <a href="README.es.md">Español 🇪🇸</a> | <a href="README.md">English 🇬🇧</a>
</p>

---

## Warcrafted Teleport Module
- Latest build status with azerothcore: [![Build Status](https://github.com/warcrafted-server/mod-warcrafted-teleport/actions/workflows/core-build.yml/badge.svg?branch=main)](https://github.com/warcrafted-server/mod-warcrafted-teleport/actions) ![WoW Version](https://img.shields.io/badge/WoW-3.3.5a-blue) ![Last Commit](https://img.shields.io/github/last-commit/warcrafted-server/mod-warcrafted-teleport)

# mod-warcrafted-teleport

An **AzerothCore** module that adds a teleporter NPC with a gossip menu covering capital cities, classic/TBC/WotLK dungeons, raids and world zones.

The module ships one common set of destinations that works unchanged on a plain AzerothCore realm and on one running Playerbots. If [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression) is also installed and enabled, the module hides any destination the character's progression tier hasn't unlocked yet.

## Features

* **One NPC, one menu tree:** capitals (Horde/Alliance/neutral), classic dungeons, TBC dungeons, WotLK dungeons, raids, and a zone browser split by continent.
* **Level and faction gated:** each destination only appears once the character meets the zone's or dungeon's expected level, and faction-specific destinations only show to the matching faction.
* **Individual Progression tier gate:** the character's tier is read live from mod-individual-progression, so unlocking a tier opens its destinations the next time the menu is opened. No restart, no reload, no relog.
* **Overridable via config:** `WarcraftedTeleport.ForceTierGate` forces the tier gate on or off regardless of whether Individual Progression is installed.
* **GM debug mode:** with `WarcraftedTeleport.GmBypass` on, game masters in GM mode see every destination ignoring level, faction and tier, and travel free and instantly. Regular players are unaffected.
* **Optional gold cost:** a fixed price per trip scaled by character level, shown in the client's confirmation popup and charged server-side.
* **Optional cooldown and channelling:** a wait between trips, and a channel animation with a chat countdown before travelling. Moving away or entering combat cancels the trip and refunds it.
* **Everything applies live:** every option is read at the point of use, so `.reload config` takes effect immediately.

## Installation

1. Clone this repository into your AzerothCore `modules/` directory:
   ```bash
   cd /path/to/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Re-run CMake and rebuild the project.
3. Import `data/sql/world/base/mod_teleport_base.sql` and `data/sql/db-world/mod-warcrafted-teleport-strings.sql` into your `acore_world` database.
4. Copy `conf/mod_warcrafted_teleport.conf.dist` to `mod_warcrafted_teleport.conf` in your server's config folder.
5. Restart the worldserver.

## Configuration

| Option | Description | Default |
|---|---|---|
| `WarcraftedTeleport.Enable` | Enables or disables the module. With it off the NPC keeps its menu but no longer teleports anyone. | `1` |
| `WarcraftedTeleport.Announce` | Announces the module in chat on login. | `1` |
| `WarcraftedTeleport.ForceTierGate` | `auto` gates only if Individual Progression is enabled. `on` always gates by tier. `off` never gates by tier. | `auto` |
| `WarcraftedTeleport.GmBypass` | Game masters in GM mode see every destination and travel free and instantly. | `0` |
| `WarcraftedTeleport.Cost.Base` | Fixed part of the price of one trip, in copper. | `0` |
| `WarcraftedTeleport.Cost.PerLevel` | Copper added to the price per character level. | `0` |
| `WarcraftedTeleport.Cooldown` | Seconds a character waits between trips. | `0` |
| `WarcraftedTeleport.CastTime` | Seconds of channelling before travelling. | `0` |

## Editing destinations

Destinations live in the `mod_warcrafted_teleport_destinations` table, keyed by the gossip menu and option they belong to, and are loaded when the worldserver starts. The menu entries themselves are ordinary `gossip_menu_option` rows, and their level and faction requirements ordinary `conditions` rows. The tier requirements are the one exception: they are a table in `src/mod_warcrafted_teleport.cpp`, because they need to be checked against the live character rather than stored as static conditions.

The NPC does not use SmartAI. SmartAI teleports a player before any script hook can charge them, check a cooldown or grant a GM bypass, so the module drives the whole menu itself.

## Requirements

* AzerothCore v1.0.0+ (WotLK 3.3.5a)
* Optional: [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), for the tier gate.

## License

This module is distributed under the GNU General Public License v2 (GPL-2.0).
