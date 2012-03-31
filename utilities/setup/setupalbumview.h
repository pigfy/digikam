/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-02-01
 * Description : album view configuration setup tab
 *
 * Copyright (C) 2003-2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SETUPALBUMVIEW_H
#define SETUPALBUMVIEW_H

// Qt includes

#include <QScrollArea>

class KPageDialog;

namespace Digikam
{

class SetupAlbumView : public QScrollArea
{
    Q_OBJECT

public:

    SetupAlbumView(QWidget* parent = 0);
    ~SetupAlbumView();

    void applySettings();

private:

    void readSettings();

private:

    class SetupAlbumViewPriv;
    SetupAlbumViewPriv* const d;
};

}  // namespace Digikam

#endif // SETUPALBUMVIEW_H
