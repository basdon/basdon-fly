-- use BASE for a69 instead of XXXX
UPDATE apt SET c='BASE' WHERE c='XXXX';
ALTER TABLE apt DROP COLUMN b;
