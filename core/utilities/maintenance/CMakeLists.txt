#
# Copyright (c) 2010-2019 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(libmaintenance_SRCS
    maintenancedata.cpp
    iteminfojob.cpp
    iteminfoalbumsjob.cpp
    metadatatask.cpp
    metadatasynchronizer.cpp
    duplicatesfinder.cpp
    databasetask.cpp
    dbcleaner.cpp
    newitemsfinder.cpp
    thumbsgenerator.cpp
    thumbstask.cpp
    facesdetector.cpp
    fingerprintsgenerator.cpp
    fingerprintstask.cpp
    imagequalitysorter.cpp
    imagequalitytask.cpp
    maintenancedlg.cpp
    maintenancemngr.cpp
    maintenancetool.cpp
    maintenancesettings.cpp
    maintenancethread.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::Solid,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(maintenance_src OBJECT ${libmaintenance_SRCS})
