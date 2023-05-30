#ifndef ROUTERNODE_PS_H
#define ROUTERNODE_PS_H

#include "priceQueryMsg_m.h"
#include "priceUpdateMsg_m.h"
#include "hostNodeBase.h"

using namespace std;
using namespace omnetpp;

class hostOnlineFeeBase : public hostNodeBase {

    private:
        // price scheme specific signals
        vector<simsignal_t> probabilityPerDestSignals = {};
        map<int, simsignal_t> numWaitingPerDestSignals = {};
        vector<simsignal_t> numTimedOutAtSenderSignals = {};
        vector<simsignal_t> pathPerTransPerDestSignals = {};
        map<int, simsignal_t> demandEstimatePerDestSignals = {};


    protected:
        // message generators
        // virtual routerMsg *generateTriggerPriceUpdateMessage();
        // virtual routerMsg *generatePriceUpdateMessage(double nLocal, double serviceRate, double arrivalRate, 
        //         double queueSize, int reciever);
        // virtual routerMsg *generateTriggerPriceQueryMessage();
        // virtual routerMsg *generatePriceQueryMessage(vector<int> route, int routeIndex);
        // virtual routerMsg *generateTriggerTransactionSendMessage(vector<int> route, 
        //         int routeIndex, int destNode);

        // helpers
        // functions to compute projections while ensure rates are feasible
        // virtual bool ratesFeasible(vector<PathRateTuple> actualRates, double demand);
        // virtual vector<PathRateTuple> computeProjection(vector<PathRateTuple> recommendedRates, 
        //         double demand);

        // modified message handlers
        virtual void handleMessage(cMessage *msg) override;
        // virtual void handleTimeOutMessage(routerMsg *msg) override;
        virtual void handleTransactionMessageSpecialized(routerMsg *msg) override;
        // virtual void handleStatMessage(routerMsg *msg) override;
        virtual void handleAckMessageSpecialized(routerMsg* ttmsg) override;
        // virtual void handleClearStateMessage(routerMsg *msg) override;

        // special messages for priceScheme
        // virtual void handleTriggerPriceUpdateMessage(routerMsg *msg);   // price update
        // virtual void handlePriceUpdateMessage(routerMsg* ttmsg);
        // virtual void handleTriggerPriceQueryMessage(routerMsg *msg);  // price query
        // virtual void handlePriceQueryMessage(routerMsg* ttmsg);
        // virtual void handleTriggerTransactionSendMessage(routerMsg* ttmsg);  // transaction send


        /**** CORE LOGIC ******/
        // initialize price probes for a given set of paths to a destination and store the paths
        // for that destination
        // virtual void initializePriceProbes(vector<vector<int>> kShortestPaths, int destNode);
        // updates timers once rates have been updated on a certain path    
        // virtual void updateTimers(int destNode, int pathIndex, double newRate); // do not need since our rate is not controled, we assume the rate is as it arrive possion distribution
        virtual void initialize() override;
        virtual bool checkCost(double path_total_cost, double valuation);
        virtual double getCost(vector<vector<int>> kShortestPaths);
        virtual double Assumption2_demand_threshold(double min_ce, double mu_1, double mu_2, double alpha_e, double beta_e);
        
        //ConvolutionalFee:
        virtual vector <double> getEachEdgeCost(vector<vector<int>> kShortestPaths);
        virtual vector <double> ConvolutionalFlow(double dest_amount,vector <double> eachEdgeCost);
        // virtual vector <double> getTotalPathCost(double,vector <double>);
        virtual double getPathCost(vector <double> eachEdgeCost, vector <double> convolution_flow);
        
};
#endif


