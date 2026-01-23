INSERT INTO art(cat,name,title) VALUES ((SELECT id FROM artcat WHERE name='News'), 'Year_in_review_2025', 'Year in review 2025');
