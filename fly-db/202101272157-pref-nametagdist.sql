-- custom nametags were added, users change adjust the maximum render distance
ALTER TABLE usr ADD COLUMN nametagdist SMALLINT UNSIGNED NOT NULL DEFAULT 5000 AFTER prefs;
