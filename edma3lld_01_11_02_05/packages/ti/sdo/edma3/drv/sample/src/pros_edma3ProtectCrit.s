 ;
 ; pros_edma3ProtectCrit.s
 ;
 ; This file implements OS Protect Interrupt call. Since PrOS does not provide
 ; a function that stores previous status of interrupts (cpsr register), this
 ; function shall do the same.
 ;
 ; Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 ;
 ;
 ;  Redistribution and use in source and binary forms, with or without
 ;  modification, are permitted provided that the following conditions
 ;  are met:
 ;
 ;    Redistributions of source code must retain the above copyright
 ;    notice, this list of conditions and the following disclaimer.
 ;
 ;    Redistributions in binary form must reproduce the above copyright
 ;    notice, this list of conditions and the following disclaimer in the
 ;    documentation and/or other materials provided with the
 ;    distribution.
 ;
 ;    Neither the name of Texas Instruments Incorporated nor the names of
 ;    its contributors may be used to endorse or promote products derived
 ;    from this software without specific prior written permission.
 ;
 ;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ;  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ;  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ;  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ;  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ;  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ;  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ;  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ;  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ;  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ;

                .global         _Edma3_critsec32
                .global         _Edma3_endcrit32

                .if __TI_ARM9ABI_ASSEMBLER
                .armfunc         _Edma3_critsec32
                .armfunc         _Edma3_endcrit32
                .endif

                .global         _gintmask

                .text

                .align
                .state32
_Edma3_critsec32:
                mrs             r1, cpsr
                mov             r0,r1
                orr             r1, r1, #0xc0
                msr             cpsr, R1
                bx              r14

                .align
                .state32
_Edma3_endcrit32:
                msr             cpsr, R0
                bx              r14

                .end
