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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CQLChainedIdentifier.h"
#include "CQLChainedIdentifierRep.h"
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>
#include <Pegasus/Query/QueryCommon/QueryIdentifier.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>

PEGASUS_NAMESPACE_BEGIN

CQLChainedIdentifierRep::CQLChainedIdentifierRep():
  QueryChainedIdentifierRep()
{
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(const String& inString):
  QueryChainedIdentifierRep()
{
    parse(inString);
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(const CQLIdentifier &id):
  QueryChainedIdentifierRep()
{
  _subIdentifiers.append(id);
}

CQLChainedIdentifierRep::CQLChainedIdentifierRep(
  const CQLChainedIdentifierRep* rep)
  :
  QueryChainedIdentifierRep()
{
    _subIdentifiers = rep->_subIdentifiers;
}

CQLChainedIdentifierRep::~CQLChainedIdentifierRep(){
    
}

Array<CQLIdentifier> CQLChainedIdentifierRep::getSubIdentifiers()const
{
  Array<CQLIdentifier> result;
  
  for (Uint32 i = 0; i < _subIdentifiers.size(); i++)
  {
    result.append(CQLIdentifier(_subIdentifiers[i]));
  }

    return result;
}

CQLIdentifier CQLChainedIdentifierRep::getLastIdentifier()const
{
    if(_subIdentifiers.size() > 0)
        return CQLIdentifier(_subIdentifiers[_subIdentifiers.size()-1]);
    return CQLIdentifier();
}

CQLIdentifier CQLChainedIdentifierRep::operator[](Uint32 index)const
{
    return CQLIdentifier(_subIdentifiers[index]);
}

CQLChainedIdentifierRep& CQLChainedIdentifierRep::operator=(
    const CQLChainedIdentifierRep& rhs)
{
    if(&rhs != this)
    {
        _subIdentifiers = rhs._subIdentifiers;
    }
    return *this;
}

void CQLChainedIdentifierRep::parse(const String & string)
{
    PEG_METHOD_ENTER(TRC_CQL, "CQLChainedIdentifierRep::parse");    
    /* 
      - parse string on "."
      - start from the end of string
      - if more than one substring found, 
        -- store first found string then
        -- prepend remaining substrings 
    */
    Char16 delim('.');
    Uint32 index;
    String range;

    index = string.reverseFind(delim);
    if(index == PEG_NOT_FOUND){
        _subIdentifiers.append(CQLIdentifier(string));
    }else{
        String tmp = string.subString(index+1);
        _subIdentifiers.append(CQLIdentifier(tmp));

        while(index != PEG_NOT_FOUND){
            tmp = string.subString(0,index);
            index = tmp.reverseFind(delim);
            if(index == PEG_NOT_FOUND){
                _subIdentifiers.prepend(CQLIdentifier(tmp));
            }
            else{
                _subIdentifiers.prepend(CQLIdentifier(tmp.subString(index+1)));
            }
        }
    }
    PEG_METHOD_EXIT();  
}

PEGASUS_NAMESPACE_END
