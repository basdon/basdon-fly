-- add MISSION_TYPE_PASSENGER_S types to P-gate missionpoints
UPDATE msp SET t=t|1|2 WHERE name LIKE 'gate P%';

-- fix capitalization issues added in changes of previous file
UPDATE msp SET name='cargo H' WHERE i=39;
UPDATE msp SET name='cargo A' WHERE i=36;
