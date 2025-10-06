ALTER TABLE art ADD COLUMN deleted_at INT AFTER pageviews;
UPDATE art SET deleted_at=UNIX_TIMESTAMP() WHERE name='Article_Formatting' LIMIT 1;
