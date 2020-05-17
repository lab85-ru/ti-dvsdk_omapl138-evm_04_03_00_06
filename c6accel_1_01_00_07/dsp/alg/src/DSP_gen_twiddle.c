/*==========================================================================*/
/*  Copyright (c) 2010, Texas Instruments Incorporated                      */
/*  All rights reserved.                                                    */
/*                                                                          */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_fft16x16(short *w, int n)                       */
/*              gen_twiddle_fft16x16(int *w, int n)                         */
/*                                                                          */
/*          short *w      Pointer to twiddle-factor array                   */
/*          int   n       Size of FFT                                       */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */


/* Before Defining the fixed point twiddle factor generating function,      */
/*   We define some datatype conversion functions                           */

/* ======================================================================== */
/*  D2S -- Truncate a 'double' to a 'short', with clamping.                 */
/* ======================================================================== */
short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}

/* ======================================================================== */
/*  D2S_VEC -- Vector form of D2S                                           */
/* ======================================================================== */
void d2s_vec(double *d_vec,short *s_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(s_vec+i)= d2s(*(d_vec+i));
  }
}

/* ======================================================================== */
/*  D2I -- Truncate a 'double' to a 'int',   with clamping.                 */
/* ======================================================================== */
int d2i(double d)
{
    if (d >=  2147483647.0) return (int)0x7FFFFFFF;
    if (d <= -2147483648.0) return (int)0x80000000;
    return (int)d;
}

/* ======================================================================== */
/*  D2I_VEC -- Vector form of D2I                                           */
/* ======================================================================== */
void d2i_vec(double *d_vec,int *i_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(i_vec+i)= d2i(*(d_vec+i));
  }
}


/* ======================================================================== */
/*  i2d -- Truncate a 'double' to a 'int',   with clamping.                 */
/* ======================================================================== */
double i2d(int i)
{
    return (double)i;
}

/* ======================================================================== */
/*  i2d_VEC -- Vector form of i2d                                           */
/* ======================================================================== */
void i2d_vec(double *d_vec,int *i_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(d_vec+i)= i2d(*(i_vec+i));
  }
}
/* ======================================================================== */
/*  s2d -- Convert a 'short' to a 'double'                                  */
/* ======================================================================== */
double s2d(short s)
{
    return (double)s;
}

/* ======================================================================== */
/*  s2d_vec -- Vector form of s2d                                           */
/* ======================================================================== */
void s2d_vec(double *d_vec,short *s_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(d_vec+i)= s2d(*(s_vec+i));
  }
}



int gen_twiddle_fft16x16(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] =  d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] =  d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] = -d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] = -d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] = -d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] = -d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] =  d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] =  d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}

int gen_twiddle_fft32x32(int *w, int n, double scale)
{
    int i, j, k, s=0, t;
    const double PI = 3.141592654;
    for (j = 1, k = 0; j < n >> 2; j = j << 2, s++) {
        for (i = t=0; i < n >> 2; i += j, t++) {
            w[k +  5] = d2i(scale * cos(6.0 * PI * i / n));
            w[k +  4] = d2i(scale * sin(6.0 * PI * i / n));

            w[k +  3] = d2i(scale * cos(4.0 * PI * i / n));
            w[k +  2] = d2i(scale * sin(4.0 * PI * i / n));

            w[k +  1] = d2i(scale * cos(2.0 * PI * i / n));
            w[k +  0] = d2i(scale * sin(2.0 * PI * i / n));

            k += 6;
        }
    }
    return k;
}


/*=========================================================================*/
/*                                                                         */
/*  Version: 0.0.1                                                         */
/*======================================================================== */
