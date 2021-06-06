-- This file is amended whenever a new changelog entry is added, instead of creating different files for each entry.
INSERT INTO chg(stamp,entry) VALUES
('2021 Feb 07', 'Fixed selecting cargo in the /w menu setting a checkpoint to a passenger point instead');

INSERT INTO chg(stamp,entry) VALUES
('2021 Feb 14', 'Added Iguazu City map/destination');

INSERT INTO chg(stamp,entry) VALUES
('2021 Feb 14', 'Fixed a "ghost door" issue with certain planes after /fix');

INSERT INTO chg(stamp,entry) VALUES
('2021 Feb 15', 'Added 2 rustlers at Area 69');

INSERT INTO chg(stamp,entry) VALUES
('2021 Feb 15', 'Slightly changed the mission help/mission map menus');

INSERT INTO chg(stamp,entry) VALUES
('2021 Mar 18', 'Fixed nametags not disappearing after player disconnect');

INSERT INTO chg(stamp,entry) VALUES
('2021 Mar 19', 'Added BNS Atlantic map/destination (military)');

INSERT INTO chg(stamp,entry) VALUES
('2021 Mar 19', 'Semi-fixed retextured objects sometimes being too bright/dark');

INSERT INTO chg(stamp,entry) VALUES
('2021 Mar 25', 'Fixed new players not being able to register on connect');

INSERT INTO chg(stamp,entry) VALUES
('2021 Mar 27', 'Redesigned the mission map, removed random preselection of destinations');

INSERT INTO chg(stamp,entry) VALUES
('2021 Apr 11', 'Added /cmds (/commandlist /commands /cmdlist) and /helpcmd');

INSERT INTO chg(stamp,entry) VALUES
('2021 Apr 11', 'Fix not getting a weather bonus if the weather is already bad when starting a mission');

INSERT INTO chg(stamp,entry) VALUES
('2021 Apr 23', 'Changed OCTA taxi lights to an object model without collision');

INSERT INTO chg(stamp,entry) VALUES
('2021 Apr 23', 'New destination: Sandy Dunes (DUNE)');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 01', 'Significantly increased the range of ILS & lowered its glidescope');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 02', 'Added a number to ILS which shows the projected landing position using the current descend rate');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 13', 'Added /help /helpnav /helpadf /helpvor /helpils /helpmissions /helpaccount /helpcopilot /info /credits');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 18', 'Added a radio system, chat msgs prefixed with # are sent as a radio msg - see /helpradio and /radiomacros');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 24', 'Updated Catalina City (CATA) map');

INSERT INTO chg(stamp,entry) VALUES
('2021 May 30', 'New destination: Angel Pine Airstrip (PINE)');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jun 06', 'Finished flights will now be posted in the #flight-log channel on our Discord server');
