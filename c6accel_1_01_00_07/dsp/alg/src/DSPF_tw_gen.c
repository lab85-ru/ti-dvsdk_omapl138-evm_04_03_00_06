/*==================================================================== */
/*  Copyright (c) 2010, Texas Instruments Incorporated                 */
/*  All rights reserved.                                               */
/*                                                                     */
/*                                                                     */
/* ======== twiddle_factor.c ========                                  */
/* Function for generating Specialized sequence of fixed point         */
/* twiddle factors                                                     */
/*                                                                     */
/*=====================================================================*/

void tw_gen (float *w, int n)
{
    int i, j, k;
    double x_t, y_t, theta1, theta2, theta3;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
            theta1 = 2 * PI * i / n;
            x_t = cos (theta1);
            y_t = sin (theta1);
            w[k] = (float) x_t;
            w[k + 1] = (float) y_t;

            theta2 = 4 * PI * i / n;
            x_t = cos (theta2);
            y_t = sin (theta2);
            w[k + 2] = (float) x_t;
            w[k + 3] = (float) y_t;

            theta3 = 6 * PI * i / n;
            x_t = cos (theta3);
            y_t = sin (theta3);
            w[k + 4] = (float) x_t;
            w[k + 5] = (float) y_t;
            k += 6;
        }
    }
}

/* Before Defining the floating point twiddle factor generating function,   */
/*   We define some datatype conversion functions                           */


/* ======================================================================== */
/*  D2F -- Truncate a 'double' to a 'int',   with clamping.                 */
/* ======================================================================== */
float d2f(double d)
{
    if (d >=  2147483647.0) return (float)0x7FFFFFFF;
    if (d <= -2147483648.0) return (float)0x80000000;
    return (float)d;
}

/* ======================================================================== */
/*  D2F_VEC -- Vector form of D2I                                           */
/* ======================================================================== */
void d2f_vec(double *d_vec,short *f_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(f_vec+i)= d2f(*(d_vec+i));
  }
}

/* ======================================================================== */
/*  F2D -- Truncate a 'double' to a 'int',   with clamping.                 */
/* ======================================================================== */
double f2d(float f)
{
    return (double)f;
}

/* ======================================================================== */
/*  F2D_VEC -- Vector form of F2D                                           */
/* ======================================================================== */
void f2d_vec(double *d_vec,float *f_vec, int n)
{
 int i;
 for(i=0;i<n;i++){  
    *(d_vec+i)= f2d(*(f_vec+i));
  }
}

/*=====================================================================*/
/*                                                                     */
/*  Version: 0.0.1                                                     */
/*==================================================================== */
