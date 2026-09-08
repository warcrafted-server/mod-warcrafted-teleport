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

The module ships one common set of destinations that works unchanged on a plain AzerothCore realm and on one running Playerbots — neither changes what should be reachable. If [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression) is also installed and enabled, this module detects it automatically at startup and layers additional conditions on top, hiding any destination the character's progression tier hasn't unlocked yet. No manual variant selection is needed.

## Features

* **One NPC, one menu tree:** capitals (Horde/Alliance/neutral), classic dungeons, TBC dungeons, WotLK dungeons, raids, and a zone browser split by continent.
* **Level and faction gated:** each destination only appears once the character meets the zone's or dungeon's expected level, and faction-specific destinations only show to the matching faction.
* **Automatic Individual Progression detection:** a `WorldScript` checks `IndividualProgression.Enable`. When it's on, destinations beyond the character's current progression tier are hidden — mirroring which content mod-individual-progression itself has actually unlocked. When it's off (or the module isn't installed), only the level/faction rules apply.
* **Overridable via config:** `WarcraftedTeleport.ForceTierGate` lets you force the tier gate on or off regardless of whether Individual Progression is installed, instead of relying purely on auto-detection.
* **Applies live, no restart needed:** the gate (and its config override) is re-evaluated both at startup and on `.reload config`, so toggling Individual Progression or `ForceTierGate` takes effect immediately.

## Installation

1. Clone this repository into your AzerothCore `modules/` directory:
   ```bash
   cd /path/to/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Re-run CMake and rebuild the project.
3. Import `data/sql/world/base/mod_teleport_base.sql` into your `acore_world` database.
4. Copy `conf/mod_warcrafted_teleport.conf.dist` to `mod_warcrafted_teleport.conf` in your server's config folder.
5. Restart the worldserver. The Individual Progression tier gate (if applicable) is applied automatically.

## Configuration

| Option | Description | Default |
|---|---|---|
| `WarcraftedTeleport.Enable` | Enables or disables the module entirely, including the automatic Individual Progression detection. | `1` |
| `WarcraftedTeleport.ForceTierGate` | `auto` detects Individual Progression and gates only if it's enabled. `on` always gates by tier, even without the module installed. `off` never gates by tier, even if Individual Progression is installed and enabled. Applies at startup and on `.reload config`, no restart needed. | `auto` |

## Requirements

* AzerothCore v1.0.0+ (WotLK 3.3.5a)
* Optional: [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), to enable the automatic tier gate.

## License

This module is distributed under the GNU General Public License v2 (GPL-2.0).
