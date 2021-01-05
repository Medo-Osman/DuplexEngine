#pragma once


static const bool USE_SORTED = false;

static const bool USE_QUADTREE = !USE_SORTED;
static const bool USE_Z_PRE_PASS = USE_SORTED;
static const bool USE_FRUSTUM_CULLING = true;
static const bool USE_EXPERIMENTAL_SORTING = !USE_SORTED;

struct drawInt {
	unsigned int translucency : 2; //0-1 :)
	unsigned int dist : 9; //
	unsigned int id : 5; //
};
