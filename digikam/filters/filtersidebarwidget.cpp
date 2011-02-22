/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : tag filter view for the right sidebar
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmx dot net>
 * Copyright (C)      2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "filtersidebarwidget.moc"

// Qt includes

#include <QLabel>
#include <QLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QToolButton>

// KDE includes

#include <kselectaction.h>
#include <khbox.h>
#include <kmenu.h>
#include <kdebug.h>

// LibKDcraw includes

#include <libkdcraw/rexpanderbox.h>

// Local includes

#include "albumsettings.h"
#include "albummodel.h"
#include "contextmenuhelper.h"
#include "tagcheckview.h"
#include "colorlabelfilter.h"
#include "picklabelfilter.h"
#include "ratingfilter.h"
#include "mimefilter.h"

using namespace KDcrawIface;

namespace Digikam
{

class TagFilterView::TagFilterViewPriv
{
public:

    TagFilterViewPriv() :
        onRestoreTagFiltersAction(0),
        offRestoreTagFiltersAction(0),
        ignoreTagAction(0),
        includeTagAction(0),
        excludeTagAction(0),
        restoreTagFiltersAction(0),
        tagFilterModeAction(0),
        tagFilterModel(0)
    {
    }

    QAction*       onRestoreTagFiltersAction;
    QAction*       offRestoreTagFiltersAction;
    QAction*       ignoreTagAction;
    QAction*       includeTagAction;
    QAction*       excludeTagAction;

    KSelectAction* restoreTagFiltersAction;
    KSelectAction* tagFilterModeAction;

    TagModel*      tagFilterModel;
};

TagFilterView::TagFilterView(QWidget* parent, TagModel* tagFilterModel)
    : TagCheckView(parent, tagFilterModel), d(new TagFilterViewPriv)
{
    d->tagFilterModel             = tagFilterModel;

    d->restoreTagFiltersAction    = new KSelectAction(i18n("Restore Tag Filters"), this);
    d->onRestoreTagFiltersAction  = d->restoreTagFiltersAction->addAction(i18n("On"));
    d->offRestoreTagFiltersAction = d->restoreTagFiltersAction->addAction(i18n("Off"));

    d->tagFilterModeAction        = new KSelectAction(i18n("Tag Filter Mode"), this);
    d->ignoreTagAction            = d->tagFilterModeAction->addAction(i18n("Ignore This Tag"));
    d->includeTagAction           = d->tagFilterModeAction->addAction(KIcon("list-add"), i18n("Must Have This Tag"));
    d->excludeTagAction           = d->tagFilterModeAction->addAction(KIcon("list-remove"), i18n("Must Not Have This Tag"));
}

TagFilterView::~TagFilterView()
{
    delete d;
}

void TagFilterView::addCustomContextMenuActions(ContextMenuHelper& cmh, Album* album)
{
    TagCheckView::addCustomContextMenuActions(cmh, album);

    // restoring
    cmh.addAction(d->restoreTagFiltersAction);

    Qt::CheckState state = d->tagFilterModel->checkState(album);

    switch (state)
    {
        case Qt::Unchecked:
            d->tagFilterModeAction->setCurrentAction(d->ignoreTagAction);
            break;
        case Qt::PartiallyChecked:
            d->tagFilterModeAction->setCurrentAction(d->excludeTagAction);
            break;
        case Qt::Checked:
            d->tagFilterModeAction->setCurrentAction(d->includeTagAction);
            break;
    }

    cmh.addAction(d->tagFilterModeAction);

    d->onRestoreTagFiltersAction->setChecked(isRestoreCheckState());
    d->offRestoreTagFiltersAction->setChecked(!isRestoreCheckState());
}

void TagFilterView::handleCustomContextMenuAction(QAction* action, AlbumPointer<Album> album)
{
    TagCheckView::handleCustomContextMenuAction(action, album);

    if (!action)
    {
        return;
    }

    if (action == d->onRestoreTagFiltersAction)        // Restore TagFilters ON.
    {
        setRestoreCheckState(true);
    }
    else if (action == d->offRestoreTagFiltersAction)  // Restore TagFilters OFF.
    {
        setRestoreCheckState(false);
    }
    else if (action == d->ignoreTagAction)
    {
        albumModel()->setCheckState(album, Qt::Unchecked);
    }
    else if (action == d->includeTagAction)
    {
        albumModel()->setCheckState(album, Qt::Checked);
    }
    else if (action == d->excludeTagAction)
    {
        albumModel()->setCheckState(album, Qt::PartiallyChecked);
    }
}

// --------------------------------------------------------------------------------------------------------

class FilterSideBarWidget::FilterSideBarWidgetPriv
{
public:

