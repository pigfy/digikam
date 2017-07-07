/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to print images
 *
 * Copyright (C) 2017 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "advprintphotopage.h"

// Qt includes

#include <QIcon>
#include <QPrinter>
#include <QPrinterInfo>
#include <QWidget>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "advprintwizard.h"

namespace Digikam
{

class AdvPrintPhotoPage::Private
{
public:

    template <class Ui_Class>

    class WizardUI : public QWidget, public Ui_Class
    {
    public:

        WizardUI(QWidget* const parent)
            : QWidget(parent)
        {
            this->setupUi(this);
        }
    };

    typedef WizardUI<Ui_AdvPrintPhotoPage> PhotoUI;

public:

    Private(QWidget* const parent)
      : printer(0)
    {
        photoUi = new PhotoUI(parent);
    }

    PhotoUI*            photoUi;
    QPrinter*           printer;
    QList<QPrinterInfo> printerList;
};

AdvPrintPhotoPage::AdvPrintPhotoPage(QWizard* const wizard, const QString& title)
    : DWizardPage(wizard, title),
      d(new Private(this))
{
    d->photoUi->BtnPreviewPageUp->setIcon(QIcon::fromTheme(QLatin1String("go-next"))
                                            .pixmap(16, 16));
    d->photoUi->BtnPreviewPageDown->setIcon(QIcon::fromTheme(QLatin1String("go-previous"))
                                              .pixmap(16, 16));

    QList<QPrinterInfo>::iterator it;
    d->printerList = QPrinterInfo::availablePrinters();

    qCDebug(DIGIKAM_GENERAL_LOG) << " printers: " << d->printerList.count();

    for (it = d->printerList.begin() ; it != d->printerList.end() ; ++it)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << " printer: " << it->printerName();
        d->photoUi->m_printer_choice->addItem(it->printerName());
    }

    connect(d->photoUi->m_printer_choice, SIGNAL(activated(QString)),
            this, SLOT(slotOutputChanged(QString)));

    connect(d->photoUi->BtnPreviewPageUp, SIGNAL(clicked()),
            wizard, SLOT(slotBtnPreviewPageUpClicked()));

    connect(d->photoUi->BtnPreviewPageDown, SIGNAL(clicked()),
            wizard, SLOT(slotBtnPreviewPageDownClicked()));

    connect(d->photoUi->ListPhotoSizes, SIGNAL(currentRowChanged(int)),
            wizard, SLOT(slotListPhotoSizesSelected()));

    connect(d->photoUi->m_pagesetup, SIGNAL(clicked()),
            wizard, SLOT(slotPageSetup()));

    if (d->photoUi->mPrintList->layout())
    {
        delete d->photoUi->mPrintList->layout();
    }

    // -----------------------------------

    d->photoUi->mPrintList->setAllowDuplicate(true);
    d->photoUi->mPrintList->setControlButtons(DImagesList::Add      |
                                    DImagesList::Remove   |
                                    DImagesList::MoveUp   |
                                    DImagesList::MoveDown |
                                    DImagesList::Clear    |
                                    DImagesList::Save     |
                                    DImagesList::Load);
    d->photoUi->mPrintList->setControlButtonsPlacement(DImagesList::ControlButtonsAbove);
    d->photoUi->mPrintList->enableDragAndDrop(false);

    d->photoUi->BmpFirstPagePreview->setAlignment(Qt::AlignHCenter);

    connect(d->photoUi->mPrintList, SIGNAL(signalMoveDownItem()),
            wizard, SLOT(slotBtnPrintOrderDownClicked()));

    connect(d->photoUi->mPrintList, SIGNAL(signalMoveUpItem()),
            wizard, SLOT(slotBtnPrintOrderUpClicked()));

    connect(d->photoUi->mPrintList, SIGNAL(signalAddItems(QList<QUrl>)),
            wizard, SLOT(slotAddItems(QList<QUrl>)));

    connect(d->photoUi->mPrintList, SIGNAL(signalRemovingItem(int)),
            wizard, SLOT(slotRemovingItem(int)));

    connect(d->photoUi->mPrintList, SIGNAL(signalContextMenuRequested()),
            wizard, SLOT(slotContextMenuRequested()));

    // Save item list => we catch the signal to add our PA attributes and elements Image children
    connect(d->photoUi->mPrintList, SIGNAL(signalXMLSaveItem(QXmlStreamWriter&, int)),
            wizard, SLOT(slotXMLSaveItem(QXmlStreamWriter&, int)));

    // Save item list => we catch the signal to add our PA elements (not per image)
    connect(d->photoUi->mPrintList, SIGNAL(signalXMLCustomElements(QXmlStreamWriter&)),
            wizard, SLOT(slotXMLCustomElement(QXmlStreamWriter&)));

    connect(d->photoUi->mPrintList, SIGNAL(signalXMLLoadImageElement(QXmlStreamReader&)),
            wizard, SLOT(slotXMLLoadElement(QXmlStreamReader&)));

    connect(d->photoUi->mPrintList, SIGNAL(signalXMLCustomElements(QXmlStreamReader&)),
            wizard, SLOT(slotXMLCustomElement(QXmlStreamReader&)));

    // -----------------------------------

    setPageWidget(d->photoUi);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("image-stack")));

    slotOutputChanged(d->photoUi->m_printer_choice->currentText());
}

AdvPrintPhotoPage::~AdvPrintPhotoPage()
{
    delete d->printer;
    delete d;
}

QPrinter* AdvPrintPhotoPage::printer() const
{
    return d->printer;
}

DImagesList* AdvPrintPhotoPage::imagesList() const
{
    return d->photoUi->mPrintList;
}

Ui_AdvPrintPhotoPage* AdvPrintPhotoPage::ui() const
{
    return d->photoUi;
}

void AdvPrintPhotoPage::slotOutputChanged(const QString& text)
{
    if (text == i18n("Print to PDF") ||
        text == i18n("Print to JPG") ||
        text == i18n("Print with Gimp"))
    {
        delete d->printer;

        d->printer = new QPrinter();
        d->printer->setOutputFormat(QPrinter::PdfFormat);
    }
    else // real printer
    {
        for (QList<QPrinterInfo>::iterator it = d->printerList.begin() ;
             it != d->printerList.end() ; ++it)
        {
            if (it->printerName() == text)
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Chosen printer: " << it->printerName();
                delete d->printer;
                d->printer = new QPrinter(*it);
            }
        }

        d->printer->setOutputFormat(QPrinter::NativeFormat);
    }

    // default no margins

    d->printer->setFullPage(true);
    d->printer->setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
}

void AdvPrintPhotoPage::updateUi()
{
    d->photoUi->update();
}

bool AdvPrintPhotoPage::isComplete() const
{
    return (!d->photoUi->mPrintList->imageUrls().empty());
}

} // namespace Digikam
