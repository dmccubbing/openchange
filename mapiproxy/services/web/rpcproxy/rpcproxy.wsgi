#!/usr/bin/python
#
# rpcproxy.wsgi -- OpenChange RPC-over-HTTP implementation
#
# Copyright (C) 2012  Julien Kerihuel <j.kerihuel@openchange.org>
#                     Wolfgang Sourdeau <wsourdeau@inverse.ca>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#   
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#   
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# this is the WSGI starting point for rpcproxy

import sys

print >>sys.stderr, "path: %s" % ":".join(sys.path)

import logging

from openchange.web.auth.NTLMAuthHandler import *
from RPCProxyApplication import *


SOCKETS_DIR = "/tmp/rpcproxy"
SAMBA_HOST = "127.0.0.1"
LOG_LEVEL = logging.INFO

application = NTLMAuthHandler(RPCProxyApplication(samba_host=SAMBA_HOST),
                              samba_host=SAMBA_HOST)
