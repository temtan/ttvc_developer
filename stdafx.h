// stdafx.h

#pragma once

// C
#include <errno.h>
#include <limits.h>
#include <process.h>
#include <stdio.h>
#include <time.h>

// C++
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <optional>

// Windows
#include "tt_windows_h_include.h"
#include <commctrl.h>
#pragma warning(push)
#  pragma warning( disable: 4005 )
#  include <shellapi.h>
#pragma warning(pop)
#pragma warning(push)
#  pragma warning(disable : 4005)
#  pragma warning(disable : 4917)
#  include <Shlobj.h>
#pragma warning(pop)

// ttl
#include "ttl_define.h"
#include "tt_window.h"
#include "tt_window_controls.h"
#include "tt_utility.h"
#include "tt_exception.h"
#include "tt_file_dialog.h"
#include "tt_ini_file.h"
#include "tt_message_box.h"
#include "tt_string.h"
#include "tt_path.h"
#include "tt_folder_browser_dialog.h"

// Squirrel
#include "squirrel.h"
#include "sqstdio.h"
#include "sqstdstring.h"
