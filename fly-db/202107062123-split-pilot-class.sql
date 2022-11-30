-- pilot(1) is split in passenger_pilot(1) and cargo_pilot(2) and everything else needs to be << 1
UPDATE spw SET class=(class&1)|((class&1)<<1)|((class&(~1))<<1);
