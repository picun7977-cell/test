#pragma once

#include <common/Common.hpp>

bool Hook_IsLoadoutAllowed();

using IsLoadoutAllowed_t = decltype(&Hook_IsLoadoutAllowed);
inline IsLoadoutAllowed_t IsLoadoutAllowed_o = nullptr;
