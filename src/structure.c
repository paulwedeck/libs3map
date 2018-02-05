#include "s3map.h"
#ifdef PRIVATE_FILENAME
#line __LINE__ "structure.c"
#endif


s3map_t* s3map_new_malloc() {
	return s3map_new_func(NULL, s3util_default_alloc_func, s3util_default_free_func);
}

s3map_t* s3map_new_malloc_monitor(void* arg, s3util_ioset_t* ioset, bool open) {
	void* ioarg = open ? ioset->open_func(arg, arg) : arg;
	if(!ioarg) return NULL;

	s3util_monitor_t* monitor = s3util_default_alloc_func(0, sizeof(s3util_monitor_t));
	monitor->io_arg = ioarg;
	monitor->ioset = ioset;
	monitor->close = open;

	monitor->last_state = 0;
	monitor->mem_arg = 0;
	monitor->alloc_func = s3util_default_alloc_func;
	monitor->free_func = s3util_default_free_func;

	s3util_monitor_print(monitor);

	return s3map_new_func(monitor, s3util_monitor_alloc_func, s3util_monitor_free_func);
}

s3map_t* s3map_new_func(void* arg, void* (*alloc_func) (void*, size_t), void (*free_func) (void*, void*)) {
	s3map_t* handle = alloc_func(arg, sizeof(s3map_t));
	handle->memset.arg = arg;
	handle->memset.free_func = free_func;
	handle->memset.alloc_func = alloc_func;

	return handle;
}

void s3map_delete(s3map_t* handle) {
	if(handle->ioset.close_func) handle->ioset.close_func(handle->ioset.arg);

	if(handle->playerinfos) {
		for(uint32_t i = 0;i != handle->player_count;i++) {
			if(handle->playerinfos[i].name) s3util_free_func(&handle->memset, handle->playerinfos[i].name);
		}

		s3util_free_func(&handle->memset, handle->playerinfos);
	}

	for(uint32_t i = 0;i != 10;i++) {
		if(handle->constellation_name[i]) {
			s3util_free_func(&handle->memset, handle->constellation_name[i]);
		}
	}

	if(handle->mapdesc) {
		s3util_free_func(&handle->memset, handle->mapdesc);
	}

	if(handle->maptips) {
		s3util_free_func(&handle->memset, handle->maptips);
	}

	s3util_free_func(&handle->memset, handle);
}
