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

Un módulo para **AzerothCore** que añade un NPC transportador con un menú de destinos: capitales, mazmorras clásicas/TBC/WotLK, raids y zonas del mundo.

El módulo trae un único conjunto de destinos que funciona igual en un reino de AzerothCore estándar y en uno con Playerbots activo: ninguno de los dos cambia qué debería ser alcanzable. Si además tienes instalado y activo [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), este módulo lo detecta automáticamente al arrancar y añade condiciones extra por encima, ocultando cualquier destino que el tier de progresión del personaje aún no haya desbloqueado. No hace falta elegir ninguna variante a mano.

## Características

* **Un solo NPC, un árbol de menús:** capitales (Horda/Alianza/neutral), mazmorras clásicas, mazmorras TBC, mazmorras WotLK, raids, y un explorador de zonas dividido por continente.
* **Restringido por nivel y facción:** cada destino solo aparece cuando el personaje alcanza el nivel esperado de esa zona o mazmorra, y los destinos específicos de facción solo se muestran a la facción correspondiente.
* **Detección automática de Individual Progression:** un `WorldScript` comprueba `IndividualProgression.Enable`. Si está activo, oculta los destinos por encima del tier de progresión actual del personaje, reflejando qué contenido ha desbloqueado de verdad el propio mod-individual-progression. Si está desactivado (o el módulo no está instalado), solo se aplican las reglas de nivel y facción.
* **Forzable por configuración:** `WarcraftedTeleport.ForceTierGate` permite forzar la puerta por tier activada o desactivada independientemente de si Individual Progression está instalado, en vez de depender solo de la detección automática.
* **Se aplica en caliente, sin reiniciar:** la puerta (y su override de configuración) se reevalúa tanto al arrancar como con `.reload config`, así que activar o desactivar Individual Progression o `ForceTierGate` surte efecto al instante.

## Instalación

1. Clona este repositorio dentro del directorio `modules/` de tu AzerothCore:
   ```bash
   cd /ruta/a/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Vuelve a ejecutar CMake y recompila el proyecto.
3. Importa `data/sql/world/base/mod_teleport_base.sql` en tu base de datos `acore_world`.
4. Copia `conf/mod_warcrafted_teleport.conf.dist` a `mod_warcrafted_teleport.conf` en la carpeta de configuración de tu servidor.
5. Reinicia el worldserver. La puerta por tier de Individual Progression (si aplica) se activa sola.

## Configuración

| Opción | Descripción | Por defecto |
|---|---|---|
| `WarcraftedTeleport.Enable` | Habilita o deshabilita el módulo por completo, incluida la detección automática de Individual Progression. | `1` |
| `WarcraftedTeleport.ForceTierGate` | `auto` detecta Individual Progression y solo activa la puerta si está habilitado. `on` fuerza siempre la puerta por tier, aunque el módulo no esté instalado. `off` nunca aplica la puerta por tier, aunque Individual Progression esté instalado y activo. Se aplica al arrancar y con `.reload config`, sin reiniciar. | `auto` |

## Requisitos

* AzerothCore v1.0.0+ (WotLK 3.3.5a)
* Opcional: [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), para activar la puerta automática por tier.

## Licencia

Este módulo se distribuye bajo la licencia GNU General Public License v2 (GPL-2.0).
