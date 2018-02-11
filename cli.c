#include "s3map.h"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

s3util_exception_t* ex;

int main(int argc, char** argv) {
	DIR* mapdir = NULL;
	if(argc == 1) {
		mapdir = opendir("MAP");

		if(!mapdir) {
			printf("MAP/ can not be opened\n");
			return 1;
		}
	}

	struct dirent* dirent;

	int i = argc == 1 ? 0 : 1;

	while((argc == 1 && (dirent = readdir(mapdir)) != NULL) || (argc > 1 && i != argc)) {
		if(argc == 1 && (strncmp(".", dirent->d_name, 1) == 0 || strncmp("..", dirent->d_name, 2) == 0)) continue;

		int len = 0;
		if(argc == 1) {
			len = strlen(dirent->d_name);
		}

		char name[len+5];
		if(argc == 1) {
			name[0] = 'M';
			name[1] = 'A';
			name[2] = 'P';
			name[3] = '/';
			strcpy(name+4, dirent->d_name);
		}


		if(i != 0) printf("\n");

		printf("[%i] %s\n", i, argc == 1 ? name : argv[i]);

		s3map_t* handle = s3map_new_malloc();

		s3map_readfile_name(handle, argc == 1 ? name : argv[i], &ex);
		if(s3util_catch_exception(&ex)) {
			printf("[%i] %s map%s with%s (%i startres) checksum: %x\n", i,
			handle->single_player ? "Singleplayer" : "Multiplayer",
			handle->edmguard ? " from editor" : "",
			handle->contains_amazon ? " Amazons" : "out Amazons", handle->startres,
			handle->checksum);

			if(handle->mapdesc) {
				printf("[%i] map description: %s\n", i, handle->mapdesc);
			}
			if(handle->maptips) {
				printf("[%i] map tips: %s\n", i, handle->maptips);
			}

			printf("[%i] %i*%i preview at %i\n", i, handle->preview.arg, handle->preview.arg, handle->preview.pos);
			printf("[%i] %i*%i map at %i\n", i, handle->map.arg, handle->map.arg, handle->map.pos);
			printf("[%i] %i stacks at %i\n", i, handle->stacks.arg, handle->stacks.pos);
			printf("[%i] %i settlers at %i\n", i, handle->settlers.arg, handle->settlers.pos);
			printf("[%i] %i buildings at %i\n", i, handle->buildings.arg, handle->buildings.pos);

			/*printf("[%i] must players be killed to win ? %s\n", i, handle->kill_wincond != 0 ? "Yes" : "No");
			for(uint32_t p = 0;p != 10;p++) {
				if(handle->building_wincond[p].player != 0xFF) {
					printf("[%i] wincond: Player %is %i must be destroyed!\n", i, handle->building_wincond[p].player, handle->building_wincond[p].building);
				}
			}
			for(uint32_t p = 0;p != 5;p++) {
					if(handle->position_wincond[p].enabled) {
						printf("[%i] wincond: (%i:%i) must be conquered!\n", i, handle->position_wincond[p].x, handle->position_wincond[p].y);
					}
			}
			for(uint32_t p = 0;p != 3;p++) {
				if(handle->production_wincond[p].count > 0) {
					printf("[%i] you must produce %i %is to win!\n", i, handle->production_wincond[p].count, handle->production_wincond[p].stacktype);
				}
			}

			for(uint32_t p = 0;p != handle->player_count;p++) {
				printf("[%i] player %i (called \"%s\") is %i at (%i:%i)%s", i, p,
				handle->playerinfos[p].name, handle->playerinfos[p].nation,
				handle->playerinfos[p].startx, handle->playerinfos[p].starty,
				(handle->kill_wincond>>p)&1 ? " and must be killed to win" : "");

				if(handle->time_wincond[p] > 0) {
					printf(" and must survive %humin to win!", handle->time_wincond[p]);
				}
				printf("\n");
			}

			for(uint32_t c = 0;c != handle->constellation_count;c++) {
				printf("[%i] constellation %i: %s\n", i, c, handle->constellation_name[c]);
				for(uint32_t p = 0;p != handle->player_count;p++) {
					printf("[%i] player %i is %i in team %i\n", i, p, handle->playerinfos[p].type[c], handle->playerinfos[p].team[c]);
				}
			}

			s3map_settler_t* settlers;
			s3map_read_settlerdata(handle, &settlers, &ex);
			if(s3util_catch_exception(&ex)) {
				for(uint32_t s = 0;s != handle->settlers.arg;s++) {
					printf("[%i] settler %i is a %i from player %i at (%i:%i)\n", i, s, settlers[s].type, settlers[s].player, settlers[s].x, settlers[s].y);
				}
				s3util_free_func(&handle->memset, settlers);
			}

			s3map_stack_t* stacks;
			s3map_read_stackdata(handle, &stacks, &ex);
			if(s3util_catch_exception(&ex)) {
				for(uint32_t s = 0;s != handle->stacks.arg;s++) {
					printf("[%i] stack %i is %ix%i at (%i:%i)\n", i, s, stacks[s].size, stacks[s].type, stacks[s].x, stacks[s].y);
				}
				s3util_free_func(&handle->memset, stacks);
			}

			s3map_building_t* buildings;
			s3map_read_buildingdata(handle, &buildings, &ex);
			if(s3util_catch_exception(&ex)) {
				for(uint32_t s = 0;s != handle->buildings.arg;s++) {
					printf("[%i] building %i is %i from player %i at (%i:%i); troops=%i\n", i, s, buildings[s].type, buildings[s].player, buildings[s].x, buildings[s].y, buildings[s].sword3);
				}
				s3util_free_func(&handle->memset, buildings);
			}*/

			/*
			s3util_color_t* color;
			s3map_read_previewpixels(handle, &color, &ex);
			if(s3util_catch_exception(&ex)) {
				char preview_name[128];
				snprintf(preview_name, 128, "%x-preview.data", i);

				FILE* f = fopen(preview_name, "wb");
				fwrite(color, 4, handle->preview.arg*handle->preview.arg, f);
				fclose(f);
				s3util_free_func(&handle->memset, color);
			}*/
		}

		s3map_delete(handle);

		i++;
	}

	closedir(mapdir);
	return 0;
}

