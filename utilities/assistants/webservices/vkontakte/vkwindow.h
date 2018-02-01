/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-11-15
 * Description : a tool to export images to VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Luka Renko <lure at kubuntu dot org>
 * Copyright (C) 2010      by Roman Tsisyk <roman at tsisyk dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef VK_WINDOW_H
#define VK_WINDOW_H

// Local includes

#include "wstooldialog.h"
#include "digikam_export.h"
#include "dprogresswdg.h"
#include "dinfointerface.h"
#include "dimageslist.h"

class QLabel;
class KJob;

namespace Vkontakte
{
    class VkApi;
}

namespace Digikam
{

class VKAlbumChooser;
class VKAuthWidget;

class DIGIKAM_EXPORT VKWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit VKWindow(DInfoInterface* const iface,
                             QWidget* const parent,
                             bool import = false);
    ~VKWindow();

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void startReactivation();

Q_SIGNALS:

    void signalUpdateBusyStatus(bool busy);

protected Q_SLOTS:

    // requesting photo information
    void slotPhotoUploadDone(KJob* kjob);

    void slotStartTransfer();

    void slotFinished();

    void updateBusyStatus(bool busy);
    void updateBusyStatusReady();

    void authenticated();
    void authCleared();
    void updateHeaderLabel();

protected:

    void initAccountBox();

    void readSettings();
    void writeSettings();

    void reset();

    void handleVkError(KJob* kjob);

    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

protected:

    bool                           m_import;

    /// User interface
    QWidget*                       m_mainWidget;
    QWidget*                       m_settingsBox;
    QLabel*                        m_headerLabel;

    /// accounts
    VKAuthWidget*                  m_accountBox;

    // album selection
    VKAlbumChooser*                m_albumsBox;

    /// options
    //QCheckBox*                    m_checkKeepOriginal;

    DImagesList*                   m_imgList;
    QWidget*                       m_uploadWidget;
    DInfoInterface*                m_iface;
    DProgressWdg*                  m_progressBar;

    /// Pointers to running jobs
    QList<KJob*>                   m_jobs;

    Vkontakte::VkApi*              m_vkapi;

    int                            m_albumToSelect;

    QString                        m_appId;
};

} // namespace Digikam

#endif // VK_WINDOW_H