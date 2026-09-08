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

El módulo trae un único conjunto de destinos que funciona igual en un reino de AzerothCore estándar y en uno con Playerbots activo. Si además tienes instalado y activo [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), el módulo oculta cualquier destino que el tier de progresión del personaje aún no haya desbloqueado.

## Características

* **Un solo NPC, un árbol de menús:** capitales (Horda/Alianza/neutral), mazmorras clásicas, mazmorras TBC, mazmorras WotLK, raids, y un explorador de zonas dividido por continente.
* **Restringido por nivel y facción:** cada destino solo aparece cuando el personaje alcanza el nivel esperado de esa zona o mazmorra, y los destinos específicos de facción solo se muestran a la facción correspondiente.
* **Bloqueo por tier de Individual Progression:** el tier del personaje se lee en vivo, así que desbloquear un tier abre sus destinos la próxima vez que abre el menú. Sin reiniciar, sin recargar y sin volver a entrar.
* **Forzable por configuración:** `WarcraftedTeleport.ForceTierGate` fuerza el bloqueo por tier activado o desactivado independientemente de si Individual Progression está instalado.
* **Modo GM para depurar:** con `WarcraftedTeleport.GmBypass` activo, los game masters en modo GM ven todos los destinos ignorando nivel, facción y tier, y viajan gratis y al instante. Los jugadores normales no se ven afectados.
* **Coste en oro opcional:** un precio fijo por viaje escalado por el nivel del personaje, visible en el aviso de confirmación del cliente y cobrado en el servidor.
* **Cooldown y canalización opcionales:** una espera entre viajes, y una animación de canalización con cuenta atrás por chat antes de viajar. Alejarse o entrar en combate cancela el viaje y devuelve el importe.
* **Todo se aplica en caliente:** cada opción se lee en el momento de usarla, así que `.reload config` surte efecto al instante.

## Instalación

1. Clona este repositorio dentro del directorio `modules/` de tu AzerothCore:
   ```bash
   cd /ruta/a/azerothcore-wotlk/modules
   git clone https://github.com/warcrafted-server/mod-warcrafted-teleport.git
   ```
2. Vuelve a ejecutar CMake y recompila el proyecto.
3. Importa `data/sql/world/base/mod_teleport_base.sql` y `data/sql/db-world/mod-warcrafted-teleport-strings.sql` en tu base de datos `acore_world`.
4. Copia `conf/mod_warcrafted_teleport.conf.dist` a `mod_warcrafted_teleport.conf` en la carpeta de configuración de tu servidor.
5. Reinicia el worldserver.

## Configuración

| Opción | Descripción | Por defecto |
|---|---|---|
| `WarcraftedTeleport.Enable` | Habilita o deshabilita el módulo. Desactivado, el NPC conserva su menú pero deja de transportar a nadie. | `1` |
| `WarcraftedTeleport.Announce` | Anuncia el módulo por chat al iniciar sesión. | `1` |
| `WarcraftedTeleport.ForceTierGate` | `auto` bloquea solo si Individual Progression está activo. `on` bloquea siempre por tier. `off` no bloquea nunca por tier. | `auto` |
| `WarcraftedTeleport.GmBypass` | Los game masters en modo GM ven todos los destinos y viajan gratis y al instante. | `0` |
| `WarcraftedTeleport.Cost.Base` | Parte fija del precio de un viaje, en cobre. | `0` |
| `WarcraftedTeleport.Cost.PerLevel` | Cobre añadido al precio por nivel del personaje. | `0` |
| `WarcraftedTeleport.Cooldown` | Segundos que espera un personaje entre viajes. | `0` |
| `WarcraftedTeleport.CastTime` | Segundos de canalización antes de viajar. | `0` |

## Editar destinos

Los destinos viven en la tabla `mod_warcrafted_teleport_destinations`, indexados por el menú y la opción a los que pertenecen, y se cargan al arrancar el worldserver. Las entradas del menú son filas normales de `gossip_menu_option`, y sus requisitos de nivel y facción, filas normales de `conditions`. Los requisitos de tier son la excepción: son una tabla en `src/mod_warcrafted_teleport.cpp`, porque hay que comprobarlos contra el personaje en vivo en vez de guardarlos como condiciones estáticas.

El NPC no usa SmartAI. SmartAI transporta al jugador antes de que ningún hook de script pueda cobrarle, comprobar un cooldown o saltarse el bloqueo para un GM, así que el módulo gestiona el menú entero por su cuenta.

## Requisitos

* AzerothCore v1.0.0+ (WotLK 3.3.5a)
* Opcional: [mod-individual-progression](https://github.com/warcrafted-server/mod-individual-progression), para el bloqueo por tier.

## Licencia

Este módulo se distribuye bajo la licencia GNU General Public License v2 (GPL-2.0).
