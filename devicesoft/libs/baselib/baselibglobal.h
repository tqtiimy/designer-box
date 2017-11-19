#ifndef BASELIBGLOBAL_H
#define BASELIBGLOBAL_H

#include <QtCore/qglobal.h>


#if defined(BASE_LIBRARY)
#  define BASELIB_EXPORT Q_DECL_EXPORT
#else
#  define BASELIB_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBGLOBAL_H
