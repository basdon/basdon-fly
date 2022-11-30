START TRANSACTION;

-- flight duration will no longer be 'tlastupdate-tstart' because flight can be paused,
-- so add a dedicated duration column
ALTER TABLE flg
ADD COLUMN duration INT NOT NULL DEFAULT 0 AFTER tunload;
UPDATE flg SET duration=tlastupdate-tstart;

-- paused flights table
CREATE TABLE IF NOT EXISTS pfl (
	fid int NOT NULL,
	t int NOT NULL,
	x float NOT NULL,
	y float NOT NULL,
	z float NOT NULL,
	qw float NOT NULL,
	qx float NOT NULL,
	qy float NOT NULL,
	qz float NOT NULL,
	vx float NOT NULL,
	vy float NOT NULL,
	vz float NOT NULL,
	fuel float NOT NULL,
	hp float NOT NULL,
	gear_keys int NOT NULL,
	udlrkeys int NOT NULL,
	has_misc int NOT NULL,
	misc int NOT NULL,
	reason int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
ALTER TABLE pfl ADD CONSTRAINT fk_pfl_fid FOREIGN KEY (fid) REFERENCES flg(id);

-- make vehicle nullable in refuellog
ALTER TABLE refuellog MODIFY vehicle INT NULL;
-- make vehicle nullable in repairlog
ALTER TABLE repairlog MODIFY vehicle INT NULL;

-- fuel is now in percentage, update refuellog
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/95000.0 WHERE v.m=592; -- androm
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/23000.0 WHERE v.m=577; -- at400
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/518.0 WHERE v.m=511; -- beagle
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/5510.0 WHERE v.m=548; -- cargobob
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/132.0 WHERE v.m=512; -- cropdust
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/285.0 WHERE v.m=593; -- dodo
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/2400.0 WHERE v.m=425; -- hunter
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/3754.0 WHERE v.m=520; -- hydra
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/2925.0 WHERE v.m=417; -- leviathan
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/416.0 WHERE v.m=487; -- maverick
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/3224.0 WHERE v.m=553; -- nevada
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/416.0 WHERE v.m=497; -- police maverick
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/1360.0 WHERE v.m=563; -- raindance
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/1018.0 WHERE v.m=476; -- rustler
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/106.0 WHERE v.m=447; -- sea sparrow
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/4160.0 WHERE v.m=519; -- shamal
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/285.0 WHERE v.m=460; -- skimmer
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/106.0 WHERE v.m=469; -- sparrow
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/91.0 WHERE v.m=513; -- stuntplane
UPDATE refuellog r JOIN veh v ON r.vehicle=v.i SET r.fuel=r.fuel/416.0 WHERE v.m=488; -- vcnmav
UPDATE refuellog r SET r.fuel=r.fuel/1000.0 WHERE r.fuel>1; -- other

-- fuel is now in percentage, update missions
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/95000.0 WHERE v.m=592; -- androm
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/23000.0 WHERE v.m=577; -- at400
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/518.0 WHERE v.m=511; -- beagle
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/5510.0 WHERE v.m=548; -- cargobob
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/132.0 WHERE v.m=512; -- cropdust
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/285.0 WHERE v.m=593; -- dodo
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/2400.0 WHERE v.m=425; -- hunter
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/3754.0 WHERE v.m=520; -- hydra
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/2925.0 WHERE v.m=417; -- leviathan
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/416.0 WHERE v.m=487; -- maverick
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/3224.0 WHERE v.m=553; -- nevada
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/416.0 WHERE v.m=497; -- police maverick
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/1360.0 WHERE v.m=563; -- raindance
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/1018.0 WHERE v.m=476; -- rustler
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/106.0 WHERE v.m=447; -- sea sparrow
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/4160.0 WHERE v.m=519; -- shamal
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/285.0 WHERE v.m=460; -- skimmer
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/106.0 WHERE v.m=469; -- sparrow
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/91.0 WHERE v.m=513; -- stuntplane
UPDATE flg f JOIN veh v ON f.vehicle=v.i SET f.fuel=f.fuel/416.0 WHERE v.m=488; -- vcnmav

COMMIT;