    FilterSideBarWidgetPriv() :
        space(0),
        expanderVlay(0),
        tagFilterView(0),
        tagFilterSearchBar(0),
        tagOptionsBtn(0),
        tagOptionsMenu(0),
        tagFilterModel(0),
        tagOrCondAction(0),
        tagAndCondAction(0),
        tagMatchCond(ImageFilterSettings::OrCondition),
        colorLabelFilter(0),
        pickLabelFilter(0),
        ratingFilter(0),
        textFilter(0),
        mimeFilter(0),
        expbox(0)
    {
    }

    static const QString                   configLastShowUntaggedEntry;
    static const QString                   configMatchingConditionEntry;

    QWidget*                               space;
    QVBoxLayout*                           expanderVlay;

    TagFilterView*                         tagFilterView;
    SearchTextBar*                         tagFilterSearchBar;
    QToolButton*                           tagOptionsBtn;
    KMenu*                                 tagOptionsMenu;
    TagModel*                              tagFilterModel;
    QAction*                               tagOrCondAction;
    QAction*                               tagAndCondAction;
    ImageFilterSettings::MatchingCondition tagMatchCond;

    ColorLabelFilter*                      colorLabelFilter;
    PickLabelFilter*                       pickLabelFilter;
    RatingFilter*                          ratingFilter;
    SearchTextBar*                         textFilter;
    MimeFilter*                            mimeFilter;

    QCheckBox*                             withoutTagCheckBox;

