/* ============================================================
 *
 * Date        : 2008-02-10
 * Description : a tool to fix automatically camera lens aberrations
 *
 * Copyright (C) 2008      by Adrian Schroeter <adrian at suse dot de>
 * Copyright (C) 2008-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LENS_FUN_IFACE_H
#define DIGIKAM_LENS_FUN_IFACE_H

// Lib LensFun includes

// Pragma directives to reduce warnings from Lensfun header files.
#if defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <lensfun.h>

// Restore warnings
#if defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

// Local includes

#include "dmetadata.h"
#include "digikam_export.h"
#include "lensfunfilter.h"

namespace Digikam
{

class DIGIKAM_EXPORT LensFunIface
{
public:

    typedef const lfCamera* DevicePtr;
    typedef const lfLens*   LensPtr;
    typedef QList<LensPtr>  LensList;

    enum MetadataMatch
    {
        MetadataUnavailable  = -2,
        MetadataNoMatch      = -1,
        MetadataPartialMatch = 0,
        MetadataExactMatch   = 1
    };

public:

    explicit LensFunIface();
    virtual ~LensFunIface();

    void setFilterSettings(const LensFunContainer& other);

    void             setSettings(const LensFunContainer& other);
    LensFunContainer settings() const;

    LensPtr usedLens() const;
    void    setUsedLens(LensPtr lens);

    DevicePtr usedCamera() const;
    void      setUsedCamera(DevicePtr cam);

    lfDatabase*            lensFunDataBase() const;
    const lfCamera* const* lensFunCameras()  const;

    DevicePtr findCamera(const QString& make, const QString& model) const;
    LensPtr   findLens(const QString& model)                        const;

    MetadataMatch findFromMetadata(const DMetadata& meta);

    bool supportsDistortion() const;
    bool supportsCCA()        const;
    bool supportsVig()        const;
    bool supportsGeometry()   const;

    /** Return Camera maker string description found in metadata
    */
    QString makeDescription()  const;

    /** Return Camera model string description found in metadata
    */
    QString modelDescription() const;

    /** Return Lens string description found in metadata
     */
    QString lensDescription()  const;

    static QString lensFunVersion();

private:

    QString  metadataMatchDebugStr(MetadataMatch val) const;
    LensList findLenses(const lfCamera* const camera, const QString& lensDesc,
                        const QString& lensMaker=QString()) const;

private:

    LensFunIface(const LensFunIface&); // Disable

    class Private;
    Private* const d;
};

} // namespace Digikam

Q_DECLARE_METATYPE( Digikam::LensFunIface::DevicePtr )
Q_DECLARE_METATYPE( Digikam::LensFunIface::LensPtr )

#endif // DIGIKAM_LENS_FUN_IFACE_H
