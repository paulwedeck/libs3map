#include "s3map.h"
#include <math.h>
#include <GLFW/glfw3.h>

int width, height;
double scale = 1;

s3map_t* map = NULL;

bool buildings_map_possible = true;
bool settlers_map_possible = true;
bool stacks_map_possible = true;
bool buildings_map = true;
bool politics_map = true;
bool settlers_map = true;
bool terrain_map = true;
bool stacks_map = true;
int height_map = 1;

bool redraw = false;

void onkey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {
		if(key == GLFW_KEY_B && buildings_map_possible) buildings_map = !buildings_map;
		if(key == GLFW_KEY_S && settlers_map_possible) settlers_map = !settlers_map;
		if(key == GLFW_KEY_R && stacks_map_possible) stacks_map = !stacks_map;
		if(key == GLFW_KEY_P) politics_map = !politics_map;
		if(key == GLFW_KEY_T) terrain_map = !terrain_map;
		if(key == GLFW_KEY_H) height_map = (height_map+1) % 3;

		redraw = true;
	}
}

void onresize(GLFWwindow* wnd, int w, int h) {
	width = w;
	height = h;

	double s = w/1.5 > h ? h : w/1.5 ;

	scale = s/map->map.arg;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -10, 1);
	glMatrixMode(GL_MODELVIEW);

	redraw = true;
}

#define COORD(x, y) x*scale+y*scale*0.5, y*scale

void vertexat(double x, double y) {
	glVertex2d(COORD(x,y));
}

void drawTileAt(double r, double g, double b, int x, int y) {
	glColor3d(r, g, b);
	glBegin(GL_QUADS);
	vertexat(x, y);
	vertexat(x, y+1);
	vertexat(x+1, y+1);
	vertexat(x+1, y);
	glEnd();
}

void drawCircleAt(double r, double g, double b, int x, int y, double radius) {
	glColor3d(r, g, b);
	glPushMatrix();
	glTranslatef(COORD(x,y), 0);
	glBegin(GL_POLYGON);
	for(double x = 0; x < 2*M_PI;x += 0.01) {
		glVertex2d(sin(x)*radius*scale, cos(x)*radius*scale);
	}
	glEnd();
	glPopMatrix();
}

