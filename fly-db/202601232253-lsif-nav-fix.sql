-- LSIF shouldn't have VOR on 35 because that approach is blocked by many trees

UPDATE rnw SET n=0 WHERE a=(SELECT i FROM apt WHERE c='LSIF' LIMIT 1) AND h=350 LIMIT 1;
