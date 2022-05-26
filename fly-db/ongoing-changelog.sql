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

INSERT INTO chg(stamp,entry) VALUES
('2021 Jun 12', 'Changed SALS taxi lights to an object model without collision');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 06', 'Passengers can now also see the passenger satisfaction of the mission of the vehicle they are in');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 08', 'You now need the correct class to start missions (cargo missions require cargo pilot class etc)');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 08', 'Changed mission point colors: red = wrong class, blue = wrong vehicle, green = available');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 17', 'Updated PINE map, made it bigger and combined its non-heli mission points');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 21', 'New destination: Santa Rosa Airport (STRS)');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 22', 'Fixed a bug where random objects would disappear');

INSERT INTO chg(stamp,entry) VALUES
('2021 Jul 29', 'Made PINE runway longer, fixed STRS misaligned taxi lights');

INSERT INTO chg(stamp,entry) VALUES
('2021 Oct 10', 'Fixed game crashes due to DUNE not completely unloading, overflowing the client when loading other islands');

INSERT INTO chg(stamp,entry) VALUES
('2021 Oct 18', 'Fuel will not drain anymore while being afk');

INSERT INTO chg(stamp,entry) VALUES
('2021 Oct 31', 'New destination: Windmill Resort (WMRE)');

INSERT INTO chg(stamp,entry) VALUES
('2022 May 06', 'Flights will now be paused when leaving the server, they can be resumed when reconnecting');

INSERT INTO chg(stamp,entry) VALUES
('2022 May 26', 'Fixed some issues regarding to continuing paused flights');
