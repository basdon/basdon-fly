ALTER TABLE ses ADD COLUMN netgameversion INT NULL;
UPDATE ses SET netgameversion=4057;
ALTER TABLE ses MODIFY netgameversion INT NOT NULL;

ALTER TABLE ses ADD COLUMN versionstring CHAR(24) NULL;