    RExpanderBox*                          expbox;
};

const QString FilterSideBarWidget::FilterSideBarWidgetPriv::configLastShowUntaggedEntry("Show Untagged");
const QString FilterSideBarWidget::FilterSideBarWidgetPriv::configMatchingConditionEntry("Matching Condition");

// ---------------------------------------------------------------------------------------------------

FilterSideBarWidget::FilterSideBarWidget(QWidget* parent, TagModel* tagFilterModel)
    : KVBox(parent), StateSavingObject(this), d(new FilterSideBarWidgetPriv)
{
    setObjectName("TagFilter Sidebar");

    d->expbox = new RExpanderBox(this);
    d->expbox->setObjectName("FilterSideBarWidget Expander");

    // --------------------------------------------------------------------------------------------------------

    QWidget* box1 = new QWidget(d->expbox);
    d->textFilter = new SearchTextBar(box1, "AlbumIconViewFilterSearchTextBar");
    d->textFilter->setTextQueryCompletion(true);
    d->textFilter->setToolTip(i18n("Text quick filter (search)"));
    d->textFilter->setWhatsThis(i18n("Enter search patterns to quickly filter this view on "
                                     "file names, captions (comments), and tags"));

    QGridLayout* lay1 = new QGridLayout(box1);
    lay1->addWidget(d->textFilter, 0, 0, 1, 1);
    lay1->setMargin(0);
    lay1->setSpacing(0);

    d->expbox->addItem(box1, SmallIcon("text-field"), i18n("Text Filter"), QString("TextFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* box2 = new QWidget(d->expbox);
    d->mimeFilter = new MimeFilter(box2);

    QGridLayout* lay2 = new QGridLayout(box2);
    lay2->addWidget(d->mimeFilter, 0, 0, 1, 1);
    lay2->setMargin(0);
    lay2->setSpacing(0);

    d->expbox->addItem(box2, SmallIcon("system-file-manager"), i18n("Type Mime Filter"), QString("TypeMimeFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* box3         = new QWidget(d->expbox);
    d->tagFilterModel     = tagFilterModel;
    d->tagFilterView      = new TagFilterView(box3, tagFilterModel);
    d->tagFilterView->setObjectName("DigikamViewTagFilterView");
    d->tagFilterSearchBar = new SearchTextBar(box3, "DigikamViewTagFilterSearchBar");
    d->tagFilterSearchBar->setModel(d->tagFilterView->filteredModel(),
                                    AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagFilterSearchBar->setFilterModel(d->tagFilterView->albumFilterModel());

    const QString notTaggedTitle   = i18n("Images Without Tags");
    d->withoutTagCheckBox          = new QCheckBox(notTaggedTitle, box3);
    d->withoutTagCheckBox->setWhatsThis(i18n("Show images without a tag."));

    d->tagOptionsBtn = new QToolButton(box3);
    d->tagOptionsBtn->setToolTip( i18n("Tags Matching Condition"));
    d->tagOptionsBtn->setIcon(KIconLoader::global()->loadIcon("configure", KIconLoader::Toolbar));
    d->tagOptionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->tagOptionsBtn->setWhatsThis(i18n("Defines in which way the selected tags are combined "
                                        "to filter the images. This also includes the '%1' check box.",
                                        notTaggedTitle));

    d->tagOptionsMenu  = new KMenu(d->tagOptionsBtn);
    d->tagOrCondAction = d->tagOptionsMenu->addAction(i18n("OR"));
    d->tagOrCondAction->setCheckable(true);
    d->tagAndCondAction = d->tagOptionsMenu->addAction(i18n("AND"));
    d->tagAndCondAction->setCheckable(true);
    d->tagOptionsBtn->setMenu(d->tagOptionsMenu);

    QGridLayout* lay3 = new QGridLayout(box3);
    lay3->addWidget(d->tagFilterView,      0, 0, 1, 3);
    lay3->addWidget(d->tagFilterSearchBar, 1, 0, 1, 3);
    lay3->addWidget(d->withoutTagCheckBox, 2, 0, 1, 1);
    lay3->addWidget(d->tagOptionsBtn,      2, 2, 1, 1);
    lay3->setRowStretch(0, 100);
    lay3->setColumnStretch(1, 10);
    lay3->setMargin(0);
    lay3->setSpacing(0);

    d->expbox->addItem(box3, SmallIcon("tag-assigned"), i18n("Tags Filter"), QString("TagsFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* box4       = new QWidget(d->expbox);
    d->colorLabelFilter = new ColorLabelFilter(box4);
    d->pickLabelFilter  = new PickLabelFilter(box4);
    d->ratingFilter     = new RatingFilter(box4);

    QGridLayout* lay4   = new QGridLayout(box4);
    lay4->addWidget(d->colorLabelFilter, 0, 0, 1, 3);
    lay4->addWidget(d->pickLabelFilter,  1, 0, 1, 1);
    lay4->addWidget(d->ratingFilter,     1, 2, 1, 1);
    lay4->setColumnStretch(2, 1);
    lay4->setColumnStretch(3, 10);
    lay4->setMargin(0);
    lay4->setSpacing(0);

    d->expbox->addItem(box4, SmallIcon("favorites"), i18n("Labels Filter"), QString("LabelsFilter"), true);

    d->expanderVlay = dynamic_cast<QVBoxLayout*>(dynamic_cast<QScrollArea*>(d->expbox)->widget()->layout());
    d->space        = new QWidget();
    d->expanderVlay->addWidget(d->space);

    // --------------------------------------------------------------------------------------------------------

    connect(d->expbox, SIGNAL(signalItemExpanded(int, bool)),
            this, SLOT(slotItemExpanded(int, bool)));

    connect(d->mimeFilter, SIGNAL(activated(int)),
            this, SIGNAL(signalMimeTypeFilterChanged(int)));

    connect(d->textFilter, SIGNAL(signalSearchTextSettings(const SearchTextSettings&)),
            this, SIGNAL(signalTextFilterChanged(const SearchTextSettings&)));

    connect(d->tagFilterView, SIGNAL(checkedTagsChanged(const QList<TAlbum*>&, const QList<TAlbum*>&)),
            this, SLOT(slotCheckedTagsChanged(const QList<TAlbum*>&, const QList<TAlbum*>&)));

    connect(d->colorLabelFilter, SIGNAL(signalColorLabelSelectionChanged(const QList<ColorLabel>&)),
            this, SLOT(slotColorLabelFilterChanged(const QList<ColorLabel>&)));

    connect(d->pickLabelFilter, SIGNAL(signalPickLabelSelectionChanged(const QList<PickLabel>&)),
            this, SLOT(slotPickLabelFilterChanged(const QList<PickLabel>&)));

    connect(d->withoutTagCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotWithoutTagChanged(int)));

    connect(d->tagOptionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotTagOptionsTriggered(QAction*)));

    connect(d->tagOptionsMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotTagOptionsMenu()));

    connect(d->ratingFilter, SIGNAL(signalRatingFilterChanged(int, ImageFilterSettings::RatingCondition)),
            this, SIGNAL(signalRatingFilterChanged(int, ImageFilterSettings::RatingCondition)));
}

FilterSideBarWidget::~FilterSideBarWidget()
{
    delete d;
}

void FilterSideBarWidget::slotTagOptionsMenu()
{
    d->tagOrCondAction->setChecked(false);
    d->tagAndCondAction->setChecked(false);

    switch (d->tagMatchCond)
    {
        case ImageFilterSettings::OrCondition:
            d->tagOrCondAction->setChecked(true);
            break;
        case ImageFilterSettings::AndCondition:
            d->tagAndCondAction->setChecked(true);
            break;
    }
}

void FilterSideBarWidget::slotItemExpanded(int id, bool b)
{
    if (id == 2)
        d->expanderVlay->setStretchFactor(d->space, b ? 0 : 100);
}

void FilterSideBarWidget::setFocusToTextFilter()
{
    d->textFilter->setFocus();
}

void FilterSideBarWidget::slotFilterMatchesForText(bool match)
{
    d->textFilter->slotSearchResult(match);
}

void FilterSideBarWidget::slotResetFilters()
{
    d->textFilter->setText(QString());
    d->mimeFilter->setMimeFilter(MimeFilter::AllFiles);
    d->tagFilterView->slotResetCheckState();
    d->withoutTagCheckBox->setChecked(false);
    d->colorLabelFilter->reset();
    d->pickLabelFilter->reset();
    d->ratingFilter->setRating(0);
    d->ratingFilter->setRatingFilterCondition(ImageFilterSettings::GreaterEqualCondition);
    d->tagMatchCond = ImageFilterSettings::OrCondition;
}

void FilterSideBarWidget::slotTagOptionsTriggered(QAction* action)
{
    if (action)
    {
        if (action == d->tagOrCondAction)
        {
            d->tagMatchCond = ImageFilterSettings::OrCondition;
        }
        else if (action == d->tagAndCondAction)
        {
            d->tagMatchCond = ImageFilterSettings::AndCondition;
        }
    }

    filterChanged();
}

void FilterSideBarWidget::slotCheckedTagsChanged(const QList<TAlbum*>& includedTags,
                                                 const QList<TAlbum*>& excludedTags)
{
    Q_UNUSED(includedTags);
    Q_UNUSED(excludedTags);
    filterChanged();
}

void FilterSideBarWidget::slotColorLabelFilterChanged(const QList<ColorLabel>& list)
{
    Q_UNUSED(list);
    filterChanged();
}

void FilterSideBarWidget::slotPickLabelFilterChanged(const QList<PickLabel>& list)
{
    Q_UNUSED(list);
    filterChanged();
}

void FilterSideBarWidget::slotWithoutTagChanged(int newState)
{
    Q_UNUSED(newState);
    filterChanged();
}

void FilterSideBarWidget::filterChanged()
{
    bool showUntagged = d->withoutTagCheckBox->checkState() == Qt::Checked;

    QList<int> includedTagIds;
    QList<int> excludedTagIds;
    QList<int> clTagIds;
    QList<int> plTagIds;

    if (!showUntagged || d->tagMatchCond == ImageFilterSettings::OrCondition)
    {
        foreach (TAlbum* tag, d->tagFilterView->getCheckedTags())
        {
            if (tag)
            {
                includedTagIds << tag->id();
            }
        }
        foreach (TAlbum* tag, d->tagFilterView->getPartiallyCheckedTags())
        {
            if (tag)
            {
                excludedTagIds << tag->id();
            }
        }
        foreach (TAlbum* tag, d->colorLabelFilter->getCheckedColorLabelTags())
        {
            if (tag)
            {
                clTagIds << tag->id();
            }
        }
        foreach (TAlbum* tag, d->pickLabelFilter->getCheckedPickLabelTags())
        {
            if (tag)
            {
                plTagIds << tag->id();
            }
        }
    }

    emit signalTagFilterChanged(includedTagIds, excludedTagIds, d->tagMatchCond, showUntagged, clTagIds, plTagIds);
}

void FilterSideBarWidget::setConfigGroup(KConfigGroup group)
{
    StateSavingObject::setConfigGroup(group);
    d->tagFilterView->setConfigGroup(group);
}

void FilterSideBarWidget::doLoadState()
{
    d->expbox->readSettings();

    d->ratingFilter->setRatingFilterCondition((ImageFilterSettings::RatingCondition)
            (AlbumSettings::instance()->getRatingFilterCond()));

    d->tagMatchCond = (ImageFilterSettings::MatchingCondition)
                      (getConfigGroup().readEntry(entryName(d->configMatchingConditionEntry),
                                                  (int)ImageFilterSettings::OrCondition));

    d->tagFilterView->loadState();

    if (d->tagFilterView->isRestoreCheckState())
    {
        d->withoutTagCheckBox->setChecked(getConfigGroup().readEntry(entryName(
                                          d->configLastShowUntaggedEntry), false));
    }

    filterChanged();
}

void FilterSideBarWidget::doSaveState()
{
    d->expbox->writeSettings();

    AlbumSettings::instance()->setRatingFilterCond(d->ratingFilter->ratingFilterCondition());

    getConfigGroup().writeEntry(entryName(d->configMatchingConditionEntry), (int)d->tagMatchCond);

    d->tagFilterView->saveState();
    getConfigGroup().writeEntry(entryName(d->configLastShowUntaggedEntry),
                                d->withoutTagCheckBox->isChecked());
    getConfigGroup().sync();
}

} // namespace Digikam
