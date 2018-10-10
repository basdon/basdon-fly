
/* vim: set filetype=c ts=8 noexpandtab: */

struct zone {
	float x1, y1, z1, x2, y2, z2;
	int id;
};

struct region {
	struct zone zone;
	int minzone, maxzone;
};

