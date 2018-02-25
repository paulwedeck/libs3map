#ifndef S3MAP_TYPES_H
#define S3MAP_TYPES_H

typedef enum {
	s3map_terrain_water1 = 0,
	s3map_terrain_water2 = 1,
	s3map_terrain_water3 = 2,
	s3map_terrain_water4 = 3,
	s3map_terrain_water5 = 4,
	s3map_terrain_water6 = 5,
	s3map_terrain_water7 = 6,
	s3map_terrain_water8 = 7,

	s3map_terrain_grass = 16,
	s3map_terrain_strand = 48,

	s3map_terrain_river1 = 96,
	s3map_terrain_river2 = 97,
	s3map_terrain_river3 = 98,
	s3map_terrain_river4 = 99,

	/*
	s3map_terrain_<whatever>3 selectable in map editor
	s3map_terrain_<whatever>2 between two
	s3map_terrain_<whatever>1 next to grass
	*/

	s3map_terrain_rock3 = 32,
	s3map_terrain_rock2 = 33,
	s3map_terrain_rock1 = 17,

	s3map_terrain_snow3 = 128,
	s3map_terrain_snow2 = 129,
	s3map_terrain_snow1 = 35,

	s3map_terrain_sand3 = 64,
	s3map_terrain_sand2 = 65,
	s3map_terrain_sand1 = 20,

	s3map_terrain_swamp3 = 80,
	s3map_terrain_swamp2 = 81,
	s3map_terrain_swamp1 = 21,

	s3map_terrain_mud3 = 144,
	s3map_terrain_mud2 = 23,
	s3map_terrain_mud1 = 145,
} s3map_terrain;

typedef enum {
	s3map_resource_fish = 8,
	s3map_resource_coal = 24,
	s3map_resource_iron = 40,
	s3map_resource_gold = 56,
	s3map_resource_gem = 72,
	s3map_resource_sulfur = 88,
} s3map_resource;

