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
//cplusplus {{
//#include <vector>
//#include <map>
////typedef std::vector<int> IntVector_1;
////typedef std::vector<double> DoubleVector_1;
////typedef std::map<int, double> IntMap_1;
//}};

//class noncobject IntVector;
//class noncobject IntMap;
//class noncobject DoubleVector;
//class IntVector { @existingClass; }
//class IntMap_1 { @existingClass; }
//class DoubleVector_1 { @existingClass; }

packet transactionSendMsg
//if encapIsAck == false needs to look in encapsulated transactionMsg for priority class and amount
{
	int pathIndex;
	IntVector transactionPath;
	int receiver;

}