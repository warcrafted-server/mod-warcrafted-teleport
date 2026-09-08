<p align="center">
  <a href="https://github.com/warcrafted-server">
    <img src="https://raw.githubusercontent.com/warcrafted-server/WarCrafted-ControlP/main/Logo_github.jpg" alt="WarCrafted Universe Header" />
  </a>
</p>

<p align="center">
  🌐 <b>Idiomas / Languages:</b> <a href="README.es.md">Español 🇪🇸</a> | <a href="README.md">English 🇬🇧</a>
</p>

---

## Módulo Warcrafted Teleport
- Estado de la última compilación con azerothcore: [![Build Status](https://github.com/warcrafted-server/mod-warcrafted-teleport/actions/workflows/core-build.yml/badge.svg?branch=main)](https://github.com/warcrafted-server/mod-warcrafted-teleport/actions) ![WoW Version](https://img.shields.io/badge/WoW-3.3.5a-blue) ![Last Commit](https://img.shields.io/github/last-commit/warcrafted-server/mod-warcrafted-teleport)

# mod-warcrafted-teleport

Un módulo para **AzerothCore** (SQL puro, sin C++) que añade un NPC transportador con un menú de destinos: capitales, mazmorras clásicas/TBC/WotLK, raids y zonas del mundo.

Esta primera versión está pensada para un reino de AzerothCore estándar: sin comportamiento específico de Playerbots ni restricciones por nivel de Individual Progression. Los destinos solo se restringen por nivel de personaje y facción, igual que las reglas de acceso del AzerothCore base.

## Características

* **Un solo NPC, un árbol de menús:** capitales (Horda/Alianza/neutral), mazmorras clásicas, mazmorras TBC, mazmorras WotLK, raids, y un explorador de zonas dividido por continente.
* **Restringido por nivel y facción:** cada destino solo aparece cuando el personaje alcanza el nivel esperado de esa zona o mazmorra, y los destinos específicos de facción solo se muestran a la facción correspondiente.
* **SQL puro:** no hace falta reiniciar el servidor para aplicar los datos, solo una importación a la base de datos.

## Instalación

1. Clona este repositorio dentro del directorio `modules/` de tu AzerothCore:
   ```bash
   cd /ruta/a/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Vuelve a ejecutar CMake y recompila el proyecto (el módulo aún no trae C++, pero mantiene la estructura estándar para que el sistema de compilación recoja su SQL).
3. Importa `data/sql/world/base/mod_teleport_vanilla.sql` en tu base de datos `acore_world`.

## Requisitos

* AzerothCore v1.0.0+ (WotLK 3.3.5a)

## Licencia

Este módulo se distribuye bajo la licencia GNU General Public License v2 (GPL-2.0).
