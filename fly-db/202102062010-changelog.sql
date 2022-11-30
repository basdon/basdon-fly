CREATE TABLE IF NOT EXISTS chg (
  id INT NOT NULL AUTO_INCREMENT,
  stamp char(11) NOT NULL,
  entry char(128) NOT NULL,
  PRIMARY KEY (id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO chg(stamp,entry) VALUES
('2020 Feb 12', 'Added a kneeboard that shown info about your current mission'),
('2020 May 22', 'All spawn locations now have vehicles'),
('2020 Jun 02', 'Detailed information about navigation is not available on the website'),
('2020 Aug 12', 'New destination: Octavia Island (OCTA)'),
('2020 Sep 22', 'Added a mission map you can use to choose a mission, instead of giving random missions'),
('2020 Sep 28', 'Greatly updated the flight map on the website'),
('2020 Nov 01', 'New destination: Catalina City (CATA)'),
('2021 Jan 17', 'Passenger/copilot feature, disabled vehicle weapons, code stability improvements'),
('2021 Jan 23', 'The kneeboard can now be hidden by using /p'),
('2021 Jan 27', 'Added custom nametag labels, use /p to adjust their maximum distance'),
('2021 Jan 30', 'Added /androm(ada) to teleport into an andromada as driver (for people with mods)'),
('2021 Feb 06', 'Login messages now show when the player last connected'),
('2021 Feb 06', 'Added a changelog (/changelog or /updates)');
