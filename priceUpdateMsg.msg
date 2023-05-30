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

packet priceUpdateMsg
{
    double nLocal;
    double serviceRate;
    double arrivalRate;
    double queueSize;
    double flow_amount; // OnlineScheme: need finially a new message class but now just a test here.
}

