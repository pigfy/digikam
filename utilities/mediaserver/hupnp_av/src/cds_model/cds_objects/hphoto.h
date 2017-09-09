/*
 *  Copyright (C) 2011 Tuomo Penttinen, all rights reserved.
 *
 *  Author: Tuomo Penttinen <tp@herqq.org>
 *
 *  This file is part of Herqq UPnP Av (HUPnPAv) library.
 *
 *  Herqq UPnP Av is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Herqq UPnP Av is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Herqq UPnP Av. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HPHOTO_H_
#define HPHOTO_H_

#include <../../HUpnpAv/HImageItem>

namespace Herqq
{

namespace Upnp
{

namespace Av
{

class HPhotoPrivate;

/*!
 * \brief This class represents a photo object.
 *
 * The class identifier specified by the AV Working Committee is
 * \c object.item.imageItem.photo.
 *
 * \headerfile hphoto.h HPhoto
 *
 * \ingroup hupnp_av_cds_objects
 *
 * \remarks This class is not thread-safe.
 */
class DIGIKAM_EXPORT HPhoto :
    public HImageItem
{
Q_OBJECT
H_DISABLE_COPY(HPhoto)
H_DECLARE_PRIVATE(HPhoto)

protected:

    /*!
     * Constructs a new instance.
     *
     * \param clazz specifies the UPnP class of the object. This cannot be empty.
     *
     * \param cdsType specifies the CDS type of the object. This cannot be
     * HObject::UndefinedCdsType.
     *
     * \sa isInitialized()
     */
    HPhoto(const QString& clazz = sClass(), CdsType cdsType = sType());
    HPhoto(HPhotoPrivate&);

    // Documented in HClonable
    virtual HPhoto* newInstance() const;

public:

    /*!
     * \brief Creates a new instance.
     *
     * \param title specifies the title of the object.
     *
     * \param parentId specifies the ID of the object that contains this
     * object. If the object has no parent, this has to be left empty.
     *
     * \param id specifies the ID of this object. If this is not specified,
     * a unique identifier within the running process is created for the object.
     *
     * \sa isValid()
     */
    HPhoto(
        const QString& title,
        const QString& parentId,
        const QString& id = QString());

    /*!
     * \brief Destroys the instance.
     */
    virtual ~HPhoto();

    /*!
     * \brief Returns the albums to which this item belongs.
     *
     * upnp:album, ContentDirectory:3, Appendix B.4.2.
     *
     * \return The albums to which this item belongs.
     *
     * \sa setAlbums()
     */
    QStringList albums() const;

    /*!
     * \brief Specifies the albums to which this item belongs.
     *
     * \param arg the albums to which this item belongs.
     *
     * \sa albums()
     */
    void setAlbums(const QStringList& arg);

    /*!
     * \brief Returns the CdsType value of this class.
     *
     * \return The CdsType value of this class.
     */
    inline static CdsType sType() { return Photo; }

    /*!
     * \brief Returns the class identifier specified by the AV Working Committee.
     *
     * \return The class identifier specified by the AV Working Committee.
     */
    inline static QString sClass() { return QLatin1String("object.item.imageItem.photo"); }

    /*!
     * Creates a new instance with no title or parentID.
     *
     * \return a pointer to the newly created instance.
     *
     * \remarks the ownership of the object is transferred to the caller. Make sure
     * to delete the object.
     */
    inline static HPhoto* create() { return new HPhoto(); }
};

}
}
}

#endif /* HPHOTO_H_ */
