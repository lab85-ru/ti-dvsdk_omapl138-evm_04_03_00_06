/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */

int x = 0;
int y = 10;
int z = 20;

void func1() {
   long l;
   for (l = 0; l < 1000; l++) {
     x += y * z;
  }  
}

void func2() {
   long l;
   for (l = 0; l < 2000; l++) {
     x += y * z;
  }  
}


void func3() {
   long l;
   for (l = 0; l < 3000; l++) {
     x += y * z;
  }  
}

