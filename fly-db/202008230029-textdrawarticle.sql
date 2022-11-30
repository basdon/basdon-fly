INSERT INTO art(cat,name,title) VALUES((SELECT id FROM artcat WHERE name='File Formats'),'Textdraw_file_format','Textdraw file format');
