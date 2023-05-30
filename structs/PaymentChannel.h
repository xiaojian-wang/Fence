
using namespace std;
using namespace omnetpp;

typedef tuple<int,int> Id; // first is transaction id, second is htlc index

struct hashId {
    size_t operator()(const tuple<int, int> &tid ) const
    {
        return hash<int>()(get<0>(tid)) ^ hash<int>()(get<1>(tid));
    }
};

// the property of a payment channel
class PaymentChannel{
public:
    //channel information
    cGate* gate;
    double balance;
    double minAvailBalance = 100000;
    double balanceEWMA;
    double entitledFunds; //FIXME: which one is total capacity of this channel?
    double owedFunds;
    vector<tuple<int, double, routerMsg*,  Id, simtime_t >> queuedTransUnits; //make_heap in initialization
    
    double queueSizeSum = 0;
    double totalAmtInQueue = 0; // total amount in the queue
    unordered_map<Id, double, hashId> incomingTransUnits; //(key,value) := ((transactionId, htlcIndex), amount)
    double totalAmtIncomingInflight;
    double totalAmtOutgoingInflight;
    unordered_map<Id, double, hashId> outgoingTransUnits;

    double queueDelayEWMA;
    int numRebalanceEvents = 0;
    double amtAdded = 0.0;
    double amtExplicitlyRebalanced = 0.0;
    double amtImplicitlyRebalanced = 0.0;
    double fakeRebalancingQueue = 0.0;

    //channel information for price scheme
    double nValue;// Total amount across all transactions (i.e. the sum of the amount in each transaction) Note: transaction amount is nValue
        // that have arrived to be sent along the channel
    double lastNValue;
    double lastQueueSize;
    double xLocal; //Transaction arrival rate ($x_local$) xLocal is the tx arrive rate
    double origTotalCapacity; //Total channel capacity ($C$) Note: total capacity of a channel
    double serviceRate; // ratio of rate transaction arrival over transaction service rate (queue->inflight)   
    double arrivalRate;   
    double lambda; //Price due to load ($\lambda$)
    int numUpdateMessages; // in the last _Tquery interval
    double updateRate; // rate of sending update messages
    int updateFrequencyEntry;
    
    double sumArrivalWindowTxns = 0;
    double sumServiceWindowTxns = 0;
    list<tuple<double, simtime_t, simtime_t>> serviceArrivalTimeStamps; 
    //each entry is amount and service and arrival time of
    
    // to keep track of how long every transaction spent in flight
    unordered_map<Id, simtime_t, hashId> txnSentTimes;
    double sumTimeInFlight = 0;
    double timeInFlightSamples = 0;
    simsignal_t timeInFlightPerChannelSignal;
     
    // last n serviced txns
    list<tuple<double, simtime_t>> arrivalTimeStamps; 
    //each entry arrival time of last n transactions that arrived and their sizes (may have been dropped)

    double lastLambdaGrad = 0; // for accelerated gradient descent
    double yLambda; // Nesterov's equation y

    // TODO: define some parameters that for online fee method
    // TODO: the parameters here is for dynamic update variables
    double lambda_1;
    double lambda_2;
    double cost_1;
    double cost_2;

    // Note: to store the capacity violation related information, 
    // the time of tx arrive at this channel, tx_id, the flow amount
    // first check if arrive time is in 30ms range, then get the tx id and corresponding flow amount
    // unordered_map<simtime_t, unordered_map<int, double>> _arriveTime_txId_flow;
    // should erase(simtime_t) when simtime_t < simTine() - timeslot (30ms in our setting)
    //double delete_tx_before_this_time = simTime().dbl() - _timeslot;  // get some key
    // _arriveTime_txId_flow.erase(key)
    // it seems that we only need to record the txs that not yet be acked in this channel
    unordered_map<int, double> not_yet_acked_tx; // <txId, flow_amount>
    unordered_map<int, double> not_yet_acked_tx_protocol; // OnlineScheme: protocol flow


    // double total_cost;
    // TODO: statistic signal, emit to neighbor
    simsignal_t Lambda1Signal;
    simsignal_t Lambda2Signal;
    simsignal_t Cost1Signal;
    simsignal_t Cost2Signal;





    double muLocal; //Price due to channel imbalance at my end  ($\mu_{local}$)
    double lastMuLocalGrad = 0; // for accelerated gradient descent
    double yMuLocal; // Nesterov's equation y

    double muRemote; //Price due to imbalance at the other end of the channel ($\mu_{remote}$)
    double yMuRemote; // Nesterov's equation y

    //statistics for price scheme per payment channel
    simsignal_t nValueSignal;
    simsignal_t xLocalSignal;
    simsignal_t serviceRateSignal;
    simsignal_t arrivalRateSignal;
    simsignal_t inflightOutgoingSignal;
    simsignal_t inflightIncomingSignal;
    simsignal_t lambdaSignal;
    simsignal_t muLocalSignal;
    simsignal_t muRemoteSignal;
    simsignal_t fakeRebalanceQSignal;

    //statistics - ones for per payment channel
    int statNumProcessed;
    double deltaAmtSent = 0.0; //used for celer network, reset with clear state message
    double deltaAmtReceived = 0.0; //used for celer network, reset with clear state message
    double totalAmtSent = 0.0;
    double totalAmtReceived = 0.0;
    vector<double> channelImbalance = {}; //sliding window
    simsignal_t amtInQueuePerChannelSignal;
    simsignal_t balancePerChannelSignal;
    simsignal_t capacityPerChannelSignal;
    simsignal_t numInflightPerChannelSignal;
    simsignal_t queueDelayEWMASignal;
    simsignal_t bankSignal;
    simsignal_t numSentPerChannelSignal;
    simsignal_t explicitRebalancingAmtPerChannelSignal;
    simsignal_t implicitRebalancingAmtPerChannelSignal;

    //statistics for celer_network
    map<int, simsignal_t> destToCPISignal = {};
    map<int, double>  destToCPIValue = {};
    simsignal_t kStarSignal;


};
