/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2006-01-10
 * Description : a widget to display CIE tonge from
 * an ICC profile.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * Any source code are inspired from lprof project and
 * Copyright (C) 1998-2001 Marti Maria
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

#define xmin(a,b) (a > b ? b : a)

// C++ includes.

#include <cmath>

// Qt includes.

#include <qimage.h>

// KDE includes.

#include <kdebug.h>

// Local includes.

#include "cietongewidget.h"

namespace Digikam
{

    // The  following  table  gives  the  CIE  colour  matching  functions
    // \bar{x}(\lambda),  \bar{y}(\lambda),  and   \bar{z}(\lambda),   for
    // wavelengths  \lambda  at 5 nanometre increments from 380 nm through
    // 780 nm.  This table is used in conjunction with  Planck's  law  for
    // the  energy spectrum of a black body at a given temperature to plot
    // the black body curve on the CIE chart. 
    
    // The following table gives the  spectral  chromaticity  co-ordinates
    // x(\lambda) and y(\lambda) for wavelengths in 5 nanometre increments
    // from 380 nm through  780  nm.   These  co-ordinates  represent  the
    // position in the CIE x-y space of pure spectral colours of the given
    // wavelength, and  thus  define  the  outline  of  the  CIE  "tongue"
    // diagram.
    
