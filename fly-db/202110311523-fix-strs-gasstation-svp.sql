-- The gasstation service point is too low,
-- so the label doesn't show because it tests line-of-sight but it collides with the gound.
UPDATE svp SET z=13.3766 WHERE apt=14 AND x>-11000;
