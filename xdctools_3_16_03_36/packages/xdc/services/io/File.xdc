/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * --/COPYRIGHT--*/
/*!
 *  ======== File ========
 *  File I/O
 */
metaonly module File {

    /*!
     *  ======== exists ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Bool exists(String name);

    /*!
     *  ======== getDOSPath ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    String getDOSPath(String longPath);

    /*!
     *  ======== isDirectory ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Bool isDirectory(String name);

    /*!
     *  ======== isFile ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Bool isFile(String name);

    /*!
     * Get the canonical pathname to a file or directory.
     *
     * The canonical pathname is absolute and unique, and uses Unix-style
     * forward slashes.
     *
     * The file or directory need not already exist.
     */
    String getCanonicalPath(String name);

    /*!
     *  ======== open ========
     *
     *  Return an instance of xdc.services.io.File
     *
     *  @param(name)		name of the underlying file
     *
     *  @param(mode)		"r" - for reading, "w" - for opening and writing
     *				into a new file or overwriting an exisiting file
     *				and "a" for appending to a file
     *
     *  @a(return)		xdc.services.io.File instance
     *
     *  @a(throws)		'Error' exceptions are thrown if the access mode
     *				for the file does not allow reading or writing,
     *				or if an incorrect mode string is given
     */
    Instance open(String name, String mode);

instance:
    
    /*!
     *  ======== getStream ========
     *
     *  Returns the underlying Java stream for this File
     *
     *  If the file was open for writing or appending, the returned stream is
     *  of the type java.io.PrintStream. If the file was open for reading, the
     *  returned stream is of the type java.io.BufferedReader.
     *
     *  @a(returns)		underlying Java stream
     */
    Any getStream();

    /*!
     *  ======== readLine ========
     *
     *  Reads a line from this File
     *
     *  @a(returns)		string containing the next line or null if the
     *				end of the file is reached
     */
    String readLine();
    
    /*!
     *  ======== writeLine ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Bool writeLine(String out);
 
    /*!
     *  ======== printf ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Void printf(String fmt, ...);
    
    /*!
     *  ======== close ========
     *
     *  UNDER CONSTRUCTION
     *
     * @_nodoc
     */
    Void close();
}
