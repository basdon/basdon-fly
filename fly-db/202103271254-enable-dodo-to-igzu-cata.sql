UPDATE msp SET t = t | 1 WHERE t & 1 = 0 AND name LIKE 'gate P%';