    static const double spectral_chromaticity[81][3] =
    {
        { 0.1741, 0.0050 },               // 380 nm 
        { 0.1740, 0.0050 },
        { 0.1738, 0.0049 },
        { 0.1736, 0.0049 },
        { 0.1733, 0.0048 },
        { 0.1730, 0.0048 },
        { 0.1726, 0.0048 },
        { 0.1721, 0.0048 },
        { 0.1714, 0.0051 },
        { 0.1703, 0.0058 },
        { 0.1689, 0.0069 },
        { 0.1669, 0.0086 },
        { 0.1644, 0.0109 },
        { 0.1611, 0.0138 },
        { 0.1566, 0.0177 },
        { 0.1510, 0.0227 },
        { 0.1440, 0.0297 },
        { 0.1355, 0.0399 },
        { 0.1241, 0.0578 },
        { 0.1096, 0.0868 },
        { 0.0913, 0.1327 },
        { 0.0687, 0.2007 },
        { 0.0454, 0.2950 },
        { 0.0235, 0.4127 },
        { 0.0082, 0.5384 },
        { 0.0039, 0.6548 },
        { 0.0139, 0.7502 },
        { 0.0389, 0.8120 },
        { 0.0743, 0.8338 },
        { 0.1142, 0.8262 },
        { 0.1547, 0.8059 },
        { 0.1929, 0.7816 },
        { 0.2296, 0.7543 },
        { 0.2658, 0.7243 },
        { 0.3016, 0.6923 },
        { 0.3373, 0.6589 },
        { 0.3731, 0.6245 },
        { 0.4087, 0.5896 },
        { 0.4441, 0.5547 },
        { 0.4788, 0.5202 },
        { 0.5125, 0.4866 },
        { 0.5448, 0.4544 },
        { 0.5752, 0.4242 },
        { 0.6029, 0.3965 },
        { 0.6270, 0.3725 },
        { 0.6482, 0.3514 },
        { 0.6658, 0.3340 },
        { 0.6801, 0.3197 },
        { 0.6915, 0.3083 },
        { 0.7006, 0.2993 },
        { 0.7079, 0.2920 },
        { 0.7140, 0.2859 },
        { 0.7190, 0.2809 },
        { 0.7230, 0.2770 },
        { 0.7260, 0.2740 },
        { 0.7283, 0.2717 },
        { 0.7300, 0.2700 },
        { 0.7311, 0.2689 },
        { 0.7320, 0.2680 },
        { 0.7327, 0.2673 },
        { 0.7334, 0.2666 },
        { 0.7340, 0.2660 },
        { 0.7344, 0.2656 },
        { 0.7346, 0.2654 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 },
        { 0.7347, 0.2653 }  // 780 nm 
    };

CIETongeWidget::CIETongeWidget(int w, int h, QWidget *parent, cmsHPROFILE hMonitor)
              : QWidget(parent, 0, Qt::WDestructiveClose)
{
    setMinimumSize(w, h);
    
    if (hMonitor)
        m_hMonitorProfile = hMonitor;
    else
        m_hMonitorProfile = cmsCreate_sRGBProfile();

    m_hXYZProfile = cmsCreateXYZProfile();
    m_hXFORM      = cmsCreateTransform(m_hXYZProfile, TYPE_XYZ_16,
                                       m_hMonitorProfile, TYPE_RGB_8,
                                       INTENT_PERCEPTUAL, 0);
}

CIETongeWidget::~CIETongeWidget()
{
    if (Measurement.Patches)
        free(Measurement.Patches);

    if (Measurement.Allowed)
        free(Measurement.Allowed);
        
    cmsDeleteTransform(m_hXFORM);
    cmsCloseProfile(m_hXYZProfile);
    cmsCloseProfile(m_hMonitorProfile);
}

void CIETongeWidget::setProfileData(QByteArray *profileData)
{
    cmsHPROFILE hProfile = cmsOpenProfileFromMem(profileData->data(),
                                                 (DWORD)profileData->size());

    // Get the white point.
    ZeroMemory(&MediaWhite, sizeof(cmsCIEXYZ)); 
    cmsTakeMediaWhitePoint(&MediaWhite, hProfile);
    cmsCIExyY White;
    cmsXYZ2xyY(&White, &MediaWhite);

    // Get the colorant matrix.
    ZeroMemory(&Primaries, sizeof(cmsCIExyYTRIPLE));
    
    if (cmsIsTag(hProfile, icSigRedColorantTag) &&
        cmsIsTag(hProfile, icSigGreenColorantTag) &&
        cmsIsTag(hProfile, icSigBlueColorantTag))
        {
        MAT3 Mat;

        if (cmsReadICCMatrixRGB2XYZ(&Mat, hProfile))
        {
            // Undo chromatic adaptation
            if (cmsAdaptMatrixFromD50(&Mat, &White))
            {
                cmsCIEXYZ tmp;

                tmp.X = Mat.v[0].n[0];
                tmp.Y = Mat.v[1].n[0];
                tmp.Z = Mat.v[2].n[0];

                // ScaleToWhite(&MediaWhite, &tmp);
                cmsXYZ2xyY(&Primaries.Red, &tmp);

                tmp.X = Mat.v[0].n[1];
                tmp.Y = Mat.v[1].n[1];
                tmp.Z = Mat.v[2].n[1];
                // ScaleToWhite(&MediaWhite, &tmp);
                cmsXYZ2xyY(&Primaries.Green, &tmp);
                
                tmp.X = Mat.v[0].n[2];
                tmp.Y = Mat.v[1].n[2];
                tmp.Z = Mat.v[2].n[2];
                // ScaleToWhite(&MediaWhite, &tmp);
                cmsXYZ2xyY(&Primaries.Blue, &tmp);
            }
        }       
    }

    // Get target data stored in profile
    ZeroMemory(&Measurement, sizeof(MEASUREMENT));
    char*  CharTarget;
    size_t CharTargetSize;

    if (cmsTakeCharTargetData(hProfile, &CharTarget, &CharTargetSize))
    {
        LCMSHANDLE hSheet = cmsxIT8LoadFromMem(CharTarget, CharTargetSize);
        if (hSheet != NULL)
        {
            cmsxPCollLoadFromSheet(&Measurement,  hSheet);
            cmsxIT8Free(hSheet);
            cmsxPCollValidatePatches(&Measurement, PATCH_HAS_XYZ|PATCH_HAS_RGB);
        }
    }

    cmsCloseProfile(hProfile);
    repaint(false);
}

void CIETongeWidget::MapPoint(int& icx, int& icy, LPcmsCIExyY xyY)
{
    icx = (int) floor((xyY->x * (m_pxcols - 1)) + .5);
    icy = (int) floor(((m_pxrows - 1) - xyY->y * (m_pxrows - 1)) + .5);
}

void CIETongeWidget::BiasedLine(int x1, int y1, int x2, int y2)
{
    m_pnt.drawLine(x1 + m_xBias, y1, x2 + m_xBias, y2);
}

void CIETongeWidget::BiasedText(int x, int y, const char* Txt)
{
    m_pnt.drawText(QPoint(m_xBias + x, y), Txt);
}

QRgb CIETongeWidget::ColorByCoord(double x, double y)
{
    // Get xyz components scaled from coordinates

    double cx =       ((double) x) / (m_pxcols - 1);
    double cy = 1.0 - ((double) y) / (m_pxrows - 1);
    double cz = 1.0 - cx - cy;

    // Project xyz to XYZ space. Note that in this
    // particular case we are substituting XYZ with xyz

    cmsCIEXYZ XYZ = { cx , cy , cz };

    WORD XYZW[3];
    BYTE RGB[3];

    cmsFloat2XYZEncoded(XYZW, &XYZ);
    cmsDoTransform(m_hXFORM, XYZW, RGB, 1);

    return qRgb(RGB[0], RGB[1], RGB[2]);
}

void CIETongeWidget::OutlineTonge()
{   
    int lx = 0, ly = 0;
    int fx=0, fy=0;

    for (int x = 380; x <= 700; x += 5)
    {
        int ix = (x - 380) / 5;

        cmsCIExyY p = {spectral_chromaticity[ix][0], 
                       spectral_chromaticity[ix][1], 1};    

        int icx, icy;

        MapPoint(icx, icy, &p);
    
        if (x > 380)
        {
            BiasedLine(lx, ly, icx, icy);
        }
        else
        {
            fx = icx;
            fy = icy;
        }

        lx = icx;
        ly = icy;
     }

    BiasedLine(lx, ly, fx, fy);
}

void CIETongeWidget::FillTonge()
{

    QImage Img = m_pix.convertToImage();

    int x;

    for (int y = 0; y < m_pxrows; y++)
    {
        int xe = 0;
                
        // Find horizontal extents of tongue on this line. 

        for (x = 0; x < m_pxcols; x++)
        {
            if ((QColor) Img.pixel(x + m_xBias, y) != Qt::black)
            {
                for (xe = m_pxcols - 1; xe >= x; xe--)
                {
                    if ((QColor) Img.pixel(xe + m_xBias, y) != Qt::black)
                    {
                        break;
                    }
                }
                
                break;
            }
        }

        if (x < m_pxcols)
        {
            for ( ; x <= xe; x++)
            {
                QRgb Color = ColorByCoord(x, y);
                Img.setPixel(x + m_xBias, y, Color);
            }
        }
    }

    m_pix.convertFromImage(Img, QPixmap::AvoidDither );
}

void CIETongeWidget::DrawTongeAxis()
{
    QFont font;

    font.setPointSize(8);
    m_pnt.setFont(font);
    
    m_pnt.setPen(qRgb(255, 255, 255));
    
    BiasedLine(0, 0,          0,          m_pxrows - 1);
    BiasedLine(0, m_pxrows-1, m_pxcols-1, m_pxrows - 1);

    for (int y = 1; y <= 9; y += 1)
    {
        char s[20];
        int xstart =  (y * (m_pxcols - 1)) / 10;
        int ystart =  (y * (m_pxrows - 1)) / 10;


        sprintf(s, "0.%d", y);                   // FIXME
        BiasedLine(xstart, m_pxrows - Grids(1), xstart,   m_pxrows - Grids(4));
        BiasedText(xstart - Grids(11), m_pxrows + Grids(15), s);
        
        sprintf(s, "0.%d", 10 - y);              // FIXME
        BiasedLine(0, ystart, Grids(3), ystart);    
        BiasedText(Grids(-25), ystart + Grids(5), s);
    }
}

void CIETongeWidget::DrawTongeGrid()
{       
    m_pnt.setPen(qRgb(80, 80, 80));
    
    for (int y = 1; y <= 9; y += 1)
    {
        int xstart =  (y * (m_pxcols - 1)) / 10;
        int ystart =  (y * (m_pxrows - 1)) / 10;

        BiasedLine(xstart, Grids(4), xstart,   m_pxrows - Grids(4) - 1);
        BiasedLine(Grids(7), ystart, m_pxcols-1-Grids(7), ystart);
    }
}

void CIETongeWidget::DrawLabels()
{
    QFont font;

    font.setPointSize(5);
    m_pnt.setFont(font);
    
    for (int x = 450; x <= 650; x += (x > 470 && x < 600) ? 5 : 10)
    {
        char wl[20];
        int bx = 0, by = 0, tx, ty;
        
        if (x < 520)
        {
            bx = Grids(-22);
            by = Grids(2);
        }
        else if (x < 535)
        {
            bx = Grids(-8);
            by = Grids(-6);
        }
        else
        {
            bx = Grids(4);
        }

        int ix = (x - 380) / 5;

        cmsCIExyY p = {spectral_chromaticity[ix][0],
                       spectral_chromaticity[ix][1], 1};
        
        int icx, icy;

        MapPoint(icx, icy, &p);

        tx = icx + ((x < 520) ? Grids(-2) : ((x >= 535) ? Grids(2) : 0));
        ty = icy + ((x < 520) ? 0 : ((x >= 535) ? Grids(-1) : Grids(-2))); 

        m_pnt.setPen(qRgb(255, 255, 255));
        BiasedLine(icx, icy, tx, ty);

        QRgb Color = ColorByCoord(icx, icy);
        m_pnt.setPen(Color);
        
        sprintf(wl, "%d", x);
        BiasedText(icx+bx, icy+by, wl);
    }
}

void CIETongeWidget::DrawSmallElipse(LPcmsCIExyY xyY, BYTE r, BYTE g, BYTE b, int sz)
{
    int icx, icy;

    MapPoint(icx, icy, xyY);
    m_pnt.setPen(qRgb(r, g, b));
    m_pnt.drawEllipse(icx + m_xBias- sz/2, icy-sz/2, sz, sz);
}

void CIETongeWidget::DrawPatches(LPMEASUREMENT m)
{
    for (int i=0; i < m->nPatches; i++)
    {
        LPPATCH p = m->Patches + i;

        if (m->Allowed[i])
        {
            LPcmsCIEXYZ XYZ = &p ->XYZ;
            cmsCIExyY xyY;              
            cmsXYZ2xyY(&xyY, XYZ);

            DrawSmallElipse(&xyY,  0, 0, 0, 4);

            if (p->dwFlags & PATCH_HAS_XYZ_PROOF)
            {
                if (p->XYZ.Y < 0.03)
                   continue;
                
                if (p->XYZProof.Y < 0.03)
                   continue;

                cmsCIExyY Pt;
                cmsXYZ2xyY(&Pt, &p->XYZProof);
                int icx1, icx2, icy1, icy2;

                MapPoint(icx1, icy1, &xyY);
                MapPoint(icx2, icy2, &Pt);

                if (icx2 < 5 || icy2 < 5 || icx1 < 5 || icy1 < 5)
                    continue;

                m_pnt.setPen(qRgb(255, 255, 0));
                BiasedLine(icx1, icy1, icx2, icy2);
            }
        }
    }
}

void CIETongeWidget::DrawTonge()
{   
    OutlineTonge();
    FillTonge();
    DrawTongeAxis();
    DrawLabels();
    DrawTongeGrid();
}

void CIETongeWidget::DrawColorantTriangle(LPcmsCIExyYTRIPLE Primaries)
{
    DrawSmallElipse(&Primaries->Red,   255, 128, 128, 6);
    DrawSmallElipse(&Primaries->Green, 128, 255, 128, 6);
    DrawSmallElipse(&Primaries->Blue,  128, 128, 255, 6);

    int x1, y1, x2, y2, x3, y3;

    MapPoint(x1, y1, &Primaries->Red);
    MapPoint(x2, y2, &Primaries->Green);
    MapPoint(x3, y3, &Primaries->Blue);

    m_pnt.setPen(qRgb(255, 255, 255));

    BiasedLine(x1, y1, x2, y2);
    BiasedLine(x2, y2, x3, y3);
    BiasedLine(x3, y3, x1, y1);
}

void CIETongeWidget::Sweep_sRGB(void)
{
    int r, g, b;
    cmsHPROFILE hXYZ, hsRGB;

    hXYZ = cmsCreateXYZProfile();
    hsRGB = cmsCreate_sRGBProfile();

    cmsHTRANSFORM xform = cmsCreateTransform(hsRGB, TYPE_RGB_16, hXYZ, TYPE_XYZ_16,
                                             INTENT_ABSOLUTE_COLORIMETRIC, cmsFLAGS_NOTPRECALC);
    WORD RGB[3], XYZ[3];
    cmsCIEXYZ xyz, MediaWhite;
    cmsCIExyY xyY, WhitePt;
    int x1, y1;

    cmsTakeMediaWhitePoint(&MediaWhite, hsRGB);
    cmsXYZ2xyY(&WhitePt, &MediaWhite);

    for (r=0; r < 65536; r += 1024)
    {
        for (g=0; g < 65536; g += 1024)
        {
            for (b=0; b < 65536; b += 1024)
            {
                RGB[0] = r; RGB[1] = g; RGB[2] = b;
                cmsDoTransform(xform, RGB, XYZ, 1);
                cmsXYZEncoded2Float(&xyz, XYZ);                     
                cmsXYZ2xyY(&xyY, &xyz);
                MapPoint(x1, y1, &xyY);
                m_pnt.drawPoint(x1 + m_xBias, y1);
            }
        }
    }

    cmsDeleteTransform(xform);
    cmsCloseProfile(hXYZ);
    cmsCloseProfile(hsRGB);
}

void CIETongeWidget::DrawWhitePoint(LPcmsCIEXYZ WhitePoint)
{
    cmsCIExyY Whitem_pntxyY;
    cmsXYZ2xyY(&Whitem_pntxyY, WhitePoint);
    DrawSmallElipse(&Whitem_pntxyY,  255, 255, 255, 8);
}       

void CIETongeWidget::paintEvent( QPaintEvent * )
{
    m_pix = QPixmap(size());
            
    m_pix.setOptimization(QPixmap::BestOptim);
    m_pix.fill(Qt::black);

    m_pnt.begin(&m_pix);
    
    int pixcols = m_pix.width();
    int pixrows = m_pix.height();

    m_gridside = (xmin(pixcols, pixrows)) / 512.0;
    
    m_xBias = Grids(32);
    m_yBias = Grids(20);

    m_pxcols = pixcols - m_xBias;
    m_pxrows = pixrows - m_yBias;
    
    m_pnt.setBackgroundColor(qRgb(0, 0, 0));
    m_pnt.setPen(qRgb(255, 255, 255));
    
    DrawTonge();

    if (MediaWhite.Y > 0.0)
        DrawWhitePoint(&MediaWhite);

    if (Primaries.Red.Y != 0.0)
        DrawColorantTriangle(&Primaries);

    if (Measurement.Patches && Measurement.Allowed)
        DrawPatches(&Measurement);
    
    m_pnt.end();

    bitBlt(this, 0, 0, &m_pix);
}

}  // namespace Digikam

#include "cietongewidget.moc"

