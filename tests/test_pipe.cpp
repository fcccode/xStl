/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

/*
* test_pipe.cpp
*
* Tests the
*
* Author: Elad Raz <e@eladraz.com>
*/

#include "xStl/types.h"
#include "xStl/os/os.h"
#include "xStl/data/string.h"
#include "xStl/data/array.h"
#include "xStl/data/smartptr.h"
#include "xStl/except/trace.h"
#include "xStl/except/assert.h"
#include "xStl/stream/pipeStream.h"
#include "xStl/stream/ioStream.h"
#include "xStl/os/threadedClass.h"
#include "tests.h"
#include "sampleProtocol.h"

class cTestPipe : public cTestObject {
public:
    static const char pipeName[];
    static bool isError;
    static cBuffer data;

    class TestServerSide : public cThreadedClass {
    public:
        virtual void run()
        {
            XSTL_TRY
            {
                BasicIOPtr server = cPipeStream::listenOnPipe(cTestPipe::pipeName);
                cSampleProtocol::testServerSide(*server, cout);
            }
            XSTL_CATCH_ALL
            {
                cout << "PIPE-SERVER: Error!!" << endl;
                isError = true;
            }
        }
    };

    class TestClientSide : public cThreadedClass {
    public:
        virtual void run()
        {
            XSTL_TRY
            {
                // Wait until the server will be up and running
                cOS::sleepMillisecond(1000);
                BasicIOPtr client = BasicIOPtr(new cPipeStream(cTestPipe::pipeName));
                cNullStringerStream null;
                cSampleProtocol::testClientSide(*client, null);
            }
            XSTL_CATCH_ALL
            {
                cout << "PIPE-CLIENT: Error!!" << endl;
                isError = true;
            }
        }
    };

    // Perform the test
    virtual void test()
    {
        TestServerSide server;  server.start();
        TestClientSide client;  client.start();
        server.wait();
        TESTS_ASSERT(!isError);
    }

    // Return the name of the module
    virtual cString getName() { return __FILE__; }
};

const char cTestPipe::pipeName[] = "RaziTestxStl";
bool cTestPipe::isError = false;
cBuffer cTestPipe::data;

// Instance test object
cTestPipe g_globalTestPipe;
