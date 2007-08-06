/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-06-05
 * Description : Thumbnail loading
 *
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
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

#ifndef THUMBNAIL_LOAD_THREAD_H
#define THUMBNAIL_LOAD_THREAD_H

// Qt includes

#include <QPixmap>

// Local includes.

#include "managedloadsavethread.h"

namespace Digikam
{

class ThumbnailCreator;
class ThumbnailLoadThreadPriv;

class DIGIKAM_EXPORT ThumbnailLoadThread : public ManagedLoadSaveThread
{
    Q_OBJECT

public:

    ThumbnailLoadThread();
    ~ThumbnailLoadThread();

    /**
     * Returns the application-wide default thumbnail thread.
     * It is perfectly all right to create an extra object of the class,
     * but for some purposes it will be useful to have one default thread.
     */
    static ThumbnailLoadThread *defaultThread();

    /**
     * Find a thumbnail.
     * If the pixmap is found in the cache, it is returned.
     * If the pixmap is not found in the cache, load() is called and a null QPixmap is returned.
     */
    QPixmap find(const QString &filePath);

    /**
     * Load a thumbnail.
     * The LoadingDescription shall be constructed with the constructor for preview/thumbnail jobs.
     * (in the description constructor, you need to specify file path, thumbnail size and exif rotation)
     */
    void load(const LoadingDescription &description);


    /// If the thread is currently loading thumbnails, there is no guarantee as to when
    /// the property change by one of the following methods takes effect.

    /**
     * Set the requested thumbnail size.
     * Default value: 128
     */
    void setThumbnailSize(int size);
    /**
     * Specify if the thumbnails shall be rotated by exif.
     * Note: This only applies to newly created thumbnails. The rotation state of thumbnails
     * found in the disk cache is unknown, so they are not rotated.
     * (The only, unsatisfactory solution is the forced recreation of all thumbnails)
     * Default value: true
     */
    void setExifRotate(int exifRotate);
    /**
     * If you enable this, the signal thumbnailLoaded(LoadingDescription, QPixmap) will be emitted.
     * If you do not enable this, only the QImage-based signal (see LoadSaveThread) will be emitted.
     *
     * If you disable this, pay attention to the (global) setting of the LoadingCache, which per default
     * does not cache the images !!
     *
     * Default value: Enabled.
     */
    void setPixmapRequested(bool wantPixmap);
    /**
     * If you enable this, a highlighting border will be drawn around the pixmap.
     * This option has only an effect if pixmapRequested is true.
     * Default value: Enabled.
     */
    void setHighlightPixmap(bool highlight);
    /**
     * If you enable this, the thread will try hard to send a pixmap if thumbnail loading failed.
     * It will use standard system icons to replace the real thumbnail.
     * If you disable this, a null QPixmap will be sent.
     * This does not influence the QImage-based signal; this signal will be emitted with a null
     * QImage regardless of this setting here, if the loading failed.
     * Default value: Enabled.
     */
    void setSendSurrogatePixmap(bool send);


    /**
     * This is a tool to force regeneration of thumbnails.
     * All thumbnail files for the given file will be removed from disk,
     * and the cached instances will be removed as well.
     * Use this method if you know that the contents of the file has changed.
     * This method works independently from the multithreaded thumbnail loading.
     */
    static void deleteThumbnail(const QString &filePath);

signals:

    // See LoadSaveThread for a QImage-based thumbnailLoaded() signal.

    void thumbnailLoaded(const LoadingDescription &loadingDescription, const QPixmap& pix);

public:

    // For internal use - may only be used from the thread
    ThumbnailCreator *thumbnailCreator() const;

private:

    void sendSurrogatePixmap(const LoadingDescription &loadingDescription);

private slots:

    void slotThumbnailLoaded(const LoadingDescription &loadingDescription, const QImage& thumb);

private:

    ThumbnailLoadThreadPriv *d;
};

}   // namespace Digikam

#endif // SHARED_LOAD_SAVE_THREAD_H
