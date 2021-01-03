-- unlink svp that was moved to a gas station in LS in 20200521003027-salv-sasf-sfca-xxxx.sql
UPDATE svp SET apt=NULL WHERE apt=1 AND y<0.0;
