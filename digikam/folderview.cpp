/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-05-06
 * Description : implementation of folder view.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QList>
#include <QPixmap>

// KDE includes.

#include <kglobalsettings.h>
#include <kcursor.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kconfiggroup.h>

// Local includes.

#include "albummanager.h"
#include "albumsettings.h"
#include "albumthumbnailloader.h"
#include "ddebug.h"
#include "themeengine.h"
#include "folderitem.h"
#include "folderview.h"
#include "folderview.moc"

namespace Digikam
{

class FolderViewPriv
{
public:

    FolderViewPriv()
    {
        active           = false;
        dragItem         = 0;
        oldHighlightItem = 0;
    }

    bool            active;

    int             itemHeight;

    QPixmap         itemRegPix;
    QPixmap         itemSelPix;

    QPoint          dragStartPos;

    Q3ListViewItem *dragItem;
    Q3ListViewItem *oldHighlightItem;
};

//-----------------------------------------------------------------------------

FolderView::FolderView(QWidget *parent, const char *name)
          : Q3ListView(parent)
{
    d = new FolderViewPriv;
    setObjectName(name);

    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    connect(AlbumManager::instance(), SIGNAL(signalAllAlbumsLoaded()),
            this, SLOT(slotAllAlbumsLoaded()));

    connect(AlbumThumbnailLoader::instance(), SIGNAL(signalReloadThumbnails()),
            this, SLOT(slotIconSizeChanged()));

    setColumnAlignment(0, Qt::AlignLeft|Qt::AlignVCenter);
    fontChange(font());
}

FolderView::~FolderView()
{
    saveViewState();
    delete d;
}

void FolderView::setActive(bool val)
{
    d->active = val;

    if (d->active)
        slotSelectionChanged();
}

bool FolderView::active() const
{
    return d->active;
}

int FolderView::itemHeight() const
{
    return d->itemHeight;
}

QRect FolderView::itemRect(Q3ListViewItem *item) const
{
    if(!item)
        return QRect();
    
    QRect r = Q3ListView::itemRect(item);
    r.setLeft(r.left()+(item->depth()+(rootIsDecorated() ? 1 : 0))*treeStepSize());
    return r;    
}

QPixmap FolderView::itemBasePixmapRegular() const
{
    return d->itemRegPix;    
}

QPixmap FolderView::itemBasePixmapSelected() const
{
    return d->itemSelPix;    
}

void FolderView::resizeEvent(QResizeEvent* e)
{
    Q3ListView::resizeEvent(e);

    int w = frameRect().width();
    int h = itemHeight();
    if (d->itemRegPix.width() != w ||
        d->itemRegPix.height() != h)
    {
        slotThemeChanged();
    }
}

void FolderView::fontChange(const QFont& oldFont)
{
    // this is bad, since the settings value might not always be the _real_ height of the thumbnail.
    // (e.g. when it is blended, as for the tags)
    d->itemHeight = qMax(AlbumThumbnailLoader::instance()->thumbnailSize() + 2*itemMargin(), fontMetrics().height());
    Q3ListView::fontChange(oldFont);
    slotThemeChanged();
}

void FolderView::slotIconSizeChanged()
{
    d->itemHeight = qMax(AlbumThumbnailLoader::instance()->thumbnailSize() + 2*itemMargin(), fontMetrics().height());
    slotThemeChanged();
}

void FolderView::contentsMouseMoveEvent(QMouseEvent *e)
{
    Q3ListView::contentsMouseMoveEvent(e);

    if(e->buttons() == Qt::NoButton)
    {
        if(KGlobalSettings::changeCursorOverIcon())
        {
            QPoint vp = contentsToViewport(e->pos());
            Q3ListViewItem *item = itemAt(vp);
            if (mouseInItemRect(item, vp.x()))
                setCursor(Qt::PointingHandCursor);
            else
                unsetCursor();
        }
        return;
    }

    if(d->dragItem && 
       (d->dragStartPos - e->pos()).manhattanLength() > QApplication::startDragDistance())
    {
        QPoint vp            = contentsToViewport(e->pos());
        Q3ListViewItem *item = itemAt(vp);
        if(!item)
        {
            d->dragItem = 0;
            return;
        }
    }
}

void FolderView::contentsMousePressEvent(QMouseEvent *e)
{
    QPoint vp            = contentsToViewport(e->pos());
    Q3ListViewItem *item = itemAt(vp);

    FolderCheckListItem *citem = dynamic_cast<FolderCheckListItem*>(item);
    if(citem && e->button() == Qt::MidButton) 
    {
        bool isOn = citem->isOn();
        Q3ListView::contentsMousePressEvent(e);
        // Restore the status of checkbox. 
        citem->setOn(isOn);
        return;
    }

    Q3ListView::contentsMousePressEvent(e);

    if(item && e->button() == Qt::LeftButton) 
    {
        d->dragStartPos = e->pos();
        d->dragItem     = item;
        return;
    }
}

void FolderView::contentsMouseReleaseEvent(QMouseEvent *e)
{
    Q3ListView::contentsMouseReleaseEvent(e);

    d->dragItem = 0;
}

void FolderView::startDrag()
{
    QDrag *o = makeDragObject();
    if(o)
        o->exec();
}

Q3ListViewItem* FolderView::dragItem() const
{
    return d->dragItem;
}

void FolderView::contentsDragEnterEvent(QDragEnterEvent *e)
{
    Q3ListView::contentsDragEnterEvent(e);
    e->accept();
}

void FolderView::contentsDragLeaveEvent(QDragLeaveEvent * e)
{
    Q3ListView::contentsDragLeaveEvent(e);

    if(d->oldHighlightItem)
    {
        FolderItem *fitem = dynamic_cast<FolderItem*>(d->oldHighlightItem);
        if (fitem)
            fitem->setFocus(false);
        else
        {
            FolderCheckListItem *citem = dynamic_cast<FolderCheckListItem*>(d->oldHighlightItem);
            if (citem)
                citem->setFocus(false);
        }

        d->oldHighlightItem->repaint();
        d->oldHighlightItem = 0;
    }
}

void FolderView::contentsDragMoveEvent(QDragMoveEvent *e)
{
    Q3ListView::contentsDragMoveEvent(e);

    QPoint vp            = contentsToViewport(e->pos());
    Q3ListViewItem *item = dynamic_cast<Q3ListViewItem*>(itemAt(vp));
    if(item)
    {
        if(d->oldHighlightItem)
        {
            FolderItem *fitem = dynamic_cast<FolderItem*>(d->oldHighlightItem);
            if (fitem)
                fitem->setFocus(false);
            else
            {
                FolderCheckListItem *citem = dynamic_cast<FolderCheckListItem*>(d->oldHighlightItem);
                if (citem)
                    citem->setFocus(false);
            }
            d->oldHighlightItem->repaint();
        }

        FolderItem *fitem = dynamic_cast<FolderItem*>(item);
        if (fitem)
            fitem->setFocus(true);
        else
        {
            FolderCheckListItem *citem = dynamic_cast<FolderCheckListItem*>(item);
            if (citem)
                citem->setFocus(true);
        }
        d->oldHighlightItem = item;
        item->repaint();
    }
    e->setAccepted(acceptDrop(e));
}

void FolderView::contentsDropEvent(QDropEvent *e)
{
    Q3ListView::contentsDropEvent(e);

    if(d->oldHighlightItem)
    {
        FolderItem *fitem = dynamic_cast<FolderItem*>(d->oldHighlightItem);
        if (fitem)
            fitem->setFocus(false);
        else
        {
            FolderCheckListItem *citem = dynamic_cast<FolderCheckListItem*>(d->oldHighlightItem);
            if (citem)
                citem->setFocus(false);
        }

        d->oldHighlightItem->repaint();
        d->oldHighlightItem = 0;
    }
}

bool FolderView::acceptDrop(const QDropEvent *) const
{
    return false;
}

bool FolderView::mouseInItemRect(Q3ListViewItem* item, int x) const
{
    if (!item)
        return false;

    x += contentsX();

    int offset = treeStepSize()*(item->depth() + (rootIsDecorated() ? 1 : 0));
    offset    += itemMargin();
    int width  = item->width(fontMetrics(), this, 0);

    return (x > offset && x < (offset + width));
}

void FolderView::slotThemeChanged()
{
    int w = frameRect().width();
    int h = itemHeight();

    d->itemRegPix = ThemeEngine::instance()->listRegPixmap(w, h);
    d->itemSelPix = ThemeEngine::instance()->listSelPixmap(w, h);

    viewport()->update();
}

void FolderView::slotAllAlbumsLoaded()
{
    disconnect(AlbumManager::instance(), SIGNAL(signalAllAlbumsLoaded()),
               this, SLOT(slotAllAlbumsLoaded()));
    loadViewState();
}

void FolderView::loadViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(objectName());

