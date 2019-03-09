#
# Top-level Makefile.am for the HTTP Key library
#
#  Licensed to the Apache Software Foundation (ASF) under one or more contributor license agreements.  See the NOTICE
#  file distributed with this work for additional information regarding copyright ownership.  The ASF licenses this
#  file to you under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
#  an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
#  specific language governing permissions and limitations under the License.

bin_PROGRAMS = askr

askr_CPPFLAGS = \
        $(AM_CPPFLAGS) \
        -DASKR_VERSION=\"$(ASKR_VERSION_STRING)\" \
        -I$(abs_top_srcdir)/include \
        -I$(abs_top_srcdir)/lib/gsl/include \
		-I$(abs_top_srcdir)/lib/yaml-cpp/include

askr_LDADD   = \
    -L${abs_top_builddir}/lib/yaml-cpp \
    -lyaml-cpp \
	-ljemalloc

askr_SOURCES = \
	askr.cc \
	options.cc \
	options.h \
	yaml.cc \
	yaml.h \
	key_values.cc
