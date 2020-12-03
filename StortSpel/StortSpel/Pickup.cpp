#include"3DPCH.h"
#include"Pickup.h"

bool Pickup::PICKUP_VECTOR_INIT = false;
std::vector<Pickup*>  Pickup::PICKUP_VECTOR;
int Pickup::ICON_ID = -1;
int Pickup::F_KEY_ID = -1;