#ifndef S3MAP_H
#define S3MAP_H

#include <stdbool.h>
#include <stdlib.h>
#include "s3util.h"

#define S3MAP_HANDLE_EXCEPTION(handle, throws, file, func, line)  \
	S3UTIL_HANDLE_EXCEPTION(&handle->memset, throws, file, func, line);


typedef enum {
	s3map_startres_low = 1,
	s3map_startres_medium = 2,
	s3map_startres_high = 3,
} s3map_startres_type;

typedef enum {
	s3map_roman = 0,
	s3map_egyptians = 1,
	s3map_asians = 2,
	s3map_amazons = 3,
	s3map_nation_choosable = 255,
	s3map_nation_notset = 256,
} s3map_nation_type;

typedef enum {
	s3map_ai = 2,
	s3map_human = 1,
	s3map_player_choosable = 0,
	s3map_player_disabled = 0xFF,
} s3map_player_type;

typedef struct s3map_t s3map_t;
typedef struct s3map_ptr_t s3map_ptr_t;
typedef struct s3map_stack_t s3map_stack_t;
typedef struct s3map_field_t s3map_field_t;
typedef struct s3map_const_t s3map_const_t;
typedef struct s3map_settler_t s3map_settler_t;
typedef struct s3map_building_t s3map_building_t;
typedef struct s3map_playerinfo_t s3map_playerinfo_t;
typedef struct s3map_building_wincond_t s3map_building_wincond_t;
typedef struct s3map_position_wincond_t s3map_position_wincond_t;
typedef struct s3map_production_wincond_t s3map_production_wincond_t;

struct s3map_building_t {
	uint16_t x;
	uint16_t y;

	uint8_t type;
	uint8_t player;

	uint8_t sword1 : 4;
	uint8_t sword2 : 4;
	uint8_t sword3 : 4;

	uint8_t archer1 : 4;
	uint8_t archer2 : 4;
	uint8_t archer3 : 4;

	uint8_t spear1 : 4;
	uint8_t spear2 : 4;
	uint8_t spear3 : 4;
};

struct s3map_stack_t {
	uint16_t x;
	uint16_t y;

	uint8_t type;
	uint8_t size;
};

struct s3map_settler_t {
	uint16_t x;
	uint16_t y;

	uint8_t type;
	uint8_t player;
};

struct s3map_field_t {
	uint8_t height;
	uint8_t type;
	uint8_t mapobject;
	uint8_t player;
	uint8_t resourceinfo;
};

struct s3map_building_wincond_t {
	uint8_t player; // 0xFF: win condition not used
	uint8_t building;
};

struct s3map_position_wincond_t {
	bool enabled;
	uint16_t x;
	uint16_t y;
};

struct s3map_production_wincond_t {
	uint16_t count; // 0 - win condition not used
	uint8_t stacktype;
};

struct s3map_ptr_t {
	uint32_t pos;
	uint32_t arg; // different meanings - arg^2*x or arg*x is the length of this block (x differs too)
};

struct s3map_t {
	s3util_memset_t memset;
	s3util_ioset_t ioset;

	// header
	uint32_t checksum;
	bool contains_amazon;

	// mapinfo
	bool single_player;
	s3map_startres_type startres;
	uint32_t player_count;


	// playerinfo/teaminfo
	s3map_playerinfo_t* playerinfos; // !!IMPORTANT!! you need a nullptr check before access

	uint32_t constellation_count; // max: 10

	char* constellation_name[10];

	// preview
	s3map_ptr_t preview;

	// map
	s3map_ptr_t map;

	// settlers
	s3map_ptr_t settlers;
	// buildings
	s3map_ptr_t buildings;
	// stacks
	s3map_ptr_t stacks;

	// win conditions
	uint32_t kill_wincond : 20; // must player 0-19 be killed

	s3map_building_wincond_t building_wincond[10];

	s3map_position_wincond_t position_wincond[5];

	uint16_t time_wincond[20]; // 0 - ignore

	s3map_production_wincond_t production_wincond[3];

	char* mapdesc;
	char* maptips;
};

struct s3map_playerinfo_t {
	s3map_nation_type nation;
	s3map_player_type type[10]; // constellation specific

	char* name;
	uint32_t startx;
	uint32_t starty;
	uint8_t team[10];// constellation specific
};

void s3map_readfile_func(s3map_t* handle, void* arg,
	bool (*read_func) (void*, void*, size_t),
	bool (*write_func) (void*, void*, size_t),
	size_t (*size_func) (void*),
	size_t (*pos_func) (void*),
	bool (*seek_func) (void*, uint32_t, int),
	void* (*open_func) (void*, bool),
	void (*close_func) (void*),
	void* (*fork_func) (void*),
	s3util_exception_t** throws);

void s3map_readfile_name(s3map_t* handle, char* name, s3util_exception_t** throws);
void s3map_readfile_fd(s3map_t* handle, uint32_t* file, s3util_exception_t** throws);
void s3map_readfile_ioset(s3map_t* handle, void* io_arg, s3util_ioset_t* ioset, bool use_openclose_func, s3util_exception_t** throws);



void s3map_init_func(s3map_t* handle, void* arg,
	bool (*read_func) (void*, void*, size_t),
	bool (*write_func) (void*, void*, size_t),
	size_t (*size_func) (void*),
	size_t (*pos_func) (void*),
	bool (*seek_func) (void*, uint32_t, int),
	void* (*open_func) (void*, bool),
	void (*close_func) (void*),
	void* (*fork_func) (void*));

void s3map_init_name(s3map_t* handle, char* name); //name must be vaild until s3map_readfile/s3map_writefile has ended
void s3map_init_fd(s3map_t* handle, uint32_t* file); //file must be vaild until s3map_readfile/s3map_writefile has ended
bool s3map_init_ioset(s3map_t* handle, void* io_arg, s3util_ioset_t* ioset, bool use_openclose_func); // io_arg mst be valid until s3map_readfile has ended

void s3map_readfile(s3map_t* handle, s3util_exception_t** throws);


uint32_t s3map_genchecksum(s3map_t* handle, s3util_exception_t** throws);

uint8_t s3map_internal_read8DEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws);
uint16_t s3map_internal_read16LEDEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws);
uint32_t s3map_internal_read32LEDEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws);
void s3map_internal_readDEC(s3map_t* handle, void* to, uint32_t size, uint8_t* key, s3util_exception_t** throws);

void s3map_internal_seek_func(s3map_t* handle, uint32_t pos, int whence, s3util_exception_t** throws);

void s3map_read_buildingdata(s3map_t* handle, s3map_building_t** buildingreturn, s3util_exception_t** throws);
void s3map_read_settlerdata(s3map_t* handle, s3map_settler_t** settlerreturn, s3util_exception_t** throws);
void s3map_read_previewpixels(s3map_t* handle, s3util_color_t** pixelreturn, s3util_exception_t** throws);
void s3map_read_stackdata(s3map_t* handle, s3map_stack_t** stackreturn, s3util_exception_t** throws);
void s3map_read_mapfields(s3map_t* handle, s3map_field_t** mapreturn, s3util_exception_t** throws);

s3map_t* s3map_new_malloc();
s3map_t* s3map_new_malloc_monitor(void* arg, s3util_ioset_t* ioset, bool open);
s3map_t* s3map_new_func(void* arg, void* (*alloc_func) (void*, size_t), void (*free_func) (void*, void*));

void s3map_delete(s3map_t* handle);
#endif /*S3MAP_H*/

