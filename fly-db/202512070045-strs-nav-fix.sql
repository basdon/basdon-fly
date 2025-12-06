-- STRS was only supposed to have nav on runways 90L and 27L, but it was put on 90R and 27L
-- remove nav on 90R and add nav on 90L

UPDATE rnw SET n=0 WHERE a=14 AND h=90 AND s='R';
UPDATE rnw SET n=6 WHERE a=14 AND h=90 AND s='L';