int main(int argc, char** argv) {
	s3util_exception_t* ex = NULL;

	s3map_building_t* buildings = NULL;
	s3map_settler_t* settlers = NULL;
	s3map_field_t* fields = NULL;
	s3map_stack_t* stacks = NULL;

	if(argc != 2) {
		printf("usage: %s [mapname]\n", argv[0]);
		printf("controls:\n");
		printf("P: show/hide player area\n");
		printf("B: show/hide buildings\n");
		printf("S: show/hide settlers\n");
		printf("R: show/hide stacks\n");
		printf("T: show/hide terrain\n");
		printf("H: show/hide height map\n");
		return 1;
	}

	glfwInit();

	map = s3map_new_malloc();

	s3map_readfile_name(map, argc == 1 ? "MAP3/test.edm" : argv[1], &ex);
	if(!s3util_catch_exception(&ex)) goto error;

	s3map_read_mapfields(map, &fields, &ex);
	s3util_catch_exception(&ex);

	s3map_read_settlerdata(map, &settlers, &ex);
	if(!s3util_catch_exception(&ex)) {
		settlers_map = settlers_map_possible = false;
	}

	s3map_read_buildingdata(map, &buildings, &ex);
	if(!s3util_catch_exception(&ex)) {
		buildings_map = buildings_map_possible = false;
	}

	s3map_read_stackdata(map, &stacks, &ex);
	if(!s3util_catch_exception(&ex)) {
		stacks_map =stacks_map_possible = false;
	}

	GLFWwindow* wnd = glfwCreateWindow(map->map.arg*3, map->map.arg*2, "Minimap", NULL, NULL);

	glfwSetFramebufferSizeCallback(wnd, onresize);
	glfwSetKeyCallback(wnd, onkey);

	glfwMakeContextCurrent(wnd);

	glfwSwapInterval(0);

	onresize(wnd, map->map.arg*3, map->map.arg*2);

#define L() F(x, y)
#define F(x, y) fields[(map->map.arg-y-1)*map->map.arg+x]

	//glClearColor(1, 1, 1, 1);
	while(!glfwWindowShouldClose(wnd)) {
		if(redraw) {
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();

			for(uint32_t y = 0;y != map->map.arg;y++) {
				for(uint32_t x = 0;x != map->map.arg;x++) {
					double r = 1;
					double g = 1;
					double b = 1;

					bool border_tile = x == 0 || y == 0 || x == map->map.arg-1 || y == map->map.arg-1;

					bool politics_local = true;

					if(terrain_map) {
						if(!border_tile) {
							if(L().player == F(x,y-1).player &&
								L().player == F(x,y+1).player &&
								L().player == F(x+1,y).player &&
								L().player == F(x-1,y).player) {
								politics_local = false;
							}
						}
					}

					if(politics_map && politics_local && L().player != 0xFF) {
						r = g = b = 0;

						switch(L().player) {
							case 0:
								r = 1;
							break;
							case 1:
								b = 1;
								g = 0.4;
							break;
							case 2:
								r = g = 1;
							break;
							case 3:
								g = 1;
							break;
						}
					} else if(terrain_map) {
						r = g = b = 0;
						switch(L().type) {
							case 7:
								b = 1;
							break;
							case 16:
								g = 1;
							break;
							case 0:
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 48:
								r = g = 1;
								b = 0.5;
							break;
							case 32:
							case 33:
							case 17:
								r = g = b = 0.2;
							break;
							default:
								r = b = 1;
							break;
						}
					}

					if(height_map) {
						uint8_t height_info = L().height;

						if(height_map == 2 && !border_tile) {
							if(F(x-1, y).height > L().height) height_info = 0;
								else if(F(x, y-1).height < L().height) height_info = 7;
								else if(F(x-1, y).height < L().height) height_info = 127;
								else if(F(x, y-1).height > L().height) height_info = 167;
							else height_info = 63;
						}

						double offset = height_info == 0 ? 0.5 : 0.6;

						double offsetr = r > 0 ? offset : 0;
						double offsetg = g > 0 ? offset : 0;
						double offsetb = b > 0 ? offset : 0;

						r = (r*height_info/512.0)+offsetr;
						g = (g*height_info/512.0)+offsetg;
						b = (b*height_info/512.0)+offsetb;
					} else {
						r *= 0.5;
						g *= 0.5;
						b *= 0.5;
					}

					drawTileAt(r, g, b, x, y);
				}
			}

			if(settlers_map) {
				for(uint32_t i = 0;i != map->settlers.arg;i++) {
					drawCircleAt(0, 0, 0, settlers[i].x+0.5, map->map.arg-settlers[i].y+0.5, 1);
				}
			}

			if(buildings_map) {
				for(uint32_t i = 0;i != map->buildings.arg;i++) {
					drawCircleAt(0, 0, 0, buildings[i].x+0.5, map->map.arg-buildings[i].y+0.5, 3);
				}
			}

			if(stacks_map) {
				for(uint32_t i = 0;i != map->stacks.arg;i++) {
					drawCircleAt(0, 0, 0, stacks[i].x+0.5, map->map.arg-stacks[i].y+0.5, 0.5);
				}
			}
			redraw = false;
		}

		glfwPollEvents();
		glfwSwapBuffers(wnd);
	}

	error:
	glfwTerminate();

	if(fields) s3util_free_func(&map->memset, fields);
	if(settlers) s3util_free_func(&map->memset, settlers);
	if(buildings) s3util_free_func(&map->memset, buildings);
	if(map) s3map_delete(map);
}
