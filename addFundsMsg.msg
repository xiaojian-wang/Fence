//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Same as TictocMsg11
//
cplusplus {{
#include <map>
typedef std::map<int,double> FundsVector;
}};

//class noncobject FundsVector;
class FundsVector { @existingClass; }

packet addFundsMsg
{
    FundsVector pcsNeedingFunds;
}
