UPDATE art SET name='SAMP_internals', cat=(SELECT id FROM artcat WHERE name='Internal docs' LIMIT 1) WHERE name='SAMP_Internals' LIMIT 1;
