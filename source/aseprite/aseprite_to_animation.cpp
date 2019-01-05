/*
 * Aseprite animation
 * Version 0.1
 * Copyright 2019 by Frantisek Veverka
 *
 */
#include <string>
#include "animation.h"
#include "aseprite.h"
#include "aseprite_to_animation.h"

animation::Animation animation::Animation::loadAseImage(const std::string &path) {
	return fromASEPRITE(aseprite::ASEPRITE(path));
}

