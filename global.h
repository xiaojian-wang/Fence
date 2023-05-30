#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <vector>
#include <queue>
#include <stack>
#include "routerMsg_m.h"
#include "transactionMsg_m.h"
#include "ackMsg_m.h"
#include "updateMsg_m.h"
#include "timeOutMsg_m.h"
#include "probeMsg_m.h"
#include "priceUpdateMsg_m.h"
#include "priceQueryMsg_m.h"
#include "transactionSendMsg_m.h"
#include "addFundsMsg_m.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>
#include <deque>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <list>
#include "structs/PaymentChannel.h"
#include "structs/PathInfo.h"
#include "structs/TransUnit.h"
#include "structs/Transaction.h" //FIXME: my transaction not to split  do not need...
#include "structs/CanceledTrans.h"
#include "structs/AckState.h"
#include "structs/DestInfo.h"
#include "structs/PathRateTuple.h"
#include "structs/ProbeInfo.h"
#include "structs/DestNodeStruct.h"

#define MSGSIZE 100
using namespace std;

struct LaterTransUnit
{
  bool operator()(const TransUnit& lhs, const TransUnit& rhs) const
  {
    return lhs.timeSent > rhs.timeSent;
  }
};


//global parameters
extern unordered_map<int, priority_queue<TransUnit, vector<TransUnit>, LaterTransUnit>> _transUnitList;
// extern unordered_map<int, deque<TransUnit>> _transUnitList;
extern unordered_map<int, set<int>> _destList;
extern unordered_map<int, unordered_map<int, vector<vector<int>>>> _pathsMap; //Question: what is the key? sender, receier pathlist
extern int _numNodes;
//number of nodes in network
extern int _numRouterNodes;
extern int _numHostNodes; // how many host nodes in the network
extern unordered_map<int, vector<pair<int,int>>> _channels; //adjacency list format of graph edges of network // for a channel , node1 <node2, weight(delay)>
//Note: we want to update the channel in a global manner first for the OnlineScheme 
extern unordered_map<int, vector<pair<int,double>>> _channel2;  // OnlineScheme:here the third weight is tmp_cost = csot1+cost2 
extern unordered_map<tuple<int,int>,double, hashId> _balances;
extern unordered_map<tuple<int,int>,double, hashId> _balances_protocol; // OnlineScheme: we need this balance view for update the view of the cost
extern unordered_map<tuple<int,int>, double, hashId> _capacities;
extern unordered_map<tuple<int,int>,int, hashId> _updateFrequencyEntryList; //Note: 
extern unordered_map<int,vector<double>> node_incomeList; //Note: this list is try to store the router's income list, key is the node, value is the double fee charged, 
extern unordered_map<int, vector<pair<double,double>>> tx_amount_feeList; //Note: 4.17 stat the tx fee and amount
extern unordered_map<int, int> corruptionStatusList; //Note: 4.16 corruption status record
extern double _statRate;
extern double _clearRate;
extern double _maxTravelTime; //Question: what is the max travel time?
extern double _maxOneHopDelay;
extern double _percentile;

//unordered_map of balances for each edge; key = <int,int> is <source, destination>
//extern bool withFailures;
extern bool _waterfillingEnabled;
extern bool _timeoutEnabled;
extern int _kValue; //for k shortest paths
extern double _simulationLength;
extern bool _landmarkRoutingEnabled;
extern bool _lndBaselineEnabled;
extern int _numAttemptsLNDBaseline;
//for lndbaseline
extern double _restorePeriod; 

extern bool _windowEnabled;

extern vector<tuple<int,int>> _landmarksWithConnectivityList;//pair: (number of edges, node number)
extern unordered_map<double, int> _transactionCompletionBySize;
extern unordered_map<double, int> _transactionArrivalBySize;
extern unordered_map<double, double> _txnAvgCompTimeBySize;
extern unordered_map<double, vector<double>> _txnTailCompTimesBySize;
extern ofstream _succRetriesFile, _failRetriesFile;

// for silentWhispers
extern vector<int> _landmarks;


// OnlineScheme: parameters for online fee routing
extern double _lambda1; // told the compiler there will be an variable named lambda_1
extern double _lambda2;
extern double _cost2; // unit cost, no flow
extern double _cost1; // unit cost 1
extern double _betae; // beta e
// extern double _b0efactor; // the initial point of every edge, for this host's every edge? Note: this variable not need since all the balance is inited from read the topology file
extern int _n; // max hop in HTLC
extern double _F1; // F_1 = 1
extern double _F2; // what the F2 should be? the maximum service time what is the maximum service time in spider?
extern double _mu1;
extern double _mu2;
extern double _alphae;
extern double _P; // n* maximum_time TODO: how the get the maximum time? read from the workload.txt 
extern bool _onlineSchemeEnabled; //TODO: need to init? where to inti global parameter?  and the name of this variable maybe some issue...?
extern bool _feeFlag;
extern double _HTLCtime; // input parameter, the maximum time 
extern double _mince; // Note: min ce , used by assumption 2
extern double _Z; 
extern int _updateFrequency;
extern double _corruptionRatio;
extern double _corruptionFeeRatio;

