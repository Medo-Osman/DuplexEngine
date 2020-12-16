#pragma once


static const bool USE_SORTED = false;

static const bool USE_QUADTREE = !USE_SORTED;
static const bool USE_Z_PRE_PASS = USE_SORTED;
static const bool USE_FRUSTUM_CULLING = true;
static const bool USE_EXPERIMENTAL_SORTING = true;

struct drawInt {
	unsigned int dist : 32;
	unsigned int id : 24;
};

