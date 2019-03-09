#                                               -*- Autoconf -*-
# Process this file with `autoreconf -i` to create a 'configure' file.

#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
m4_define([ASKR_VERSION_S],[1.0.0])
m4_define([ASKR_VERSION_N],[1000000])

AC_PREREQ([2.69])
AC_INIT([Askr Search Framework], ASKR_VERSION_S(), [leif@ogre.com], [askr], [https://github.com/zwoop/askr])

AC_CONFIG_SRCDIR([src/askr.cc])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_AUX_DIR([build/_aux])

AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability tar-ustar foreign subdir-objects 1.9.2])
AM_MAINTAINER_MODE([enable])

# autoheader stuff
AH_TOP([
/**
 * @file
 * @brief The autoheader generated configuration include file, internal only
 */

#pragma once
])

# Configure with --disable-silent-rules to get verbose output. For more info, see
# http://www.gnu.org/software/automake/manual/html_node/Automake-silent_002drules-Option.html
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# AC_PROG can sometimes mangle CFLAGS etc.
# in particular, on Linux they insert -g -O2, here we preserve any user CFLAGS
askr_saved_CFLAGS="${CFLAGS}"
askr_saved_CXXFLAGS="${CXXFLAGS}"

AC_PROG_LIBTOOL
AC_PROG_CC([clang cc gcc])
AC_PROG_CXX([clang++ c++ g++])

CFLAGS="${_askr_saved_CFLAGS}"
CXXFLAGS="${_askr_saved_CXXFLAGS}"

# Various tools needed
AC_PATH_PROG([DOXYGEN], [doxygen])
AC_ARG_VAR([DOXYGEN], [full path of Doxygen executable])

# Library version details
ASKR_VERSION_MAJOR=$((ASKR_VERSION_N() / 1000000 ))
ASKR_VERSION_MINOR=$(((ASKR_VERSION_N() / 1000) % 1000 ))
ASKR_VERSION_MICRO=$((ASKR_VERSION_N() % 1000 ))
ASKR_VERSION_STRING=ASKR_VERSION_S()

AC_SUBST(ASKR_VERSION_MAJOR)
AC_SUBST(ASKR_VERSION_MINOR)
AC_SUBST(ASKR_VERSION_MICRO)
AC_SUBST(ASKR_VERSION_STRING)

#
# Debug
#
AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],[turn on debugging])],
  [],
  [enable_debug=no]
)
AC_MSG_RESULT([$enable_debug])

#
# Enble ccache explicitly (it's disabled by default, because of build problems in some cases)
#
AC_MSG_CHECKING([whether to enable ccache])
AC_ARG_ENABLE([ccache],
  [AS_HELP_STRING([--enable-ccache],[Enable ccache (for developers)])],
  [],
  [enable_ccache="no"]
)
AC_MSG_RESULT([$enable_ccache])

if test "x$enable_ccache" = "xyes"; then
    AC_CHECK_PROG([CCACHE],[ccache],[ccache],[])
    if test "x${CCACHE}" = "xccache"; then
	      CC="$CCACHE $CC"
        CXX="$CCACHE $CXX"
    fi
fi

if test "x${enable_debug}" = "xyes"; then
  TS_ADDTO(CFLAGS, [-g])
  TS_ADDTO(CXXFLAGS, [-g])
  TS_ADDTO(CPPFLAGS, [-DDEBUG -D_DEBUG])
else
  TS_ADDTO(CFLAGS, [-O3])
  TS_ADDTO(CXXFLAGS, [-O3])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Do this later, because otherwise the library and function checks can fail oddly (due to e.g. -Werror)
TS_ADDTO(CXXFLAGS, [-std=c++17 -pedantic -Wextra -Wall])
TS_ADDTO(CFLAGS, [-std=c11 -Wall])
TS_ADDTO(CPPFLAGS, [-D_GNU_SOURCE -I.. -I../gsl/include])

# Genereated files
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 lib/Makefile
                 lib/yaml-cpp/Makefile
                 plugins/Makefile
                 docs/Makefile
                 test/Makefile])

AC_OUTPUT
TS_CONFIG_NICE([config.nice])

# Produce a brief summary of how this build is being performed
AC_MSG_NOTICE([Build option summary:
    CC:                 $CC
    CXX:                $CXX
    CFLAGS:             $CFLAGS
    CXXFLAGS:           $CXXFLAGS
    CPPFLAGS:           $CPPFLAGS
    LDFLAGS:            $LDFLAGS
    LIBTOOL_LINK_FLAGS: $LIBTOOL_LINK_FLAGS
])