typedef enum {
	s3map_object_bigstone1 = 1,
	s3map_object_bigstone2 = 2,
	s3map_object_bigstone3 = 3,
	s3map_object_bigstone4 = 4,
	s3map_object_bigstone5 = 5,
	s3map_object_bigstone6 = 6,
	s3map_object_bigstone7 = 7,
	s3map_object_bigstone8 = 8,

	s3map_object_stone1 = 9,
	s3map_object_stone2 = 10,
	s3map_object_stone3 = 11,
	s3map_object_stone4 = 12,

	s3map_object_borderstone1 = 13,
	s3map_object_borderstone2 = 14,
	s3map_object_borderstone3 = 15,
	s3map_object_borderstone4 = 16,
	s3map_object_borderstone5 = 17,
	s3map_object_borderstone6 = 18,
	s3map_object_borderstone7 = 19,
	s3map_object_borderstone8 = 20,

	s3map_object_littlestone1 = 21,
	s3map_object_littlestone2 = 22,
	s3map_object_littlestone3 = 23,
	s3map_object_littlestone4 = 24,
	s3map_object_littlestone5 = 25,
	s3map_object_littlestone6 = 26,
	s3map_object_littlestone7 = 27,
	s3map_object_littlestone8 = 28,

	s3map_object_wreck1 = 29,
	s3map_object_wreck2 = 30,
	s3map_object_wreck3 = 31,
	s3map_object_wreck4 = 32,
	s3map_object_wreck5 = 33,

	s3map_object_grave = 34,
	s3map_object_littleplant1 = 35,
	s3map_object_littleplant2 = 36,
	s3map_object_littleplant3 = 37,

	s3map_object_mushroom1 = 38,
	s3map_object_mushroom2 = 39,
	s3map_object_mushroom3 = 40,

	s3map_object_stump1 = 41,
	s3map_object_stump2 = 42,

	s3map_object_deadtree1 = 43,
	s3map_object_deadtree2 = 44,

	s3map_object_cactus1 = 45,
	s3map_object_cactus2 = 46,
	s3map_object_cactus3 = 47,
	s3map_object_cactus4 = 48,

	s3map_object_bones = 49,

	s3map_object_littleflower1 = 50,
	s3map_object_littleflower2 = 51,
	s3map_object_littleflower3 = 52,

	s3map_object_littlebush1 = 53,
	s3map_object_littlebush2 = 54,
	s3map_object_littlebush3 = 55,
	s3map_object_littlebush4 = 56,

	s3map_object_bush1 = 57,
	s3map_object_bush2 = 58,
	s3map_object_bush3 = 59,
	s3map_object_bush4 = 60,
	s3map_object_bush5 = 61,

	s3map_object_reed1 = 62,
	s3map_object_reed2 = 63,
	s3map_object_reed3 = 64,
	s3map_object_reed4 = 65,
	s3map_object_reed5 = 66,
	s3map_object_reed6 = 67,

	s3map_object_birch1 = 68,
	s3map_object_birch2 = 69,
	s3map_object_elm1 = 70,
	s3map_object_elm2 = 71,
	s3map_object_oak = 72,
	s3map_object_palm1 = 78,
	s3map_object_palm2 = 79,
	s3map_object_littletree = 84,

	s3map_object_riff1 = 111,
	s3map_object_riff2 = 112,
	s3map_object_riff3 = 113,
	s3map_object_riff4 = 114,

	s3map_object_stone_res1 = 115,
	s3map_object_stone_res2 = 116,
	s3map_object_stone_res3 = 117,
	s3map_object_stone_res4 = 118,
	s3map_object_stone_res5 = 119,
	s3map_object_stone_res6 = 120,
	s3map_object_stone_res7 = 121,
	s3map_object_stone_res8 = 122,
	s3map_object_stone_res9 = 123,
	s3map_object_stone_res10 = 124,
	s3map_object_stone_res11 = 125,
	s3map_object_stone_res12 = 126,
	s3map_object_stone_res13 = 127
} s3map_object;

typedef enum {
	s3map_stack_coal = 5,
	s3map_stack_goldore = 6,
	s3map_stack_ironore = 7,
	s3map_stack_sulfur = 25,
	s3map_stack_gem = 28,

	s3map_stack_gold = 10,
	s3map_stack_iron = 11,

	s3map_stack_shovel = 12,
	s3map_stack_hammer = 13,
	s3map_stack_axe = 14,
	s3map_stack_pickaxe = 15,
	s3map_stack_saw = 16,
	s3map_stack_fishingrod = 17,
	s3map_stack_scythe = 32,

	s3map_stack_sword = 18,
	s3map_stack_bow = 19,
	s3map_stack_spear = 20,

	s3map_stack_plank = 1,
	s3map_stack_stone = 2,
	s3map_stack_log = 3,
	s3map_stack_corn = 4,

	s3map_stack_bread = 4,
	s3map_stack_fish = 8,
	s3map_stack_wine = 21,
	s3map_stack_flour = 22,
	s3map_stack_pig = 23,
	s3map_stack_meat = 24,
	s3map_stack_water = 26,
	s3map_stack_rice = 27,
	s3map_stack_beer = 29,
	s3map_stack_gunpowder = 30,
	s3map_stack_scnapps = 33,
	s3map_stack_mead = 34,
	s3map_stack_honey = 35
} s3map_stack;

