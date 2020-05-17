/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Use of this software is controlled by the terms and conditions found in
 * the New BSD Software License Agreement, which is reproduced in its
 * entirity below.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 *  ======== getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 *
 *  The 'this' object is the current package object being included by
 *  the program being configured.  Thus, it is possible to
 *  conditionally return library names based on package attributes
 *  including the program object itself (this.prog).
 *
 *  Parameters
 *      this            - the current package object
 *      this.prog       - the program being configured (after all of the
 *                        program's configuration scripts complete)
 *      this.prog.build - program attributes specified in the build script
 *
 *  Returns
 *      null        - indicates that no library is to be linked
 *      non-null    - name of the library to link with (relative to the
 *                    package directory).
 */
function getLibs() {
    /* "mangle" program build attrs into an appropriate directory name */
    var name = "lib/logservercgi";

    /* return the library name: name.a<arch> */
    var lib = name + ".a" + prog.build.target.suffix;
    print("    will link with " + this.$name + ":" + lib);

    return (lib);
}

/*
 */

/*
 *  @(#) ti.bios.log.ndk; 1, 0, 2, 02,31; 5-23-2009 17:29:07; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

