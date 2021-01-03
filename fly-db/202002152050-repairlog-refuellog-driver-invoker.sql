-- change existing column 'player' to 'driver', and make it nullable
START TRANSACTION;
ALTER TABLE refuellog DROP FOREIGN KEY fk_refuellog_player;
ALTER TABLE refuellog CHANGE COLUMN player driver INT;
ALTER TABLE refuellog MODIFY driver INT NULL;
ALTER TABLE refuellog ADD CONSTRAINT fk_refuellog_driver FOREIGN KEY (driver) REFERENCES usr(i);
COMMIT;
START TRANSACTION;
ALTER TABLE repairlog DROP FOREIGN KEY fk_repairlog_player;
ALTER TABLE repairlog CHANGE COLUMN player driver INT;
ALTER TABLE repairlog MODIFY driver INT NULL;
ALTER TABLE repairlog ADD CONSTRAINT fk_repairlog_driver FOREIGN KEY (driver) REFERENCES usr(i);
COMMIT;

-- add column 'invokr' (-e because 'invoker' is a keyword)
START TRANSACTION;
ALTER TABLE refuellog ADD COLUMN invokr INT NOT NULL AFTER driver;
UPDATE refuellog SET invokr=driver;
ALTER TABLE refuellog ADD CONSTRAINT fk_refuellog_invokr FOREIGN KEY (invokr) REFERENCES usr(i);
COMMIT;
START TRANSACTION;
ALTER TABLE repairlog ADD COLUMN invokr INT NOT NULL AFTER driver;
UPDATE repairlog SET invokr=driver;
ALTER TABLE repairlog ADD CONSTRAINT fk_repairlog_invokr FOREIGN KEY (invokr) REFERENCES usr(i);
COMMIT;
