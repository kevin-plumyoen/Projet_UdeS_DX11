#include "Components.h"

void Components::cleanup() {
    transforms.clear();
    rigidbodies.clear();
    static_meshes.clear();
    cameras.clear();
    lights.clear();
    inputs.clear();
    updates.clear();
    triggers.clear();
    billboards.clear();
	grapplings.clear();
	variable_colors.clear();
    splines.clear();
}