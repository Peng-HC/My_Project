#*******************************************************************************
#  Copyright (c) 2015, 2020 logi.cals GmbH and others
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v2.0
#  and Eclipse Distribution License v1.0 which accompany this distribution.
#
#  The Eclipse Public License is available at
#     https://www.eclipse.org/legal/epl-2.0/
#  and the Eclipse Distribution License is available at
#    http://www.eclipse.org/org/documents/edl-v10.php.
#
#  Contributors:
#     Rainer Poisel - initial version
#     Ian Craggs (IBM Corp.) - merge master
#     Ian Craggs - update for MQTTV5 support
#*******************************************************************************/

# Note: on OS X you should install XCode and the associated command-line tools

## compilation/linkage settings

CONFIGURE_FILE(VersionInfo.h.in
    ${CMAKE_BINARY_DIR}/VersionInfo.h
    @ONLY
    )

message("fewfwef")

SET(common_src
        MQTTTime.c
        MQTTProtocolClient.c
        Clients.c
        utf-8.c
        MQTTPacket.c
        MQTTPacketOut.c
        Messages.c
        Tree.c
        Socket.c
        Log.c
        MQTTPersistence.c
        Thread.c
        MQTTProtocolOut.c
        MQTTPersistenceDefault.c
        SocketBuffer.c
        LinkedList.c
        MQTTProperties.c
        MQTTReasonCodes.c
        Base64.c
        SHA1.c
        WebSocket.c
)

SET(LIBRARY_OUTPUT_PATH /home/ruiy/mqtt/temp/cmake-build-debug/libs)

# 创建动态库
add_library(paho-mqtt3c SHARED MQTTClient.c)
add_library(paho-mqtt3a SHARED MQTTAsync.c)
add_library(paho-mqtt3as SHARED MQTTAsync.c SSLSocket.c)
add_library(paho-mqtt3cs SHARED MQTTClient.c SSLSocket.c)