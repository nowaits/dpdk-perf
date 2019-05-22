# Copyright (c) 2017 Cisco and/or its affiliates.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CFLAGS = -Wall -Wextra -I@TOOLKIT_INCLUDE@

portmirroring_plugin_la_CFLAGS = -DDEBUG -g

CFLAGS += -DDEBUG -g

# if VPP_ROOT is set, use it to compile
# eg. VPP_ROOT=~/vpp/build-root/install-vpp-native

CFLAGS += -I$(VPP_ROOT)/vpp/include
LDFLAGS += -L$(VPP_ROOT)/vpp/lib64

lib_LTLIBRARIES = process_test.la
process_test_la_SOURCES = \
    src/node.c \
    src/cli.c

process_test_la_LDFLAGS = -module
install-plugin:
	cp .libs/process_test.so $(VPP_ROOT)/vpp/lib/vpp_plugins
