#pragma once




static const bool USE_QUADTREE = false;
static const bool USE_Z_PRE_PASS = true;
static const bool USE_FRUSTUM_CULLING = true;
static const bool USE_EXPERIMENTAL_SORTING = false;

struct drawInt {
	unsigned int dist : 32;
	unsigned int id : 24;
};

