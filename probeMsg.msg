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
import routerMsg;
import transactionMsg;

cplusplus {{
//#include <vector>
//#include <map>
////typedef std::vector<int> IntVector;
//typedef std::vector<double> DoubleVector;
////typedef std::map<int, double> IntMap_2;
//}};

//class noncobject IntVector;
//class noncobject IntMap;
//class noncobject DoubleVector;

//class IntVector_2 { @existingClass; }
//class IntMap_2 { @existingClass; }
//class DoubleVector { @existingClass; }

packet probeMsg
//if encapIsAck == false needs to look in encapsulated transactionMsg for priority class and amount
{
    int pathIndex;
    int sender;
    int receiver;
    bool isReversed = false;
    DoubleVector pathBalances;
    IntVector path;
    int transactionId; //used only for landmark routing
}