typedef enum {
	s3map_building_watchtower = 6,
	s3map_building_littletower = 15,
	s3map_building_bigtower = 16,
	s3map_building_castle = 18,
	s3map_building_barracks = 19,
	s3map_building_hospital = 35,


	s3map_building_catapulthall = 31,
	s3map_building_ballistiahall = 47,
	s3map_building_cannonhall = 48,
	s3map_building_gonghall = 50,
	

	s3map_building_distillery = 22,
	s3map_building_ricefarm = 36,
	s3map_building_winery = 14,
	s3map_building_brewery = 38,
	s3map_building_beekeeper = 51,
	s3map_building_vinter = 52,

	s3map_building_pyramid = 41,
	s3map_building_sphinx = 42,
	s3map_building_roman_largetemple = 43,
	s3map_building_roman_smalltemple = 44,
	s3map_building_asian_largetemple = 45,
	s3map_building_asian_smalltemple = 46,
	s3map_building_amazon_largetemple = 55,
	s3map_building_amazon_smalltemple = 54,

	// special buildings
	s3map_building_sulfurmine_asian = 29,
	s3map_building_sulfurmine_amazon = 56,

	s3map_building_gunpowder = 40,
	s3map_building_laboratory = 53,

	s3map_building_charcoal_burner = 39,


	s3map_building_coalmine = 7,
	s3map_building_goldmine = 8,
	s3map_building_ironmine = 9,
	// Egyptian and Amazon
	s3map_building_gemmine = 37,
	s3map_building_goldsmelter = 10,
	s3map_building_ironsmelter = 11,
	s3map_building_toolsmith = 12,
	s3map_building_weaponsmith = 13,

	s3map_building_smallresidence = 26,
	s3map_building_mediumresidence = 27,
	s3map_building_largeresidence = 28,

	s3map_building_storage = 1,
	s3map_building_shipyard = 32,
	s3map_building_port = 33,
	s3map_building_marketplace = 34,

	s3map_building_woodcutter = 2,
	s3map_building_stonecutter = 3,
	s3map_building_sawmill = 4,
	s3map_building_forester = 5,

	s3map_building_mill = 17,
	s3map_building_bakery = 20,
	s3map_building_butcher = 21,
	s3map_building_pig_farm = 23,
	s3map_building_farm = 24,
	s3map_building_fisherman = 25,
	s3map_building_waterworks = 30,
	s3map_building_donkeyfarm = 49,
} s3map_building;

typedef enum {
	s3map_settler_ferry = 44,
	s3map_settler_cargovessel = 43,
	s3map_settler_siegemachine = 41,

	s3map_settler_carrier = 0,
	s3map_settler_grader = 1,
	s3map_settler_builder = 2,

	s3map_settler_woodcutter = 3,
	s3map_settler_stonecutter = 4,
	s3map_settler_sawmill = 6,
	s3map_settler_forester = 7,
	s3map_settler_smelter = 9,
	s3map_settler_miner = 10,
	s3map_settler_smith = 11,
	s3map_settler_miller = 12,
	s3map_settler_baker = 13,
	s3map_settler_butcher = 14,
	s3map_settler_farmer = 16,
	s3map_settler_fisherman = 17,
	s3map_settler_waterworker = 18,
	s3map_settler_deckhand = 19,
	s3map_settler_healer = 21,
	s3map_settler_pig_farmer = 28,

	s3map_settler_brewer = 26, // Egyptian
	s3map_settler_charburner = 24, // Roman
	s3map_settler_vintner_roman = 29, // Roman
	s3map_settler_distiller = 25, // Asian
	s3map_settler_rice_farmer = 38, // Asian
	s3map_settler_beekeeper = 45, // Amazon
	s3map_settler_vintner_amazon = 46, // Amazon
	s3map_settler_alchemist = 47, // Amazon


	s3map_settler_geologist = 22,
	s3map_settler_thief = 23,
	s3map_settler_priest = 36,
	s3map_settler_donkey = 39,
	s3map_settler_pioneer = 40,

	s3map_settler_archer1 = 8,
	s3map_settler_archer2 = 30,
	s3map_settler_archer3 = 33,

	s3map_settler_spear1 = 15,
	s3map_settler_spear2 = 31,
	s3map_settler_spear3 = 34,

	s3map_settler_sword1 = 5,
	s3map_settler_sword2 = 32,
	s3map_settler_sword3 = 35,
} s3map_settler;

#endif /*S3MAP_TYPES_H*/

