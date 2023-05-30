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
#include <vector>
typedef std::vector<double> DoubleVector;
}};

//class noncobject ConvolutionalFlow;
class DoubleVector { @existingClass; }

packet transactionMsg
{
    double amount;
    double timeSent;  //time after start time that job is active
    int sender;
    int receiver;
    int priorityClass;
    int transactionId; //is messageID of transactionMsg
    bool hasTimeOut;
    double timeOut;
    int htlcIndex;
    int pathIndex;
    bool isAttempted; 
    double largerTxnId;
    bool isMarked;
    double timeAttempted;
    double total_amount; // OnlineScheme:
    double tx_fee_amount;
    DoubleVector convolution_flow;
    double tx_fee_total;
}

