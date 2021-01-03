-- make all LVA heli points for passenger only
UPDATE msp SET t=64 WHERE i=7 OR i=8 OR i=9;
-- convert northern LVA cargo point to heli cargo
UPDATE msp SET t=128,name='cargo H' WHERE i=6;
-- convert one of the SFA heli points to heli cargo
UPDATE msp SET t=128,name='cargo H' WHERE i=23;
-- convert other SFA heli point to exclusive heli passengers
UPDATE msp SET t=64 WHERE i=22;
-- convert LSA heli point to exclusive heli passengers
UPDATE msp SET t=64 WHERE i=33;
-- split OCTA heli points to passengers/cargo
UPDATE msp SET t=64 WHERE i=40;
UPDATE msp SET t=128,name='cargo h' WHERE i=39;
UPDATE msp SET name='cargo a' WHERE i=36;
