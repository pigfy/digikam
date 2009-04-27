/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-22
 * Description : Qt item view for images
 *
 * Copyright (C) 2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "imagecategorizedview.h"
#include "imagecategorizedview.moc"

// Qt includes

#include <QHelpEvent>
#include <QScrollBar>
#include <QStyle>

// KDE includes

#include <kdebug.h>

// Local includes

#include "album.h"
#include "databasefields.h"
#include "imagealbummodel.h"
#include "imagealbumfiltermodel.h"
#include "imagemodeldragdrophandler.h"
#include "imagethumbnailmodel.h"
#include "imagedelegate.h"
#include "imagecategorydrawer.h"
#include "itemviewtooltip.h"
#include "thumbnailloadthread.h"
#include "tooltipfiller.h"

namespace Digikam
{

class ImageItemViewToolTip : public ItemViewToolTip
{
public:

    ImageItemViewToolTip(ImageCategorizedView *view)
        : ItemViewToolTip(view)
    {
    }

    ImageCategorizedView *view() const
    { return static_cast<ImageCategorizedView*>(ItemViewToolTip::view()); }

protected:

    virtual QString tipContents()
    {
        ImageInfo info = ImageModel::retrieveImageInfo(currentIndex());
        return ToolTipFiller::imageInfoTipContents(info);
    }
};

// -------------------------------------------------------------------------------

class ImageCategorizedViewPriv
{
public:

    ImageCategorizedViewPriv()
    {
        model          = 0;
        filterModel    = 0;
        delegate       = 0;
        categoryDrawer = 0;
        toolTip        = 0;
        scrollToItemId = 0;
    }

    ImageAlbumModel         *model;
    ImageAlbumFilterModel   *filterModel;

    ImageDelegate           *delegate;
    ImageCategoryDrawer     *categoryDrawer;
    ImageItemViewToolTip    *toolTip;

    ThumbnailSize            thumbnailSize;
    qlonglong                scrollToItemId;

