#pragma once

#include <libintl.h>

#define _(String) std::string(gettext(String))
#define ngettext(A, B, C) ((C) > 1 ? _(B) : _(A))
