/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2017-08-08
 * Description : Base functions for dnn module, can be used for face recognition, 
 *               all codes are ported from dlib library (http://dlib.net/)
 *
 * Copyright (C) 2006-2016 by Davis E. King <davis at dlib dot net>
 * Copyright (C) 2017      by Yingjie Liu <yingjiewudi at gmail dot com>
 * Copyright (C) 2017-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DLIB_sIMD4F_Hh_
#define DLIB_sIMD4F_Hh_

#include "simd_check.h"
#include "simd4i.h"
#include <cmath>
#include <iostream>

#ifdef DLIB_HAVE_SSE2
    class simd4f
    {
    public:
        typedef float type;

        inline simd4f() {}
        inline simd4f(float f) { x = _mm_set1_ps(f); }
        inline simd4f(float r0, float r1, float r2, float r3) { x = _mm_setr_ps(r0,r1,r2,r3); }
        inline simd4f(const __m128& val):x(val) {}
        inline simd4f(const simd4i& val):x(_mm_cvtepi32_ps(val)) {}

        inline simd4f& operator=(const simd4i& val)
        {
            x = simd4f(val);
            return *this;
        }

        inline simd4f& operator=(const float& val)
        {
            x = simd4f(val);
            return *this;
        }

        inline simd4f& operator=(const __m128& val)
        {
            x = val;
            return *this;
        }

        inline operator __m128() const { return x; }

        // truncate to 32bit integers
        inline operator __m128i() const { return _mm_cvttps_epi32(x); }

        inline void load_aligned(const type* ptr)  { x = _mm_load_ps(ptr); }
        inline void store_aligned(type* ptr) const { _mm_store_ps(ptr, x); }
        inline void load(const type* ptr)          { x = _mm_loadu_ps(ptr); }
        inline void store(type* ptr)         const { _mm_storeu_ps(ptr, x); }

        inline unsigned int size() const { return 4; }
        inline float operator[](unsigned int idx) const 
        {
            float temp[4];
            store(temp);
            return temp[idx];
        }

    private:
        __m128 x;
    };

    class simd4f_bool
    {
    public:
        typedef float type;

        inline simd4f_bool() {}
        inline simd4f_bool(const __m128& val):x(val) {}

        inline simd4f_bool& operator=(const __m128& val)
        {
            x = val;
            return *this;
        }

        inline operator __m128() const { return x; }


    private:
        __m128 x;
    };

#elif defined(DLIB_HAVE_VSX)

    class simd4f
    {    
        typedef union {
            vector float v;
            float x[4];
        } v4f;
        
        v4f x;
        
    public:
        inline simd4f() : x{0,0,0,0} {}
        inline simd4f(const simd4f& v) : x(v.x) { }
        inline simd4f(const vector float& v) : x{v} { }

        inline simd4f(const simd4i& v) {
            x.x[0]=v[0]; x.x[1]=v[1]; x.x[2]=v[2]; x.x[3]=v[3];
        }
        
        
        inline simd4f(float f) : x{f,f,f,f} { }
        inline simd4f(float r0, float r1, float r2, float r3)
             : x{r0,r1,r2,r3} { }

        inline simd4f& operator=(const simd4f& v) { x = v.x; return *this; }
        inline simd4f& operator=(const float& v) { *this = simd4f(v); return *this; }

        inline vector float operator() () const { return x.v; }
        inline float operator[](unsigned int idx) const { return x.x[idx]; }
        
        inline void load_aligned(const float* ptr)  { x.v = vec_ld(0, ptr); }
        inline void store_aligned(float* ptr) const { vec_st(x.v, 0, ptr); }
        inline void load(const float* ptr) { x.v = vec_vsx_ld(0, ptr); }
        inline void store(float* ptr) const { vec_vsx_st(x.v, 0, ptr); }
        
        
        // truncate to 32bit integers
        inline operator simd4i::rawarray() const 
        { 
            simd4i::rawarray temp;
            temp.v.x[0] = x.x[0];
            temp.v.x[1] = x.x[1];
            temp.v.x[2] = x.x[2];
            temp.v.x[3] = x.x[3];
            return temp;
        }
    };

    typedef simd4i simd4f_bool;

#else
    class simd4f
    {
    public:
        typedef float type;

        inline simd4f() {}
        inline simd4f(float f) { x[0]=f; x[1]=f; x[2]=f; x[3]=f; }
        inline simd4f(float r0, float r1, float r2, float r3) { x[0]=r0; x[1]=r1; x[2]=r2; x[3]=r3;}
        inline simd4f(const simd4i& val) { x[0]=val[0]; x[1]=val[1]; x[2]=val[2]; x[3]=val[3];}

        // truncate to 32bit integers
        inline operator simd4i::rawarray() const 
        { 
            simd4i::rawarray temp;
            temp.a[0] = (int32)x[0];
            temp.a[1] = (int32)x[1];
            temp.a[2] = (int32)x[2];
            temp.a[3] = (int32)x[3];
            return temp;
        }

        inline simd4f& operator=(const float& val)
        {
            *this = simd4f(val);
            return *this;
        }

        inline simd4f& operator=(const simd4i& val)
        {
            x[0] = val[0];
            x[1] = val[1];
            x[2] = val[2];
            x[3] = val[3];
            return *this;
        }


        inline void load_aligned(const type* ptr)
        {
            x[0] = ptr[0];
            x[1] = ptr[1];
            x[2] = ptr[2];
            x[3] = ptr[3];
        }

        inline void store_aligned(type* ptr) const
        {
            ptr[0] = x[0];
            ptr[1] = x[1];
            ptr[2] = x[2];
            ptr[3] = x[3];
        }

        inline void load(const type* ptr)
        {
            x[0] = ptr[0];
            x[1] = ptr[1];
            x[2] = ptr[2];
            x[3] = ptr[3];
        }

        inline void store(type* ptr) const
        {
            ptr[0] = x[0];
            ptr[1] = x[1];
            ptr[2] = x[2];
            ptr[3] = x[3];
        }

        inline unsigned int size() const { return 4; }
        inline float operator[](unsigned int idx) const { return x[idx]; }

    private:
        float x[4];
    };

    class simd4f_bool
    {
    public:
        typedef float type;

        inline simd4f_bool() {}
        inline simd4f_bool(bool r0, bool r1, bool r2, bool r3) { x[0]=r0; x[1]=r1; x[2]=r2; x[3]=r3;}

        inline bool operator[](unsigned int idx) const { return x[idx]; }
    private:
        bool x[4];
    };
#endif

// ----------------------------------------------------------------------------------------

    inline std::ostream& operator<<(std::ostream& out, const simd4f& item)
    {
        float temp[4];
        item.store(temp);
        out << "(" << temp[0] << ", " << temp[1] << ", " << temp[2] << ", " << temp[3] << ")";
        return out;
    }

// ----------------------------------------------------------------------------------------

    inline simd4f operator+ (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_add_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_add(lhs(), rhs());
#else
        return simd4f(lhs[0]+rhs[0],
                      lhs[1]+rhs[1],
                      lhs[2]+rhs[2],
                      lhs[3]+rhs[3]);
#endif
    }
    inline simd4f& operator+= (simd4f& lhs, const simd4f& rhs) 
    { lhs = lhs + rhs; return lhs; }

// ----------------------------------------------------------------------------------------

    inline simd4f operator- (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_sub_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_sub(lhs(), rhs());
#else
        return simd4f(lhs[0]-rhs[0],
                      lhs[1]-rhs[1],
                      lhs[2]-rhs[2],
                      lhs[3]-rhs[3]);
#endif
    }
    inline simd4f& operator-= (simd4f& lhs, const simd4f& rhs) 
    { lhs = lhs - rhs; return lhs; }

// ----------------------------------------------------------------------------------------

    inline simd4f operator* (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_mul_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_mul(lhs(), rhs());
#else
        return simd4f(lhs[0]*rhs[0],
                      lhs[1]*rhs[1],
                      lhs[2]*rhs[2],
                      lhs[3]*rhs[3]);
#endif
    }
    inline simd4f& operator*= (simd4f& lhs, const simd4f& rhs) 
    { lhs = lhs * rhs; return lhs; }

// ----------------------------------------------------------------------------------------

    inline simd4f operator/ (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_div_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_div(lhs(), rhs());
#else
        return simd4f(lhs[0]/rhs[0],
                      lhs[1]/rhs[1],
                      lhs[2]/rhs[2],
                      lhs[3]/rhs[3]);
#endif
    }
    inline simd4f& operator/= (simd4f& lhs, const simd4f& rhs) 
    { lhs = lhs / rhs; return lhs; }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator== (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_cmpeq_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_cmpeq(lhs(), rhs());
#else
        return simd4f_bool(lhs[0]==rhs[0],
                           lhs[1]==rhs[1],
                           lhs[2]==rhs[2],
                           lhs[3]==rhs[3]);
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator!= (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_cmpneq_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return ~(lhs==rhs);     // simd4f_bool is simd4i typedef, can use ~
#else
        return simd4f_bool(lhs[0]!=rhs[0],
                           lhs[1]!=rhs[1],
                           lhs[2]!=rhs[2],
                           lhs[3]!=rhs[3]);
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator< (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_cmplt_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_cmplt(lhs(), rhs());
#else
        return simd4f_bool(lhs[0]<rhs[0],
                           lhs[1]<rhs[1],
                           lhs[2]<rhs[2],
                           lhs[3]<rhs[3]);
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator> (const simd4f& lhs, const simd4f& rhs) 
    { 
        return rhs < lhs;
    }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator<= (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_cmple_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_cmple(lhs(), rhs());
#else
        return simd4f_bool(lhs[0]<=rhs[0],
                           lhs[1]<=rhs[1],
                           lhs[2]<=rhs[2],
                           lhs[3]<=rhs[3]);
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f_bool operator>= (const simd4f& lhs, const simd4f& rhs) 
    { 
        return rhs <= lhs;
    }

// ----------------------------------------------------------------------------------------

    inline simd4f min (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_min_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_min(lhs(), rhs());
#else
        return simd4f(std::min(lhs[0],rhs[0]),
                      std::min(lhs[1],rhs[1]),
                      std::min(lhs[2],rhs[2]),
                      std::min(lhs[3],rhs[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f max (const simd4f& lhs, const simd4f& rhs) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_max_ps(lhs, rhs); 
#elif defined(DLIB_HAVE_VSX)
        return vec_max(lhs(), rhs());
#else
        return simd4f(std::max(lhs[0],rhs[0]),
                      std::max(lhs[1],rhs[1]),
                      std::max(lhs[2],rhs[2]),
                      std::max(lhs[3],rhs[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f reciprocal (const simd4f& item) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_rcp_ps(item); 
#elif defined(DLIB_HAVE_VSX)
        return vec_re(item());
#else
        return simd4f(1.0f/item[0],
                      1.0f/item[1],
                      1.0f/item[2],
                      1.0f/item[3]);
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f reciprocal_sqrt (const simd4f& item) 
    { 
#ifdef DLIB_HAVE_SSE2
        return _mm_rsqrt_ps(item); 
#elif defined(DLIB_HAVE_VSX)
        return vec_rsqrt(item());
#else
        return simd4f(1.0f/std::sqrt(item[0]),
                      1.0f/std::sqrt(item[1]),
                      1.0f/std::sqrt(item[2]),
                      1.0f/std::sqrt(item[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    inline float dot(const simd4f& lhs, const simd4f& rhs);
    inline float sum(const simd4f& item)
    {
#ifdef DLIB_HAVE_SSE41
        return dot(simd4f(1), item);
#elif defined(DLIB_HAVE_SSE3)
        simd4f temp = _mm_hadd_ps(item,item);
        return _mm_cvtss_f32(_mm_hadd_ps(temp,temp));
#elif defined(DLIB_HAVE_SSE2) && (!defined(_MSC_VER) || _MSC_VER!=1400)
        simd4f temp = _mm_add_ps(item,_mm_movehl_ps(item,item));
        simd4f temp2 = _mm_shuffle_ps(temp,temp,1);
        return _mm_cvtss_f32(_mm_add_ss(temp,temp2));
#else
        return item[0]+item[1]+item[2]+item[3];
#endif
    }

// ----------------------------------------------------------------------------------------

    inline float dot(const simd4f& lhs, const simd4f& rhs)
    {
#ifdef DLIB_HAVE_SSE41
        return _mm_cvtss_f32(_mm_dp_ps(lhs, rhs, 0xff));
#else
        return sum(lhs*rhs);
#endif
    }
   
// ----------------------------------------------------------------------------------------

    inline simd4f sqrt(const simd4f& item)
    {
#ifdef DLIB_HAVE_SSE2
        return _mm_sqrt_ps(item);
#elif defined(DLIB_HAVE_VSX)
        return vec_sqrt(item());
#else
        return simd4f(std::sqrt(item[0]),
                      std::sqrt(item[1]),
                      std::sqrt(item[2]),
                      std::sqrt(item[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f ceil(const simd4f& item)
    {
#ifdef DLIB_HAVE_SSE41
        return _mm_ceil_ps(item);
#elif defined(DLIB_HAVE_SSE2)
        float temp[4];
        item.store(temp);
        temp[0] = std::ceil(temp[0]);
        temp[1] = std::ceil(temp[1]);
        temp[2] = std::ceil(temp[2]);
        temp[3] = std::ceil(temp[3]);
        simd4f temp2;
        temp2.load(temp);
        return temp2;
#elif defined(DLIB_HAVE_VSX)
        return vec_ceil(item());
#else
        return simd4f(std::ceil(item[0]),
                      std::ceil(item[1]),
                      std::ceil(item[2]),
                      std::ceil(item[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    inline simd4f floor(const simd4f& item)
    {
#ifdef DLIB_HAVE_SSE41
        return _mm_floor_ps(item);
#elif defined(DLIB_HAVE_SSE2)
        float temp[4];
        item.store(temp);
        temp[0] = std::floor(temp[0]);
        temp[1] = std::floor(temp[1]);
        temp[2] = std::floor(temp[2]);
        temp[3] = std::floor(temp[3]);
        simd4f temp2;
        temp2.load(temp);
        return temp2;
#elif defined(DLIB_HAVE_VSX)
        return vec_floor(item());
#else
        return simd4f(std::floor(item[0]),
                      std::floor(item[1]),
                      std::floor(item[2]),
                      std::floor(item[3]));
#endif
    }

// ----------------------------------------------------------------------------------------

    // perform cmp ? a : b
    inline simd4f select(const simd4f_bool& cmp, const simd4f& a, const simd4f& b)
    {
#ifdef DLIB_HAVE_SSE41
        return _mm_blendv_ps(b,a,cmp);
#elif defined(DLIB_HAVE_SSE2)
        return _mm_or_ps(_mm_and_ps(cmp,a) , _mm_andnot_ps(cmp,b));
#else
        return simd4f(cmp[0]?a[0]:b[0],
                      cmp[1]?a[1]:b[1],
                      cmp[2]?a[2]:b[2],
                      cmp[3]?a[3]:b[3]);
#endif
    }

// ----------------------------------------------------------------------------------------



#endif // DLIB_sIMD4F_Hh_

