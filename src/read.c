#include "s3map.h"
#ifdef PRIVATE_FILENAME
#line __LINE__ "read.c"
#endif

void s3map_read_map(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_stacks(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_maptext(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_wincond(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_preview(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_mapinfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_teaminfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_settlers(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_buildings(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);
void s3map_read_playerinfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws);

void s3map_readfile_func(s3map_t* handle, void* arg,
	bool (*read_func) (void*, void*, size_t),
	bool (*write_func) (void*, void*, size_t),
	size_t (*size_func) (void*),
	size_t (*pos_func) (void*),
	bool (*seek_func) (void*, uint32_t, int),
	void* (*open_func) (void*, bool),
	void (*close_func) (void*),
	void* (*fork_func) (void*),
	s3util_exception_t** throws) {

	s3map_init_func(handle, arg, read_func, write_func, size_func, pos_func, seek_func, open_func, close_func, fork_func);
}

void s3map_init_func(s3map_t* handle, void* arg,
	bool (*read_func) (void*, void*, size_t),
	bool (*write_func) (void*, void*, size_t),
	size_t (*size_func) (void*),
	size_t (*pos_func) (void*),
	bool (*seek_func) (void*, uint32_t, int),
	void* (*open_func) (void*, bool),
	void (*close_func) (void*),
	void* (*fork_func) (void*)) {

	handle->ioset.arg = arg;
	handle->ioset.read_func = read_func;
	handle->ioset.write_func = write_func;
	handle->ioset.size_func = size_func;
	handle->ioset.pos_func = pos_func;
	handle->ioset.open_func = open_func;
	handle->ioset.close_func = close_func;
	handle->ioset.fork_func = fork_func;
}


void s3map_readfile_name(s3map_t* handle, char* name, s3util_exception_t** throws) {
	s3map_init_name(handle, name);
	s3map_readfile(handle, throws);
	S3UTIL_HANDLE_EXCEPTION(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_init_name(s3map_t* handle, char* name) {
	s3map_init_ioset(handle, name, s3util_get_default_ioset(S3UTIL_IOSET_DEFAULT), true);
}


void s3map_readfile_fd(s3map_t* handle, uint32_t* file, s3util_exception_t** throws) {
	s3map_init_fd(handle, file);
	S3UTIL_HANDLE_EXCEPTION(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_init_fd(s3map_t* handle, uint32_t* file) {
	s3map_init_ioset(handle, file, s3util_get_default_ioset(S3UTIL_IOSET_DEFAULT), false);
}


void s3map_readfile_ioset(s3map_t* handle, void* io_arg, s3util_ioset_t* ioset, bool use_openclose_func, s3util_exception_t** throws) {
	if(!s3map_init_ioset(handle, io_arg, ioset, use_openclose_func)) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_IOSET, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_readfile(handle, throws);
	S3UTIL_HANDLE_EXCEPTION(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

bool s3map_init_ioset(s3map_t* handle, void* io_arg, s3util_ioset_t* ioset, bool use_openclose_func) {
	if(ioset == NULL || ioset->available == false) return false;

	handle->ioset = *ioset;
	handle->ioset.arg = io_arg;

	if(!use_openclose_func) {
		handle->ioset.open_func = NULL;
		handle->ioset.close_func = NULL;
	}

	return true;
}

void s3map_readfile(s3map_t* handle, s3util_exception_t** throws) {
	if(handle->ioset.open_func) handle->ioset.arg = handle->ioset.open_func(handle->ioset.arg, false);

	if(handle->ioset.arg == NULL) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OPEN, __FILE__, __func__, __LINE__);
		return;
	}

	handle->checksum = S3UTIL_INTERNAL_READ(32LE, &handle->ioset, &handle->memset, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint32_t map_type = S3UTIL_INTERNAL_READ(32LE, &handle->ioset, &handle->memset, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	if(map_type != 0xB && map_type != 0xA) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}
	handle->contains_amazon = map_type == 0xB;

	struct s3map_chunk_t {
		uint32_t type;
		void (*read_chunk) (s3map_t*,uint32_t, uint8_t, s3util_exception_t** throws);
		uint32_t pos;
		uint32_t len;
	};

	#define S3MAP_CHUNK_MAPINFO 1
	#define S3MAP_CHUNK_PLAYERINFO 2
	#define S3MAP_CHUNK_TEAMINFO 3
	#define S3MAP_CHUNK_PREVIEW 4
	#define S3MAP_CHUNK_MAP 6
	#define S3MAP_CHUNK_SETTLERS 7
	#define S3MAP_CHUNK_BUILDINGS 8
	#define S3MAP_CHUNK_STACKS 9
	#define S3MAP_CHUNK_WINCOND 10
	#define S3MAP_CHUNK_MAPDESC 11
	#define S3MAP_CHUNK_MAPTIPS 12

	struct s3map_chunk_t chunks[] = {
		{S3MAP_CHUNK_MAPINFO, s3map_read_mapinfo, 0, 0},
		{S3MAP_CHUNK_PLAYERINFO, s3map_read_playerinfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO, s3map_read_teaminfo, 0, 0,},
		{S3MAP_CHUNK_PREVIEW, s3map_read_preview, 0, 0},
		{S3MAP_CHUNK_MAP, s3map_read_map, 0, 0},
		{S3MAP_CHUNK_SETTLERS, s3map_read_settlers, 0, 0},
		{S3MAP_CHUNK_BUILDINGS, s3map_read_buildings, 0, 0},
		{S3MAP_CHUNK_STACKS, s3map_read_stacks, 0, 0},
		{S3MAP_CHUNK_WINCOND, s3map_read_wincond, 0, 0},
		{S3MAP_CHUNK_MAPDESC, s3map_read_maptext, 0, 0},
		{S3MAP_CHUNK_MAPTIPS, s3map_read_maptext, 0, 0},

		{S3MAP_CHUNK_TEAMINFO | 1<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 2<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 3<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 4<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 5<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 6<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 7<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 8<<16, s3map_read_teaminfo, 0, 0},
		{S3MAP_CHUNK_TEAMINFO | 9<<16, s3map_read_teaminfo, 0, 0},
		{0, NULL, 0, 0} // end mark
	};

	bool eof_mark = false;
	do {
		uint32_t type = S3UTIL_INTERNAL_READ(32LE, &handle->ioset, &handle->memset, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		uint32_t len = S3UTIL_INTERNAL_READ(32LE, &handle->ioset, &handle->memset, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		len -= 8;

		bool chunk_known = false;

		if(type == 0) {
			eof_mark = true;
			chunk_known = true;
		} else if(type > 1000000) { // some chunks are filled with random data, we will ignore them
		} else {
			uint32_t chunk_id = 0;
			while(chunks[chunk_id].type != 0 && !chunk_known) {
				if(chunks[chunk_id].type == type) {
					if(chunks[chunk_id].pos != 0) {
						s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
							return;
					}
					chunks[chunk_id].pos = handle->ioset.pos_func(handle->ioset.arg);
					chunks[chunk_id].len = len;
					chunk_known = true;
				}
				chunk_id++;
			}
		}

		s3map_internal_seek_func(handle, len, S3UTIL_SEEK_CUR, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	} while(!eof_mark);

	uint32_t chunk_id = 0;
	while(chunks[chunk_id].type != 0) {
		if(chunks[chunk_id].pos) {
			s3map_internal_seek_func(handle, chunks[chunk_id].pos, S3UTIL_SEEK_SET, throws);
			S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
		}
		chunks[chunk_id].read_chunk(handle, chunks[chunk_id].len, chunks[chunk_id].type&0xFF, throws); 
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		chunk_id++;
	}
}

void s3map_read_mapinfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len != 24) {
		// 12byte data + 12byte null not more or less
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__ ,__func__, __LINE__);
		return;
	}

	// read
	uint32_t maptype = s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	// and validate
	if(maptype != 1 && maptype != 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__ ,__func__, __LINE__);
		return;
	}
	handle->single_player = maptype;


	// read
	uint32_t players = s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	// and validate
	if(players == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__ ,__func__, __LINE__);
		return;
	} else if(handle->player_count != 0 && handle->player_count != players) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__ ,__func__, __LINE__);
		return;
	}
	handle->player_count = players;


	// read
	uint32_t start_resources = s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	// and validate
	if(start_resources == 1) {
		handle->startres = s3map_startres_low;
	} else if(start_resources == 2) {
		handle->startres = s3map_startres_medium;
	} else if(start_resources == 3) {
		handle->startres = s3map_startres_high;
	} else {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__ ,__func__, __LINE__);
		return;
	}

	// create playerinfos
	handle->playerinfos = s3util_alloc_func(&handle->memset, sizeof(s3map_playerinfo_t)*handle->player_count, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

}

void s3map_read_playerinfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(handle->player_count*45 != len) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
		return;
	}

	char names[handle->player_count*34];

	for(uint32_t i = 0;i != handle->player_count;i++) {
		uint32_t nation = s3map_internal_read32LEDEC(handle, &key, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		if(((handle->contains_amazon && nation > 3) || (!handle->contains_amazon && nation > 2)) && nation < 255) {
			s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__, __func__, __LINE__);
			return;
		}

		handle->playerinfos[i].nation = nation;

		handle->playerinfos[i].startx = s3map_internal_read32LEDEC(handle, &key, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		handle->playerinfos[i].starty = s3map_internal_read32LEDEC(handle, &key, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		s3map_internal_readDEC(handle, &names[34*i], 33, &key, throws);
		names[34*i+33] = '\0';
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	}

	for(uint32_t i = 0;i != handle->player_count;i++) {
		uint32_t namelen = strnlen(&names[34*i], 33);
		handle->playerinfos[i].name = s3util_alloc_func(&handle->memset, namelen+1, throws);
		if(*throws != NULL) {
			s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);

			for(uint32_t ri = 0;ri != i;ri++) {
				s3util_free_func(&handle->memset, handle->playerinfos[ri].name);
			}
			return;
		}

		strncpy(handle->playerinfos[i].name, &names[34*i], namelen);
		handle->playerinfos[i].name[namelen] = '\0';

		s3util_iso8859_to_utf8(&handle->memset, &handle->playerinfos[i].name, namelen+1, false, throws);
		if(*throws != NULL) {
			s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);

			for(uint32_t ri = 0;ri != i+1;ri++) {
				s3util_free_func(&handle->memset, handle->playerinfos[ri].name);
			}
			return;
		}
	}
}

void s3map_read_teaminfo(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	if((handle->player_count*2)+33 != len) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
		return;
	}

	if(handle->constellation_count == 10) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__, __func__, __LINE__);
		return;
	}

	char tmp_const_name[34];
	s3map_internal_readDEC(handle, tmp_const_name, 33, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	tmp_const_name[33] = '\0';

	for(uint32_t i = 0;i != handle->player_count;i++) {
		uint8_t type = s3map_internal_read8DEC(handle, &key, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

		if(type > 3 && type != 0xFF) {
			s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__, __func__, __LINE__);
			return;
		}

		handle->playerinfos[i].type[handle->constellation_count] = type;
		handle->playerinfos[i].team[handle->constellation_count] = s3map_internal_read8DEC(handle, &key, throws);
		S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	}

	handle->constellation_name[handle->constellation_count] = s3util_alloc_func(&handle->memset, 34, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	strncpy(handle->constellation_name[handle->constellation_count], tmp_const_name, 33);

	if(*throws != NULL) {
		s3util_free_func(&handle->memset, handle->constellation_name[handle->constellation_count] );
		s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
		return;
	}

	s3util_iso8859_to_utf8(&handle->memset, &handle->constellation_name[handle->constellation_count], 34, false, throws);
	if(*throws != NULL) {
		s3util_free_func(&handle->memset, handle->constellation_name[handle->constellation_count]);
		s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
		return;
	}

	s3util_short(&handle->memset, &handle->constellation_name[handle->constellation_count]);

	handle->constellation_count++;
}

void s3map_read_ptr(s3map_t* handle, uint32_t len, uint8_t key, s3map_ptr_t* ptr, int32_t data_size, s3util_exception_t** throws) {
	if(len < 4) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
		return;
	}

	uint32_t arg = s3map_internal_read32LEDEC(handle, &key, throws);

	if((data_size < 0 && arg*arg*data_size != -(len-4)) || (data_size >= 0 && arg*data_size != (len-4))) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
		return;
	}

	ptr->arg = arg;
	ptr->pos = handle->ioset.pos_func(handle->ioset.arg)-4;
}

void s3map_read_preview(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	s3map_read_ptr(handle, len, key, &handle->preview, -2, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_read_map(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	s3map_read_ptr(handle, len, key, &handle->map, -6, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_read_settlers(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	s3map_read_ptr(handle, len, key, &handle->settlers, 6, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_read_buildings(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	s3map_read_ptr(handle, len, key, &handle->buildings, 12, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_read_stacks(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	s3map_read_ptr(handle, len, key, &handle->stacks, 8, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
}

void s3map_read_wincond(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	if(len == 0) return;
	if(len != 119) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_CONFLICTING_DATA, __FILE__, __func__, __LINE__);
		return;
	}

	uint8_t bfr[119];
	s3map_internal_readDEC(handle, bfr, 119, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	// bfr[0] = is it enabled
	// bfr[1-20] = must player be killed
	// 21 bytes

	for(uint32_t i = 0;i != 20;i++) {
		if(bfr[i+1] > 1) {
			s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__, __func__, __LINE__);
			return;
		}

		handle->kill_wincond |= (bfr[i+1]&bfr[0]) << i;
	}

	//bfr[21 - 41]
	// 21 bytes

	for(uint32_t i = 0;i != 10;i++) {
		if(bfr[21]&1) {
			if(bfr[22+i*2] >= handle->player_count && bfr[22+i*2] != 0xFF) {
				s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_OUT_OF_RANGE, __FILE__, __func__, __LINE__);
				return;
			}
			handle->building_wincond[i].player = bfr[22+i*2];
			handle->building_wincond[i].building = bfr[23+i*2];
		} else {
			handle->building_wincond[i].player = 0xFF;
		}
	}

	// bfr[42-67]
	// 26 bytes

	for(uint32_t i = 0;i != 5;i++) {
		handle->position_wincond[i].enabled = bfr[43+i*5]&bfr[42];
		handle->position_wincond[i].x = bfr[44+i*5]|(bfr[45+i*5]<<8);
		handle->position_wincond[i].y = bfr[46+i*5]|(bfr[47+i*5]<<8);
	}

	// bfr[68-108]
	// 41 bytes

	if(bfr[68]) {
		for(uint32_t i = 0;i != 20;i++) {
			handle->time_wincond[i] = bfr[69+i*2]|(bfr[70+i*2]<<8);
		}
	}

	// bfr[109-118]
	// 10 bytes
	if(bfr[109]) {
		for(uint32_t i = 0;i != 3;i++) {
			handle->production_wincond[i].count = bfr[110+i*3]|(bfr[111+i*3]<<8);
			handle->production_wincond[i].stacktype = bfr[112+i*3];
		}
	}
}

void s3map_read_maptext(s3map_t* handle, uint32_t len, uint8_t key, s3util_exception_t** throws) {
	char** dest = key == S3MAP_CHUNK_MAPDESC ? &handle->mapdesc : &handle->maptips;

	char bfr[len+1];
	s3map_internal_readDEC(handle, bfr, len, &key, throws);
	bfr[len] = '\0';

	if(strnlen(bfr, len) == 0) return;

	*dest = s3util_alloc_func(&handle->memset, len, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
	strncpy(*dest, bfr, len);

	s3util_iso8859_to_utf8(&handle->memset, dest, len, false, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	s3util_short(&handle->memset, dest);
}


uint32_t s3map_genchecksum(s3map_t* handle, s3util_exception_t** throws) {
	s3map_internal_seek_func(handle, 4, S3UTIL_SEEK_SET, throws);
	if(*throws != NULL) {
		s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
		return 0;
	}

	uint32_t checksum = 0;

	uint32_t size = handle->ioset.size_func(handle->ioset.arg)/4-1;

	for(uint32_t i = 0;i != size;i++) {
		uint32_t tmp_int = S3UTIL_INTERNAL_READ(32LE, &handle->ioset, &handle->memset, throws);
		if(*throws != NULL) {
			s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
			return 0;
		}

		checksum = (checksum >> 31) | ((checksum << 1) ^ tmp_int);
	}

	return checksum;
}

void s3map_read_previewpixels(s3map_t* handle, s3util_color_t** pixelreturn, s3util_exception_t** throws) {
	if(handle->preview.pos == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_internal_seek_func(handle, handle->preview.pos, S3UTIL_SEEK_SET, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t key = 4;
	s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	s3util_color_t* pixeldata = s3util_alloc_func(&handle->memset, sizeof(s3util_color_t)*handle->preview.arg*handle->preview.arg, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint32_t pixels = handle->preview.arg*handle->preview.arg;
	for(uint32_t i = 0;i != pixels;i++) {
		uint16_t rgb5 = s3map_internal_read16LEDEC(handle, &key, throws);
		if(*throws != NULL) {
			s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
			s3util_free_func(&handle->memset, pixeldata);
			return;
		}

		pixeldata[i] = s3util_native_to_8b(&rgb5, s3util_rgb555);
	}

	*pixelreturn = pixeldata;
}

void s3map_read_settlerdata(s3map_t* handle, s3map_settler_t** settlerreturn, s3util_exception_t** throws) {
	if(handle->settlers.pos == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_internal_seek_func(handle, handle->settlers.pos, S3UTIL_SEEK_SET, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t key = 7;
	s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t rawdata[handle->settlers.arg*6];
	s3map_internal_readDEC(handle, rawdata, handle->settlers.arg*6, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	*settlerreturn = s3util_alloc_func(&handle->memset, sizeof(s3map_settler_t)*handle->settlers.arg, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	for(uint32_t i = 0;i != handle->settlers.arg;i++) {
		(*settlerreturn)[i].player = rawdata[i*6];
		(*settlerreturn)[i].type = rawdata[i*6+1];
		(*settlerreturn)[i].x = rawdata[i*6+2]|(rawdata[i*6+3]<<8);
		(*settlerreturn)[i].y = rawdata[i*6+4]|(rawdata[i*6+5]<<8);
	}
}

void s3map_read_buildingdata(s3map_t* handle, s3map_building_t** buildingreturn, s3util_exception_t** throws) {
	if(handle->buildings.pos == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_internal_seek_func(handle, handle->buildings.pos, S3UTIL_SEEK_SET, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t key = 8;
	s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t rawdata[handle->buildings.arg*12];
	s3map_internal_readDEC(handle, rawdata, handle->buildings.arg*12, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	*buildingreturn = s3util_alloc_func(&handle->memset, sizeof(s3map_building_t)*handle->buildings.arg, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	s3map_building_t* buildings = *buildingreturn;

	for(uint32_t i = 0;i != handle->buildings.arg;i++) {
		buildings[i].player = rawdata[i*12];
		buildings[i].type = rawdata[i*12+1];
		buildings[i].x = rawdata[i*12+2]|(rawdata[i*12+3]<<8);
		buildings[i].y = rawdata[i*12+4]|(rawdata[i*12+5]<<8);

		buildings[i].sword1 = (rawdata[i*12+7]>>4)&0xF;
		buildings[i].sword2 = rawdata[i*12+7]&0xF;
		buildings[i].sword3 = (rawdata[i*12+9]>>4)&0xF;

		buildings[i].archer1 = rawdata[i*12+9]&0xF;
		buildings[i].archer2 = (rawdata[i*12+8]>>4)&0xF;
		buildings[i].archer2 = rawdata[i*12+8]&0xF;

		buildings[i].spear1 = (rawdata[i*12+11]>>4)&0xF;
		buildings[i].spear2 = rawdata[i*12+11]&0xF;
		buildings[i].spear3 = (rawdata[i*12+10]>>4)&0xF;

		if(buildings[i].sword1 == 0 && buildings[i].sword2 == 0 && buildings[i].sword3 == 0 && 
			buildings[i].spear1 == 0 && buildings[i].spear2 == 0 && buildings[i].spear3 == 0 &&
			buildings[i].archer1 == 0 && buildings[i].archer2 == 0 && buildings[i].archer3 == 0) {
			buildings[i].sword1 = 1;
		}
	}
}

void s3map_read_stackdata(s3map_t* handle, s3map_stack_t** stackreturn, s3util_exception_t** throws) {
	if(handle->stacks.pos == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_internal_seek_func(handle, handle->stacks.pos, S3UTIL_SEEK_SET, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t key = 9;
	s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t rawdata[handle->stacks.arg*8];
	s3map_internal_readDEC(handle, rawdata, handle->stacks.arg*8, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	*stackreturn = s3util_alloc_func(&handle->memset, sizeof(s3map_stack_t)*handle->stacks.arg, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	for(uint32_t i = 0;i != handle->stacks.arg;i++) {
		(*stackreturn)[i].x = rawdata[i*8]|(rawdata[i*8+1]<<8);
		(*stackreturn)[i].y = rawdata[i*8+2]|(rawdata[i*8+3]<<8);
		(*stackreturn)[i].type = rawdata[i*8+4];
		(*stackreturn)[i].size = rawdata[i*8+5];
	}
}

void s3map_read_mapfields(s3map_t* handle, s3map_field_t** mapreturn, s3util_exception_t** throws) {
	if(handle->map.pos == 0) {
		s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_HEADER, __FILE__, __func__, __LINE__);
		return;
	}

	s3map_internal_seek_func(handle, handle->map.pos, S3UTIL_SEEK_SET, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t key = 6;
	s3map_internal_read32LEDEC(handle, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint8_t rawdata[handle->map.arg*handle->map.arg*6];
	s3map_internal_readDEC(handle, rawdata, handle->map.arg*handle->map.arg*6, &key, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	*mapreturn = s3util_alloc_func(&handle->memset, sizeof(s3map_field_t)*handle->map.arg*handle->map.arg, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);

	uint32_t fields = handle->map.arg*handle->map.arg;
	for(uint32_t i = 0;i != fields;i++) {
		(*mapreturn)[i].height = rawdata[i*6];
		(*mapreturn)[i].type = rawdata[i*6+1];
		(*mapreturn)[i].mapobject = rawdata[i*6+2];
		(*mapreturn)[i].player = rawdata[i*6+3];
		//we dont read the accessible byte
		(*mapreturn)[i].resourceinfo = rawdata[i*6+5];
	}
}

uint8_t s3map_internal_read8DEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws) {
	uint8_t value;
	s3map_internal_readDEC(handle, &value, 1, key, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
	return value;
}

uint16_t s3map_internal_read16LEDEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws) {
	uint16_t value;
	s3map_internal_readDEC(handle, &value, 2, key, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
	return s3util_le16(value);
}

uint32_t s3map_internal_read32LEDEC(s3map_t* handle, uint8_t* key, s3util_exception_t** throws) {
	uint32_t value;
	s3map_internal_readDEC(handle, &value, 4, key, throws);
	s3util_add_to_stack(&handle->memset, throws, __FILE__, __func__, __LINE__);
	return s3util_le32(value);
}

void s3map_internal_readDEC(s3map_t* handle, void* to, uint32_t size, uint8_t* key, s3util_exception_t** throws) {
	if(!handle->ioset.read_func(handle->ioset.arg, to, size)) s3util_throw(&handle->memset, throws, S3UTIL_EXCEPTION_IOERROR, __FILE__, __func__, __LINE__);

	uint8_t* byte_ptr = to;

	for(uint32_t i = 0;i != size;i++) {
		byte_ptr[i] ^= *key;
		*key = (*key << 1) ^ byte_ptr[i];
	}
}

void s3map_internal_seek_func(s3map_t* handle, uint32_t pos, int whence, s3util_exception_t** throws) {
	s3util_internal_seek_func(&handle->ioset, &handle->memset, pos, whence, throws);
	S3MAP_HANDLE_EXCEPTION(handle, throws, __FILE__, __func__, __LINE__);
}


