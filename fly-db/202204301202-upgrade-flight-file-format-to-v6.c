#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define STATIC_ASSERT(E) typedef char __static_assert_[(E)?1:-1]

STATIC_ASSERT(sizeof(char) == 1);
STATIC_ASSERT(sizeof(short) == 2);
STATIC_ASSERT(sizeof(int) == 4);

#define FRAME_FLAG_PAUSED 0x1
#define FRAME_FLAG_ENGINE 0x2

#pragma pack(push,1)
struct magic {
	char version;
	char f, l, y;
};

struct v3header {
	int mission_id;
	float fuel_cap;
	short vehicle_model_id;
	char player_name_length;
	char player_name[25];
};
STATIC_ASSERT(sizeof(struct v3header) == 40 - sizeof(struct magic));

struct v3frame {
	int timestamp;
	char flags;
	char satisfaction;
	short speed;
	short altitude;
	short health;
	float fuel_level;
	float x;
	float y;
};
STATIC_ASSERT(sizeof(struct v3frame) == 24);

struct v4header {
	int mission_id;
	float fuel_cap;
	short vehicle_model_id;
	char player_name_length;
	char player_name[25];
};
STATIC_ASSERT(sizeof(struct v4header) == 40 - sizeof(struct magic));

struct v4frame {
	int timestamp;
	char flags;
	char satisfaction;
	short speed;
	short altitude;
	short health;
	float fuel_level;
	float x;
	float y;
	short bank10;
	short pitch10;
};
STATIC_ASSERT(sizeof(struct v4frame) == 28);

struct v5header {
	int mission_id;
	float fuel_cap;
	short vehicle_model_id;
	char player_name_length;
	char player_name[25];
};
STATIC_ASSERT(sizeof(struct v5header) == 40 - sizeof(struct magic));

struct v5frame {
	int timestamp;
	char flags;
	char satisfaction;
	short speed;
	short altitude;
	short health;
	float fuel_level;
	float x;
	float y;
	short bank10;
	short pitch10;
};
STATIC_ASSERT(sizeof(struct v5frame) == 28);

#define HDR_FLAG_CANARY 0x1
#define HDR_FLAG_HAS_BANK_PITCH 0x2
#define HDR_FLAG_HAS_ALTITUDE 0x4

struct v6header {
	int mission_id;
	short flags;
};
STATIC_ASSERT(sizeof(struct v6header) == 10 - sizeof(struct magic));

struct v6frame {
	int timestamp;
	char flags;
	char satisfaction;
	short speed;
	short altitude;
	short health;
	float fuel_level;
	float x;
	float y;
	short bank10;
	short pitch10;
};
STATIC_ASSERT(sizeof(struct v6frame) == 28);
#pragma pack(pop)

