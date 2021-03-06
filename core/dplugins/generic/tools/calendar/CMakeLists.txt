#
# Copyright (c) 2010-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(MacroDPlugins)

include_directories($<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
)

if(KF5CalendarCore_FOUND)
    include_directories($<TARGET_PROPERTY:KF5::CalendarCore,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

set(calendarplugin_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/print/calsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/print/calpainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/print/calprinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/print/calsystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/calintropage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/calmonthwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/caltemplate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/calwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wizard/calwizard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/calendarplugin.cpp
)

ki18n_wrap_ui(calendarplugin_SRCS
              ${CMAKE_CURRENT_SOURCE_DIR}/ui/caltemplate.ui
              ${CMAKE_CURRENT_SOURCE_DIR}/ui/calevents.ui
              ${CMAKE_CURRENT_SOURCE_DIR}/ui/calprogress.ui
)

if(KF5CalendarCore_FOUND)
    DIGIKAM_ADD_GENERIC_PLUGIN(NAME    Calendar
                                        SOURCES ${calendarplugin_SRCS}
                                        DEPENDS KF5::CalendarCore
    )
else()
    DIGIKAM_ADD_GENERIC_PLUGIN(NAME    Calendar
                                        SOURCES ${calendarplugin_SRCS}
    )
endif()