    QPoint                   mouseClickPosition;
    QList<QModelIndex>       indexesToThumbnail;
};

// -------------------------------------------------------------------------------

ImageCategorizedView::ImageCategorizedView(QWidget *parent)
                    : KCategorizedView(parent), d(new ImageCategorizedViewPriv)
{
    setViewMode(QListView::IconMode);
    setLayoutDirection(Qt::LeftToRight);
    setFlow(QListView::LeftToRight);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setWrapping(true);
    // important optimization to make layouting O(1)
    setUniformItemSizes(true);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragEnabled(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewport()->setAcceptDrops(true);
    setMouseTracking(true);

    d->model = new ImageAlbumModel(this);
    d->model->setThumbnailLoadThread(ThumbnailLoadThread::defaultIconViewThread());
    d->filterModel = new ImageAlbumFilterModel(this);
    d->filterModel->setSourceImageModel(d->model);

    // set flags that we want to get dataChanged() signals for
    DatabaseFields::Set watchFlags;
    watchFlags |= DatabaseFields::Name | DatabaseFields::FileSize | DatabaseFields::ModificationDate;
    watchFlags |= DatabaseFields::Rating | DatabaseFields::CreationDate | DatabaseFields::Orientation |
                  DatabaseFields::Width | DatabaseFields::Height;
    watchFlags |= DatabaseFields::Comment;
    d->model->setWatchFlags(watchFlags);

    d->delegate = new ImageDelegate(this);
    d->delegate->setSpacing(10);
    setItemDelegate(d->delegate);
    setCategoryDrawer(d->delegate->categoryDrawer());

    d->toolTip = new ImageItemViewToolTip(this);

    setModel(d->filterModel);

    connect(d->model, SIGNAL(signalImageInfosAdded(const QList<ImageInfo> &)),
            this, SLOT(slotImageInfosAdded()));

    connect(d->delegate, SIGNAL(gridSizeChanged(const QSize &)),
            this, SLOT(slotGridSizeChanged(const QSize &)));

    connect(d->delegate, SIGNAL(waitingForThumbnail(const QModelIndex &)),
            this, SLOT(slotDelegateWaitsForThumbnail(const QModelIndex &)));

    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(slotActivated(const ImageInfo &)));

    updateDelegateSizes();

    /*
    if (KGlobalSettings::singleClick())
    {
        connect(this, SIGNAL(clicked(const QModelIndex&)),
                controller, SLOT(triggerItem(const QModelIndex&)));
    }
    else
    {
        connect(this, SIGNAL(doubleClicked(const QModelIndex&)),
                controller, SLOT(triggerItem(const QModelIndex&)));
    }
    */
}

ImageCategorizedView::~ImageCategorizedView()
{
    delete d->toolTip;
    delete d;
}

ImageAlbumModel *ImageCategorizedView::imageModel() const
{
    return d->model;
}

ImageAlbumFilterModel *ImageCategorizedView::imageFilterModel() const
{
    return d->filterModel;
}

ImageDelegate *ImageCategorizedView::delegate() const
{
    return d->delegate;
}

ImageInfo ImageCategorizedView::currentInfo() const
{
    return d->filterModel->imageInfo(currentIndex());
}

QList<ImageInfo> ImageCategorizedView::selectedImageInfos() const
{
    return d->filterModel->imageInfos(selectedIndexes());
}

QList<ImageInfo> ImageCategorizedView::selectedImageInfosCurrentFirst() const
{
    QList<QModelIndex> indexes = selectedIndexes();
    QModelIndex        current = currentIndex();
    QList<ImageInfo>   infos;
    foreach (const QModelIndex &index, indexes)
    {
        ImageInfo info = d->filterModel->imageInfo(index);
        if (index == current)
            infos.prepend(info);
        else
            infos.append(info);
    }
    return infos;
}

QList<ImageInfo> ImageCategorizedView::imageInfos() const
{
    return d->filterModel->imageInfosSorted();
}

KUrl::List ImageCategorizedView::urls() const
{
    QList<ImageInfo> infos = imageInfos();
    KUrl::List urls;
    foreach (const ImageInfo &info, infos)
        urls << info.fileUrl();
    return urls;
}

KUrl::List ImageCategorizedView::selectedUrls() const
{
    QList<ImageInfo> infos = selectedImageInfos();
    KUrl::List urls;
    foreach (const ImageInfo &info, infos)
        urls << info.fileUrl();
    return urls;
}

void ImageCategorizedView::openAlbum(Album *album)
{
    d->model->openAlbum(album);
}

ThumbnailSize ImageCategorizedView::thumbnailSize() const
{
    return d->model->thumbnailSize();
}

void ImageCategorizedView::setThumbnailSize(int size)
{
    setThumbnailSize(ThumbnailSize(size));
}

void ImageCategorizedView::setThumbnailSize(const ThumbnailSize &size)
{
    d->model->setThumbnailSize(size);
    d->delegate->setThumbnailSize(size);
    //viewport()->update();
}

void ImageCategorizedView::scrollToWhenAvailable(qlonglong imageId)
{
    d->scrollToItemId = imageId;
}

void ImageCategorizedView::setCurrentUrl(const KUrl &url)
{
    QString path = url.path();
    QModelIndex index = d->filterModel->indexForPath(path);
    if (!index.isValid())
    {
        kWarning() << "no QModelIndex found for" << url;
        return;
    }
    setCurrentIndex(index);
}

void ImageCategorizedView::scrollToStoredItem()
{
    if (d->scrollToItemId)
    {
        if (d->model->hasImage(d->scrollToItemId))
        {
            scrollTo(d->filterModel->indexForImageId(d->scrollToItemId));
            d->scrollToItemId = 0;
        }
    }
}

void ImageCategorizedView::slotImageInfosAdded()
{
    if (d->scrollToItemId)
        scrollToStoredItem();
}

void ImageCategorizedView::slotThemeChanged()
{
    viewport()->update();
}

void ImageCategorizedView::slotSetupChanged()
{
    viewport()->update();
}

void ImageCategorizedView::slotGridSizeChanged(const QSize &gridSize)
{
    setGridSize(gridSize);

    horizontalScrollBar()->setSingleStep(gridSize.width() / 20);
    verticalScrollBar()->setSingleStep(gridSize.height() / 20);
}

void ImageCategorizedView::updateDelegateSizes()
{
    QStyleOptionViewItem option = viewOptions();
    /*int frameAroundContents = 0;
    if (style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents)) {
        frameAroundContents = style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2;
    }
    const int contentWidth = viewport()->width() - 1
                                - frameAroundContents
                                - style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, verticalScrollBar());
    const int contentHeight = viewport()->height() - 1
                                - frameAroundContents
                                - style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, horizontalScrollBar());
    option.rect = QRect(0, 0, contentWidth, contentHeight);
    */
    option.rect = QRect(QPoint(0,0), viewport()->size());
    d->delegate->setDefaultViewOptions(option);
}

void ImageCategorizedView::slotActivated(const QModelIndex &index)
{
    if (!d->mouseClickPosition.isNull())
    {
        // if the activation is caused by mouse click (not keyboard)
        // we need to check the hot area
        QStyleOptionViewItem option = viewOptions();
        option.rect = visualRect(index);
        if (!d->delegate->acceptsToolTip(d->mouseClickPosition, option, index))
            return;
    }

    ImageInfo info = d->filterModel->imageInfo(index);
    if (!info.isNull())
        activated(info);
}

void ImageCategorizedView::reset()
{
    KCategorizedView::reset();

    emit selectionChanged();
    emit selectionCleared();
}

void ImageCategorizedView::currentChanged(const QModelIndex &index, const QModelIndex &previous)
{
    KCategorizedView::currentChanged(index, previous);

    emit currentChanged(d->filterModel->imageInfo(index));
}

void ImageCategorizedView::selectionChanged(const QItemSelection &selectedItems, const QItemSelection &deselectedItems)
{
    KCategorizedView::selectionChanged(selectedItems, deselectedItems);

    emit selectionChanged();
    if (!selectedItems.isEmpty())
        emit selected(d->filterModel->imageInfos(selectedItems.indexes()));
    if (!deselectedItems.isEmpty())
        emit deselected(d->filterModel->imageInfos(deselectedItems.indexes()));
    if (!selectionModel()->hasSelection())
        emit selectionCleared();
}

void ImageCategorizedView::activated(const ImageInfo &)
{
    // implemented in subclass
}

void ImageCategorizedView::showContextMenu(QContextMenuEvent *, const ImageInfo &)
{
    // implemented in subclass
}

void ImageCategorizedView::showContextMenu(QContextMenuEvent *)
{
    // implemented in subclass
}

void ImageCategorizedView::copy()
{
    // implemented in subclass
}

void ImageCategorizedView::paste()
{
    // implemented in subclass
}

void ImageCategorizedView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid())
    {
        ImageInfo info = d->filterModel->imageInfo(index);
        showContextMenu(event, info);
    }
    else
        showContextMenu(event);
}

