-- I don't think it's needed to normalize this further...
CREATE TABLE IF NOT EXISTS `modelstats` (
  `usr` int NOT NULL,
  `t417` int NOT NULL DEFAULT '0',
  `o417` float NOT NULL DEFAULT '0',
  `t425` int NOT NULL DEFAULT '0',
  `o425` float NOT NULL DEFAULT '0',
  `t447` int NOT NULL DEFAULT '0',
  `o447` float NOT NULL DEFAULT '0',
  `t460` int NOT NULL DEFAULT '0',
  `o460` float NOT NULL DEFAULT '0',
  `t464` int NOT NULL DEFAULT '0',
  `o464` float NOT NULL DEFAULT '0',
  `t465` int NOT NULL DEFAULT '0',
  `o465` float NOT NULL DEFAULT '0',
  `t469` int NOT NULL DEFAULT '0',
  `o469` float NOT NULL DEFAULT '0',
  `t476` int NOT NULL DEFAULT '0',
  `o476` float NOT NULL DEFAULT '0',
  `t487` int NOT NULL DEFAULT '0',
  `o487` float NOT NULL DEFAULT '0',
  `t488` int NOT NULL DEFAULT '0',
  `o488` float NOT NULL DEFAULT '0',
  `t497` int NOT NULL DEFAULT '0',
  `o497` float NOT NULL DEFAULT '0',
  `t501` int NOT NULL DEFAULT '0',
  `o501` float NOT NULL DEFAULT '0',
  `t511` int NOT NULL DEFAULT '0',
  `o511` float NOT NULL DEFAULT '0',
  `t512` int NOT NULL DEFAULT '0',
  `o512` float NOT NULL DEFAULT '0',
  `t513` int NOT NULL DEFAULT '0',
  `o513` float NOT NULL DEFAULT '0',
  `t519` int NOT NULL DEFAULT '0',
  `o519` float NOT NULL DEFAULT '0',
  `t520` int NOT NULL DEFAULT '0',
  `o520` float NOT NULL DEFAULT '0',
  `t548` int NOT NULL DEFAULT '0',
  `o548` float NOT NULL DEFAULT '0',
  `t553` int NOT NULL DEFAULT '0',
  `o553` float NOT NULL DEFAULT '0',
  `t563` int NOT NULL DEFAULT '0',
  `o563` float NOT NULL DEFAULT '0',
  `t577` int NOT NULL DEFAULT '0',
  `o577` float NOT NULL DEFAULT '0',
  `t592` int NOT NULL DEFAULT '0',
  `o592` float NOT NULL DEFAULT '0',
  `t593` int NOT NULL DEFAULT '0',
  `o593` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`usr`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
ALTER TABLE modelstats ADD CONSTRAINT fk_modelstats_usr FOREIGN KEY (usr) REFERENCES usr(i);