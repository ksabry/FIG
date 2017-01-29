#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_SHARED __declspec( dllexport )
#define ENGINE_TEMPLATE
#else
#define ENGINE_SHARED __declspec( dllimport )
#define ENGINE_TEMPLATE extern
#endif
