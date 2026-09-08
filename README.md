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

An **AzerothCore** module (pure SQL, no C++) that adds a teleporter NPC with a gossip menu covering capital cities, classic/TBC/WotLK dungeons, raids and world zones.

This first version targets a plain AzerothCore realm: no Playerbots-specific behavior, no Individual Progression tier gating. Destinations are only restricted by character level and faction, matching stock AzerothCore's own access rules.

## Features

* **One NPC, one menu tree:** capitals (Horde/Alliance/neutral), classic dungeons, TBC dungeons, WotLK dungeons, raids, and a zone browser split by continent.
* **Level and faction gated:** each destination only appears once the character meets the zone's or dungeon's expected level, and faction-specific destinations only show to the matching faction.
* **Pure SQL:** no server restart needed to apply the data — only a database import.

## Installation

1. Clone this repository into your AzerothCore `modules/` directory:
   ```bash
   cd /path/to/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Re-run CMake and rebuild the project (the module ships no C++ yet, but keeps the standard module layout so the build system picks up its SQL).
3. Import `data/sql/world/base/mod_teleport_vanilla.sql` into your `acore_world` database.

## Requirements

* AzerothCore v1.0.0+ (WotLK 3.3.5a)

## License

This module is distributed under the GNU General Public License v2 (GPL-2.0).
