/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to create calendar.
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

#include "advprintintropage.h"

// Qt includes

#include <QLabel>
#include <QPixmap>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "calwizard.h"

namespace Digikam
{

AdvPrintIntroPage::AdvPrintIntroPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title)
{
    DVBox* const vbox  = new DVBox(this);
    QLabel* const desc = new QLabel(vbox);

    desc->setWordWrap(true);
    desc->setOpenExternalLinks(true);
    desc->setText(i18n("<qt>"
                       "<p><h1><b>Welcome to Print Creator</b></h1></p>"
                       "<p>This assistant will guide you to assemble images "
                       "to be printed following specific templates as Photo Album, "
                       "Photo Collage, or Framed Photo.</p>"
                       "<p>An adaptive photo collection page layout can be also used, "
                       "based on Atkins algorithm.</p>"
                       "</qt>"));

    setPageWidget(vbox);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("document-print")));
}

AdvPrintIntroPage::~AdvPrintIntroPage()
{
}

} // namespace Digikam