extern set<int> transactionIdSet; // Note: FIXMEd: what type of the transaction id <txid:int,success/fail>
extern set<int> failed_transactionIdSet;  // record all the tx that failed
extern int fail_1_count;
extern int fail_2_count;
extern int fail_3_count;
extern int fail_4_count;
extern int fail_5_count;
extern int fail_6_count;
extern int fail_7_count;
extern int fail_8_count;
extern int violate_balance_count;
extern int violate_capacity_count;
extern int mismatch_count;
extern int mismatch_atSender_count;
extern double total_amount_reject;
extern double total_amount_try;
extern int admission_criteria_violation_count;
extern int balance_negative_count;
// extern vector<double> time_vary_success_ratio;
// extern vector<double> time_vary_throughput;
extern vector<double> failed_tx_timeStamp;

extern int cost_traverse_go_through_edges_count; // Note: to stat the average cost of each edge when next tx send out. only stat the ack message updated cost
extern double cost1_total;
extern double cost2_total;

//Note: to record fail7 realted tx amount and balance
extern vector<double> fail7_totalAmount;
extern vector<double> fail7_currentBalance; 

//Note: to record the admission crteria violation and balance violation
extern set<int> addimssion_violation_txIdSet;
extern set<int> balance_violation_txIdSet;



extern bool _alphaeCheckFlag;
extern bool _sendFeeFlag;
extern bool _sendValuationFlag;
extern bool _capacityScaleEnable;


//parameters for price scheme
extern bool _priceSchemeEnabled;
extern bool _splittingEnabled;

extern double _rho; // for nesterov computation
extern double _rhoLambda; // and accelerated Gradients
extern double _rhoMu;

extern double _eta; //for price computation
extern double _kappa; //for price computation
extern double _capacityFactor; //for price computation
extern bool _useQueueEquation;
extern double _tUpdate; //for triggering price updates at routers
extern double _tQuery; //for triggering price query probes
extern double _alpha; //parameter for rate updates
extern double _minPriceRate; // minimum rate to assign to all nodes when computing projections and such
extern double _delta; // round trip propagation delay
extern double _avgDelay;
extern double _xi; // how fast you want to drain the queue relative to network rtt - want this to be less than 1
extern double _routerQueueDrainTime;
extern int _serviceArrivalWindow;
extern double _minWindow;
extern bool _reschedulingEnabled;
extern double _smallRate;
extern double _tokenBucketCapacity;
extern double _zeta;

// overall knobs
extern bool _signalsEnabled;
extern bool _loggingEnabled;
extern double _ewmaFactor;

// path choices knob
extern bool _widestPathsEnabled;
extern bool _heuristicPathsEnabled;
extern bool _kspYenEnabled;
extern bool _obliviousRoutingEnabled;

// queue knobs
extern bool _hasQueueCapacity;
extern int _queueCapacity;
extern double _epsilon;

// speeding up price scheme
extern bool _nesterov;
extern bool _secondOrderOptimization;

// experiments related parameters to control statistics
extern double _transStatStart;
extern double _transStatEnd;
extern double _waterfillingStartTime;
extern double _landmarkRoutingStartTime;
extern double _shortestPathStartTime;
extern double _shortestPathEndTime;
extern double _splitSize;

// rebalancing related parameters
extern bool _rebalancingEnabled;
extern double _rebalancingUpFactor;
extern double _queueDelayThreshold;
extern double _gamma;
extern double _maxGammaImbalanceQueueSize;
extern double _delayForAddingFunds;
extern double _rebalanceRate;
extern double _computeBalanceRate;
extern double _bank; // global accounter of all money in the system

// DCTCP params
extern double _windowAlpha;
extern double _windowBeta;
extern double _qEcnThreshold;
extern double _qDelayEcnThreshold;
extern bool _qDelayVersion;
extern bool _tcpVersion;
extern double _balEcnThreshold;
extern bool _dctcpEnabled;
extern double _cannonicalRTT; // for prop fair price scheme version - specific to a topology
extern double _totalPaths; // total number of paths over which cannonical RTT was computed
extern bool _isCubic;
extern double _cubicScalingConstant;

// knobs to enable the changing of paths
extern bool _changingPathsEnabled;
extern int _maxPathsToConsider;
extern double _windowThresholdForChange; 

// celer network params
extern bool _celerEnabled;
extern int _maxImbalanceWindow;
//global debt queues 
extern unordered_map<int, unordered_map<int, double>> _nodeToDebtQueue; // = {};
// (key1, (key2, value)) where key1 is nodeA (whose debt queue), and key2 is nodeB (dest node),
 // and value is amt needs to be transferred from nodeA to nodeB
extern double _celerBeta;  // denotes how much to weigh the imbalance in the per node weight

extern string workloadFile_; //Note:
extern string topologyFile_;
extern string _serverPath; // Note: store the results in server...

extern double _eps; 
extern double _timeslot;

extern int protocol_wrong;

