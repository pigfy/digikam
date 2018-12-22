/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-06-13
 * Description : A widget stack to embedded album content view
 *               or the current image preview.
 *
 * Copyright (C) 2006-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_STACKED_VIEW_H
#define DIGIKAM_STACKED_VIEW_H

// Qt includes

#include <QMainWindow>
#include <QStackedWidget>

// Local includes

#include "digikam_config.h"
#include "digikam_export.h"
#include "iteminfo.h"
#include "thumbbardock.h"

namespace Digikam
{

class DigikamItemView;
class ItemCategorizedView;
class ItemPreviewView;
class ItemThumbnailBar;
class TableView;
class TrashView;

#ifdef HAVE_MEDIAPLAYER
class MediaPlayerView;
#endif //HAVE_MEDIAPLAYER

#ifdef HAVE_MARBLE
class MapWidgetView;
#endif // HAVE_MARBLE

class StackedView : public QStackedWidget
{
    Q_OBJECT

public:

    enum StackedViewMode
    {
        StackedViewModeFirst = 0,
        IconViewMode         = 0,
        PreviewImageMode     = 1,
        WelcomePageMode      = 2,
        TableViewMode        = 3,
        TrashViewMode        = 4,
        MapWidgetMode        = 5,
        MediaPlayerMode      = 6,
        StackedViewModeLast  = 6
    };

public:

    explicit StackedView(QWidget* const parent=0);
    ~StackedView();

    /* Attach the thumbnail dock widget to the specified QMainWindow. */
    void setDockArea(QMainWindow*);

    ThumbBarDock*      thumbBarDock()     const;
    ItemThumbnailBar* thumbBar()         const;
    DigikamItemView*  imageIconView()    const;
    ItemPreviewView*  imagePreviewView() const;
    TableView*         tableView()        const;
    TrashView*         trashView()        const;

#ifdef HAVE_MEDIAPLAYER
    MediaPlayerView*   mediaPlayerView()  const;
#endif //HAVE_MEDIAPLAYER

#ifdef HAVE_MARBLE
    MapWidgetView*     mapWidgetView()    const;
#endif // HAVE_MARBLE

    /**
     * Single-file mode is image preview or media player,
     * multi-file is icon view or map,
     * abstract modes do not handle files (welcome page)
     */
    bool isInSingleFileMode()   const;
    bool isInMultipleFileMode() const;
    bool isInAbstractMode()     const;

    void setPreviewItem(const ItemInfo& info = ItemInfo(),
                        const ItemInfo& previous = ItemInfo(),
                        const ItemInfo& next = ItemInfo());
    StackedViewMode viewMode() const;
    void setViewMode(const StackedViewMode mode);
    void previewLoaded();

    void   increaseZoom();
    void   decreaseZoom();
    void   fitToWindow();
    void   toggleFitToWindowOr100();
    void   zoomTo100Percents();
    bool   maxZoom();
    bool   minZoom();
    void   setZoomFactor(double z);
    void   setZoomFactorSnapped(double z);
    double zoomFactor();
    double zoomMin();
    double zoomMax();

Q_SIGNALS:

    void signalNextItem();
    void signalPrevItem();
    void signalDeleteItem();
    void signalViewModeChanged();
    void signalEscapePreview();
    void signalSlideShowCurrent();
    void signalZoomFactorChanged(double);
    void signalPopupTagsView();
    void signalAddToExistingQueue(int);

    void signalGotoAlbumAndItem(const ItemInfo&);
    void signalGotoDateAndItem(const ItemInfo&);
    void signalGotoTagAndItem(int);

public Q_SLOTS:

    void slotEscapePreview();

private Q_SLOTS:

    void slotPreviewLoaded(bool);
    void slotZoomFactorChanged(double);
    void slotThumbBarSelectionChanged();
    void slotIconViewSelectionChanged();

private:

    void readSettings();
    void syncSelection(ItemCategorizedView* from, ItemCategorizedView* to);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_STACKED_VIEW_H
