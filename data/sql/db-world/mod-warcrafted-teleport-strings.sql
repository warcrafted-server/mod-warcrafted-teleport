DELETE FROM `module_string` WHERE `module` = "mod-warcrafted-teleport" AND `id` IN (0, 1, 2, 3, 4, 5, 6);
INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
("mod-warcrafted-teleport", 0, 'This server is running the |cff4CFF00Warcrafted Teleport|r module.'),
("mod-warcrafted-teleport", 1, 'You must wait {} more seconds before travelling again.'),
("mod-warcrafted-teleport", 2, 'You need {} to travel to this destination.'),
("mod-warcrafted-teleport", 3, 'The trip cost you {}.'),
("mod-warcrafted-teleport", 4, 'Opening the portal... {}'),
("mod-warcrafted-teleport", 5, 'The trip was interrupted.'),
("mod-warcrafted-teleport", 6, '|cff4CFF00Warcrafted Teleport|r: GM mode, every destination is visible and the trip is free and instant.');

DELETE FROM `module_string_locale` WHERE `module` = 'mod-warcrafted-teleport' AND `id` IN (0, 1, 2, 3, 4, 5, 6);
INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
('mod-warcrafted-teleport', 0, 'esES', 'Este servidor está ejecutando el módulo |cff4CFF00Warcrafted Teleport|r.'),
('mod-warcrafted-teleport', 1, 'esES', 'Debes esperar {} segundos más antes de volver a viajar.'),
('mod-warcrafted-teleport', 2, 'esES', 'Necesitas {} para viajar a este destino.'),
('mod-warcrafted-teleport', 3, 'esES', 'El viaje te ha costado {}.'),
('mod-warcrafted-teleport', 4, 'esES', 'Abriendo el portal... {}'),
('mod-warcrafted-teleport', 5, 'esES', 'El viaje se ha interrumpido.'),
('mod-warcrafted-teleport', 6, 'esES', '|cff4CFF00Warcrafted Teleport|r: modo GM, se ven todos los destinos y el viaje es gratis e instantáneo.');
