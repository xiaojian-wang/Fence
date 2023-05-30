#ifndef ROUTERNODE_PS_H
#define ROUTERNODE_PS_H

#include "priceQueryMsg_m.h"
#include "priceUpdateMsg_m.h"
#include "routerNodeBase.h"

using namespace std;
using namespace omnetpp;

class routerOnlineFeeBase : public routerNodeBase {
    protected:
        virtual void initialize() override;
        // virtual routerMsg *generateTriggerPriceUpdateMessage();
        // virtual routerMsg *generatePriceUpdateMessage(double nLocal, double serviceRate, double arrivalRate, 
        //         double queueSize, int reciever);

        // modified message handlers
        virtual void handleMessage(cMessage *msg) override;
        // virtual void handleStatMessage(routerMsg *msg) override;

        virtual void handleTransactionMessageSpecialized(routerMsg *msg);
        // virtual void handleAckMessageSpecialized(routerMsg* ttmsg); //OnlineScheme: need to overide the ack function to update the lambda_1 FIXME: override or not?
        
        virtual void handleTransactionMessage(routerMsg *msg) override; 
        virtual void handleAckMessage(routerMsg *msg) override; //Note: is not override since the routerNodeBase.h does not have this function 

        // special message handlers for price scheme
        // virtual void handleTriggerPriceUpdateMessage(routerMsg *msg);
        // virtual void handlePriceUpdateMessage(routerMsg* ttmsg);
        // virtual void handlePriceQueryMessage(routerMsg* ttmsg);
};
#endif
