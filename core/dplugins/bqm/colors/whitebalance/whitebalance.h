/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-27
 * Description : White Balance batch tool.
 *
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BQM_WHITE_BALANCE_H
#define DIGIKAM_BQM_WHITE_BALANCE_H

// Local includes

#include "batchtool.h"
#include "wbsettings.h"

using namespace Digikam;

namespace DigikamBqmWhiteBalancePlugin
{

class WhiteBalance : public BatchTool
{
    Q_OBJECT

public:

    explicit WhiteBalance(QObject* const parent = nullptr);
    ~WhiteBalance();

    BatchToolSettings defaultSettings();

    BatchTool* clone(QObject* const parent=nullptr) const { return new WhiteBalance(parent); };

    void registerSettingsWidget();

private:

    bool toolOperations();

private Q_SLOTS:

    void slotAssignSettings2Widget();
    void slotSettingsChanged();

private:

    WBSettings* m_settingsView;
};

} // namespace DigikamBqmWhiteBalancePlugin

#endif // DIGIKAM_BQM_WHITE_BALANCE_H
