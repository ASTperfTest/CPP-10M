//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef _CmpiPropertyMI_h_
#define _CmpiPropertyMI_h_

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"
#include "CmpiBaseMI.h"
#include "CmpiStatus.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiContext.h"
#include "CmpiArgs.h"
#include "Linkage.h"

class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiPropertyMI : virtual public CmpiBaseMI
{
public:
    CmpiPropertyMI(const CmpiBroker &mbp, const CmpiContext& ctx);

    static CMPIStatus driveSetProperty(
        CMPIPropertyMI* mi,
        const CMPIContext* eCtx,
        const CMPIResult* eRslt,
        const CMPIObjectPath* eOp,
        const char* name,
        CMPIData data);
    static CMPIStatus driveGetProperty(
        CMPIPropertyMI* mi,
        const CMPIContext* eCtx,
        const CMPIResult* eRslt,
        const CMPIObjectPath* eOp,
        const char* name);

    virtual CmpiStatus setProperty(
        const CmpiContext& ctx,
        CmpiResult& rslt,
        const CmpiObjectPath& op,
        const char* name,
        const CmpiData& data);
    virtual CmpiStatus getProperty(
        const CmpiContext& ctx,
        CmpiResult& rslt,
        const CmpiObjectPath& op,
        const char* name);
};

#endif