void ImageCategorizedView::mousePressEvent(QMouseEvent *event)
{
    const QModelIndex index = indexAt(event->pos());

    // Clear selection on click on empty area. Standard behavior, but not done by QAbstractItemView for some reason.
    if (!index.isValid()) {
        Qt::KeyboardModifiers modifiers = event->modifiers();
        const Qt::MouseButton button = event->button();
        const bool rightButtonPressed = button & Qt::RightButton;
        const bool shiftKeyPressed = modifiers & Qt::ShiftModifier;
        const bool controlKeyPressed = modifiers & Qt::ControlModifier;
        if (!index.isValid() && !rightButtonPressed && !shiftKeyPressed && !controlKeyPressed)
            clearSelection();
    }

    KCategorizedView::mousePressEvent(event);
}

void ImageCategorizedView::mouseReleaseEvent(QMouseEvent *event)
{
    // we store the position because the signals provide only the index
    d->mouseClickPosition = event->pos();
    KCategorizedView::mouseReleaseEvent(event);
    d->mouseClickPosition = QPoint();
}

void ImageCategorizedView::keyPressEvent(QKeyEvent *event)
{
    if (event == QKeySequence::Copy)
    {
        copy();
        event->accept();
        return;
    }
    else if (event == QKeySequence::Paste)
    {
        paste();
        event->accept();
        return;
    }

    /*
    // from dolphincontroller.cpp
    const QItemSelectionModel* selModel = m_itemView->selectionModel();
    const QModelIndex currentIndex = selModel->currentIndex();
    const bool trigger = currentIndex.isValid()
                         && ((event->key() == Qt::Key_Return)
                            || (event->key() == Qt::Key_Enter))
                         && (selModel->selectedIndexes().count() > 0);
    if (trigger) {
        const QModelIndexList indexList = selModel->selectedIndexes();
        foreach (const QModelIndex& index, indexList) {
            emit itemTriggered(itemForIndex(index));
        }
    }
    */
    KCategorizedView::keyPressEvent(event);
}

