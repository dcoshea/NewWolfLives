/*************************************************************************
** NewWolfLives
** Copyright (C) David O'Shea 2013.
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** Visual C++ 2005 provides VC/atlmfc/include/afxres.h, which is
** missing in Visual C++ 2005 Express Edition.  This header file,
** included by newwolf.rc, provides the missing definitions, as
** suggested by http://stackoverflow.com/a/3566074 It is used when
** building under the non-Express Edition too.
*************************************************************************/

#ifndef __AFXRES_H
#define __AFXRES_H

#include <windows.h>
#define IDC_STATIC -1

#endif /* __AFXRES_H */