/**
Upgrades all flight files from version 5 to version 6.
Just compile and run in this directory.
*/
int main(int argc, char **argv)
{
	struct {
		int read, v3, v4, v5, v6, errors;
	} stats;
	char fname_in[1000], fname_out[1000], *ext;
	struct v3header v3header;
	struct v4header v4header;
	struct v5header v5header;
	struct v6header v6header;
	struct v3frame v3frame;
	struct v4frame v4frame;
	struct v5frame v5frame;
	struct v6frame v6frame;
	struct magic magic;
	struct dirent *entry;
	FILE *in, *out;
	DIR *fdr;

	stats.read = 0x11223344;
	ext = (char*) &stats.read;
	assert(((void)"machine is not little endian", *ext == 0x44));

	memset(&stats, 0, sizeof(stats));
	assert((fdr = opendir("../fly-web/static/fdr")));
	while ((entry = readdir(fdr))) {
		ext = strstr(entry->d_name, ".flight");
		if (ext && !ext[7]) {
			sprintf(fname_out, "../fly-web/static/fdr/%s~", entry->d_name);
			sprintf(fname_in, "../fly-web/static/fdr/%s", entry->d_name);
			assert((in = fopen(fname_in, "rb")));
			stats.read++;
			assert(fread(&magic, sizeof(magic), 1, in));
			switch (magic.version) {
			case 3:
				stats.v3++;
				magic.version = 6;
				assert((out = fopen(fname_out, "wb")));
				assert(fwrite(&magic, sizeof(magic), 1, out));
				assert(fread(&v3header, sizeof(v3header), 1, in));
				v6header.mission_id = v3header.mission_id;
				v6header.flags = HDR_FLAG_CANARY | HDR_FLAG_HAS_ALTITUDE;
				assert(fwrite(&v6header, sizeof(v6header), 1, out));
				while (fread(&v3frame, sizeof(v3frame), 1, in)) {
					v6frame.timestamp = v3frame.timestamp;
					v6frame.flags = v3frame.flags;
					v6frame.satisfaction = v3frame.satisfaction;
					v6frame.speed = v3frame.speed;
					v6frame.altitude = v3frame.altitude;
					v6frame.health = v3frame.health;
					v6frame.fuel_level = v3frame.fuel_level / v3header.fuel_cap;
					v6frame.x = v3frame.x;
					v6frame.y = v3frame.y;
					v6frame.bank10 = 0;
					v6frame.pitch10 = 0;
					assert(fwrite(&v6frame, sizeof(v6frame), 1, out));
				}
				fclose(out);
				assert(!rename(fname_out, fname_in));
				break;
			case 4:
				stats.v4++;
				magic.version = 6;
				assert((out = fopen(fname_out, "wb")));
				assert(fwrite(&magic, sizeof(magic), 1, out));
				assert(fread(&v4header, sizeof(v4header), 1, in));
				v6header.mission_id = v4header.mission_id;
				/*Flight map version 4 has bogus data for altitude (it's same as pitch10).*/
				v6header.flags = HDR_FLAG_CANARY | HDR_FLAG_HAS_BANK_PITCH;
				assert(fwrite(&v6header, sizeof(v6header), 1, out));
				while (fread(&v4frame, sizeof(v4frame), 1, in)) {
					v6frame.timestamp = v4frame.timestamp;
					v6frame.flags = v4frame.flags;
					v6frame.satisfaction = v4frame.satisfaction;
					v6frame.speed = v4frame.speed;
					/*Flight map version 4 has bogus data for altitude (it's same as pitch10).*/
					v6frame.altitude = 0;
					v6frame.health = v4frame.health;
					v6frame.fuel_level = v4frame.fuel_level / v4header.fuel_cap;
					v6frame.x = v4frame.x;
					v6frame.y = v4frame.y;
					v6frame.bank10 = v4frame.bank10;
					v6frame.pitch10 = v4frame.pitch10;
					assert(fwrite(&v6frame, sizeof(v6frame), 1, out));
				}
				fclose(out);
				assert(!rename(fname_out, fname_in));
				break;
			case 5:
				stats.v5++;
				magic.version = 6;
				assert((out = fopen(fname_out, "wb")));
				assert(fwrite(&magic, sizeof(magic), 1, out));
				assert(fread(&v5header, sizeof(v5header), 1, in));
				v6header.mission_id = v5header.mission_id;
				v6header.flags = HDR_FLAG_CANARY | HDR_FLAG_HAS_ALTITUDE | HDR_FLAG_HAS_BANK_PITCH;
				assert(fwrite(&v6header, sizeof(v6header), 1, out));
				while (fread(&v5frame, sizeof(v5frame), 1, in)) {
					v6frame.timestamp = v5frame.timestamp;
					v6frame.flags = v5frame.flags;
					v6frame.satisfaction = v5frame.satisfaction;
					v6frame.speed = v5frame.speed;
					v6frame.altitude = v5frame.altitude;
					v6frame.health = v5frame.health;
					v6frame.fuel_level = v5frame.fuel_level / v5header.fuel_cap;
					v6frame.x = v5frame.x;
					v6frame.y = v5frame.y;
					v6frame.bank10 = v5frame.bank10;
					v6frame.pitch10 = v5frame.pitch10;
					assert(fwrite(&v6frame, sizeof(v6frame), 1, out));
				}
				fclose(out);
				assert(!rename(fname_out, fname_in));
				break;
			case 6:
				stats.v6++;
				magic.version = 6;
				break;
			default:
				printf("%s: not processing version %d\n", fname_in, magic.version);
				stats.errors++;
				break;
			}
			fclose(in);
		}
	}
	closedir(fdr);
	printf("read %d, upgraded %d, v3: %d, v4: %d, v5: %d, v6: %d, errors %d\n",
		stats.read,
		stats.v3 + stats.v4 + stats.v5,
		stats.v3,
		stats.v4,
		stats.v5,
		stats.v6,
		stats.errors
	);
	return 0;
}