bool modelIndexByRowLessThan(const QModelIndex &i1, const QModelIndex &i2)
{
    return i1.row() < i2.row();
}

void ImageCategorizedView::slotDelegateWaitsForThumbnail(const QModelIndex &index)
{
    d->indexesToThumbnail << index;
}

void ImageCategorizedView::paintEvent(QPaintEvent *e)
{
    // We want the thumbnails to be loaded in order.
    // We cannot easily know which indexes are repainted, so we have to listen
    // to our delegate telling us for which thumbnails he waits.
    // After that we reorder them. See slotDelegateWaitsForThumbnail().
    d->indexesToThumbnail.clear();

    KCategorizedView::paintEvent(e);

    if (!d->indexesToThumbnail.isEmpty())
    {
        qSort(d->indexesToThumbnail.begin(), d->indexesToThumbnail.end(), modelIndexByRowLessThan);
        d->filterModel->prepareThumbnails(d->indexesToThumbnail);
        d->indexesToThumbnail.clear();
    }
}

void ImageCategorizedView::resizeEvent(QResizeEvent *e)
{
    KCategorizedView::resizeEvent(e);
    updateDelegateSizes();
}

bool ImageCategorizedView::viewportEvent(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::FontChange:
        {
            updateDelegateSizes();
            break;
        }
        case QEvent::ToolTip:
        {
            QHelpEvent *he = static_cast<QHelpEvent*>(event);
            const QModelIndex index = indexAt(he->pos());
            if (!index.isValid())
                break;
            QStyleOptionViewItem option = viewOptions();
            option.rect = visualRect(index);
            option.state |= (index == currentIndex() ? QStyle::State_HasFocus : QStyle::State_None);
            if (d->delegate->acceptsToolTip(he->pos(), option, index))
                d->toolTip->show(he, option, index);
            return true;
        }
        default:
            break;
    }
    return KCategorizedView::viewportEvent(event);
}

void ImageCategorizedView::dragMoveEvent(QDragMoveEvent *e)
{
    KCategorizedView::dragMoveEvent(e);
    ImageModelDragDropHandler *handler = d->model->dragDropHandler();
    if (handler)
    {
        QModelIndex index = indexAt(e->pos());
        Qt::DropAction action = handler->accepts(e->mimeData(), d->filterModel->mapToSource(index));
        if (action == Qt::IgnoreAction)
            e->ignore();
        else
        {
            e->setDropAction(action);
            e->accept();
        }
    }
}

void ImageCategorizedView::dropEvent(QDropEvent *e)
{
    KCategorizedView::dropEvent(e);
    ImageModelDragDropHandler *handler = d->model->dragDropHandler();
    if (handler)
    {
        QModelIndex index = indexAt(e->pos());
        if (handler->dropEvent(this, e, d->filterModel->mapToSource(index)))
            e->accept();
    }
}

} // namespace Digikam