    int selectedItem = group.readEntry("LastSelectedItem", 0);

    QList<int> openFolders;
    if(group.hasKey("OpenFolders"))
    {
        openFolders = group.readEntry("OpenFolders",QList<int>());
    }

    FolderItem *item      = 0;
    FolderItem *foundItem = 0;
    Q3ListViewItemIterator it(this->lastItem());

    for( ; it.current(); --it)
    {
        item = dynamic_cast<FolderItem*>(it.current());
        if(!item)
            continue;
        // Start the album root always open
        if(openFolders.contains(item->id()) || item->id() == 0)
            setOpen(item, true);
        else
            setOpen(item, false);

        if(item->id() == selectedItem)
        {
            // Save the found selected item so that it can be made visible.
            foundItem = item;
        }
    }

    // Important note: this cannot be done inside the previous loop
    // because opening folders prevents the visibility.
    // Fixes bug #144815.
    // (Looks a bit like a bug in Qt to me ...)
    if (foundItem)
    {
        setSelected(foundItem, true);
        ensureItemVisible(foundItem);
    }
}

void FolderView::saveViewState()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(objectName());

    FolderItem *item = dynamic_cast<FolderItem*>(selectedItem());
    if(item)
        group.writeEntry("LastSelectedItem", item->id());
    else
        group.writeEntry("LastSelectedItem", 0);

    QList<int> openFolders;
    Q3ListViewItemIterator it(this);
    for( ; it.current(); ++it)
    {
        item = dynamic_cast<FolderItem*>(it.current());
        if(item && isOpen(item))
            openFolders.push_back(item->id());
    }
    group.writeEntry("OpenFolders", openFolders);
}

void FolderView::slotSelectionChanged()
{
    Q3ListView::selectionChanged();
}

void FolderView::selectItem(int)
{
}

}  // namespace Digikam
