//////////////////////////////////////////////////////////////////////////////
//
//    ALBUMSETTINGS.H
//
//    Copyright (C) 2003-2004 Renchi Raju <renchi at pooh.tam.uiuc.edu>
//                            Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ALBUMSETTINGS_H
#define ALBUMSETTINGS_H

// KDE includes.

#include <qstringlist.h>
#include <qstring.h>

class AlbumSettingsPrivate;

class AlbumSettings {

public:

    enum AlbumSortOrder
    {
        ByCollection = 0,
        ByDate,
        Flat
    };

    AlbumSettings();
    ~AlbumSettings();

    void readSettings();
    void saveSettings();

    void setAlbumLibraryPath(const QString& path);
    QString getAlbumLibraryPath() const;

    void setAlbumCollectionNames(const QStringList& list);
    QStringList getAlbumCollectionNames();

    bool addAlbumCollectionName(const QString& name);
    bool delAlbumCollectionName(const QString& name);
    
    void setAlbumSortOrder(const AlbumSortOrder order);
    AlbumSortOrder getAlbumSortOrder();

    void setImageFileFilter(const QString& filter);
    QString getImageFileFilter() const;
    
    void setMovieFileFilter(const QString& filter);
    QString getMovieFileFilter() const;
        
    void setRawFileFilter(const QString& filter);
    QString getRawFileFilter() const;
    
    void setDefaultIconSize(int val);
    int  getDefaultIconSize() const;

    void setIconShowMime(bool val);
    bool getIconShowMime() const;

    void setIconShowSize(bool val);
    bool getIconShowSize() const;

    void setIconShowComments(bool val);
    bool getIconShowComments() const;

    void setIconShowDate(bool val);
    bool getIconShowDate() const;

    static AlbumSettings *instance();

private:

    static AlbumSettings* instance_;
    
    void init();

    AlbumSettingsPrivate* d;

};

#endif  // ALBUMSETTINGS_H
