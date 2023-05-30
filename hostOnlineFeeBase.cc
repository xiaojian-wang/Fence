#include "hostOnlineFeeBase.h"
#include <iomanip>

// Note: global parameters for their previous priceScheme we do not need it
double _tokenBucketCapacity = 1.0;
bool _reschedulingEnabled; // whether timers can be rescheduled
bool _nesterov;
bool _secondOrderOptimization;
bool _useQueueEquation;
double _eta; //for price computation
double _kappa; //for price computation
double _capacityFactor; //for price computation TODO: why need this capacity factor
double _tUpdate; //for triggering price updates at routers
double _tQuery; //for triggering price query probes at hosts 
double _alpha;
double _zeta;
double _delta;
double _avgDelay;
double _minPriceRate;
double _rhoLambda;
double _rhoMu;
double _rho;
double _minWindow;
double _xi; //beta = 2_xi
double _routerQueueDrainTime;
double _smallRate = pow(10, -6); // ensuring that timers are no more than 1000 seconds away
// ---


unordered_map<int, unordered_map<int, double>> _hostnode_txid_flow;  //OnlineScheme: node, txid, flow; only one direction so do not need a vector

 
// OnlineScheme: parameters for online fee routing 
// Note: the parameter here is constant for online scheme
double _betae; // beta e
int _n; // max hop in HTLC
double _F1; // F_1 = 1
double _F2; // TODO: the F2 can be set different from F1
double _mu1;
double _mu2;
double _alphae;
double _P; //Note: C in the paper
double _HTLCtime;
double _mince;
bool _alphaeCheckFlag;
double _Z;
bool _sendFeeFlag;
bool _sendValuationFlag;
bool _capacityScaleEnable;
int _updateFrequency;
double _corruptionRatio;
double _corruptionFeeRatio;


Define_Module(hostOnlineFeeBase);


/* overall controller for handling messages that dispatches the right function
 * based on message type in price Scheme
 */
// TODO: change the message type unless we'd like to use LSA
void hostOnlineFeeBase::handleMessage(cMessage *msg) {
    // cout << "into handleMessage in the hostOnlineFeeBase.cc" << endl;
    routerMsg *ttmsg = check_and_cast<routerMsg *>(msg);
    // cout << "in handleMessage in the hostOnlineFeeBase.cc: check_and_cast error? no" << endl;
    

    // if (simTime() > _simulationLength){
    //     auto encapMsg = (ttmsg->getEncapsulatedPacket());
    //     ttmsg->decapsulate();
    //     delete ttmsg;
    //     delete encapMsg;
    //     return;
    // } 

    hostNodeBase::handleMessage(msg);
    
    // switch(ttmsg->getMessageType()) {
    //     case TRIGGER_TRANSACTION_SEND_MSG:
    //          if (_loggingEnabled) cout<< "[HOST "<< myIndex() 
    //              <<": RECEIVED TRIGGER_TXN_SEND MSG] "<< ttmsg->getName() << endl;
    //          handleTriggerTransactionSendMessage(ttmsg);
    //          if (_loggingEnabled) cout<< "[AFTER HANDLING:]  "<< endl;
    //          break;

    //     default:
    //          hostNodeBase::handleMessage(msg);

    // }
}



//TODO: change the description here
/* if sender is myself, check the tx did not see before
1. count for violation of assumption 2 and 1
2. find a shortest path (FAIL2)
3. calculate the convolutional flow 
4. check the path cost and valuation, if valuation greater than cost, set the flow and fee (do not forward it here, later foward), else FAIL4
*/

/* if myself is receiver, check if the arrive amount is greater than dest amount, 
    1. if not, generate failedACK
    2. if yes, count the mismatch, handleTransactionMessage
*/
/* if myself is sender, forward this tx out
*/
// OnlineScheme: just accept or reject change here......
void hostOnlineFeeBase::handleTransactionMessageSpecialized(routerMsg* ttmsg){
    // cout << "into hostOnlineFeeBase handleTransactionMessageSpecialized " << endl;
    // cout << "hop count is in host ? " << ttmsg->getHopCount() << endl;
    // cout << "xxx just in handleTransactionMessageSpecialized - route size is : " << ttmsg->getRoute().size() << endl;
    
    transactionMsg *transMsg = check_and_cast<transactionMsg *>(ttmsg->getEncapsulatedPacket()); // Note: check if this routerMsg indeed a transactionMsg
    
    int hopcount = ttmsg->getHopCount(); // Note: indeed from routerMsg record this 
    int destNode = transMsg->getReceiver();
    int transactionId = transMsg->getLargerTxnId();
    double tx_fee = transMsg->getTx_fee_amount();
    double total_amount = transMsg->getTotal_amount(); // Note: the arrived amount
    double dest_amount = transMsg->getAmount();

    char balancefileName[1000];
    string workloadFile = par("workloadFile");
    string workload = workloadFile;
    string delimiter = "/";  // separator
    string sub_str;
    size_t pos = 0;
    // use loop to extract each substring
    while ((pos = workload.find(delimiter)) != std::string::npos) {
        sub_str = workload.substr(pos + 1);  // +1 to skip the delimiter
        workload.erase(0, pos + delimiter.length());
    }
    workloadFile = sub_str;

    string topoFile = par("topologyFile");
    #ifdef __APPLE__
    sprintf(balancefileName, "%s_%s_%s_%f_%d_%f_%f_%f_%f_%f_%d_%d_%d_%d_%f_%f", "./results/", "balance", workloadFile.c_str(), _simulationLength, _n, _HTLCtime, _F1, _F2, _P, _Z*10000000, _sendFeeFlag,  _sendValuationFlag, _capacityScaleEnable,_updateFrequency, _corruptionRatio, _corruptionFeeRatio);
    #elif __linux__
    // sprintf(balancefileName, "%s_%s_%s_%f_%d_%f_%f_%f_%f_%f_%d_%d_%d", _serverPath.c_str(), "balance", workloadFile.c_str(), _simulationLength, _n, _HTLCtime, _F1, _F2, _P, _Z, _sendFeeFlag,  _sendValuationFlag, _capacityScaleEnable);
    // "/mnt/data/xwang244/Omnetpp_results/workload11Iterate"
    sprintf(balancefileName, "%s_%s_%s_%f_%d_%f_%f_%f_%f_%f_%d_%d_%d_%d_%f_%f", "/mnt/data/xwang244/Omnetpp_results/TONupdateFrequency/", "balance", workloadFile.c_str(), _simulationLength, _n, _HTLCtime, _F1, _F2, _P, _Z*10000000, _sendFeeFlag,  _sendValuationFlag, _capacityScaleEnable,_updateFrequency, _corruptionRatio, _corruptionFeeRatio);
    #endif
    // 6.30 if the tx id can be % by 5000 ==1
    if (((transactionId) % 5000 == 0 || transactionId == 2000000 || transactionId==1) && (transactionIdSet.count(transactionId) == 0)) { //1,5001,10001, ..., 2000000
        // cout << "------------------------------record the balance for one time ! At transaction: " << transactionId << endl;
        statNetworkChannelBalance(topoFile, balancefileName);
    }
    


    if (_loggingEnabled) {
        cout << "try to handle a transaction message from " << transMsg->getSender() << " to " <<  destNode << endl;
        // cout << "next hop is :" << nextNode << endl;
        cout << "transaction Id read from message is :" <<  transactionId << endl;
        cout << "init (total, dest) : " <<  total_amount << ", " << dest_amount << endl;
        cout << "init tx_fee is " << tx_fee << endl;
    }

    //isSelfMessage to see if message is send out by ScheduleAt()
    // cout << "xxx before check if it is sender - route size is : " << ttmsg->getRoute().size() << endl;
    
    /* if sender is myself, check the tx did not see before
    1. count for violation of assumption 2 and 1
    2. find a shortest path (FAIL2)
    3. calculate the convolutional flow 
    4. check the path cost and valuation, if valuation greater than cost, set the flow and fee (do not forward it here, later foward), else FAIL4
    */
    if (transMsg->getSender() == myIndex() && transactionIdSet.count(transactionId) == 0 ){ // OnlineScheme: if this transaction not appear before, in our case, one tx cannot appear twice
        if (_loggingEnabled) {
            cout << "transaction ID: " <<  transactionId << " not in transaction ID set, add to set!" << endl;
        }

        transactionIdSet.insert(transactionId);
        total_amount_try = total_amount_try + transMsg->getAmount(); 
        // cout << std::setprecision (20) << "transactionId: " << transactionId << " , amount: " << transMsg->getAmount() << endl;
        if (transactionId % 10000 == 0){
            if (_loggingEnabled) cout << std::setprecision (20) << "until tx : " << transactionId << " , the total amount is : " << total_amount_try << endl;
        }
        
        //OnlineScheme: first to check if the demand assumption is meet for a specific tx
        // Note: here is for assumption 2, assumption 1 is meeted by setting the parameters in .ini file
        double demand_threshold = Assumption2_demand_threshold(_mince,_mu1,_mu2,_alphae,_betae); //TODO: the demand threhold is a function
        
        if (_loggingEnabled) {
            cout << ":) the demand threshold of Assumption 2 is : " << demand_threshold << endl;
            cout << ":) transMsg->getAmount() " << transMsg->getAmount()  << endl;
        }


        //Note: we only need to record the number of assumption 2 violation, but do not to reject any tx that not meet this assumption
        if (transMsg->getAmount() > demand_threshold){
            if (_loggingEnabled) { //level_0
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                cout << "FAIL1: assumption 2 is not meet" << endl;
                cout << ":) the demand threshold of Assumption 2 is : " << demand_threshold << endl;
                cout << ":) transMsg->getAmount() " << transMsg->getAmount()  << endl;
            }
            fail_1_count++;
        }


        //Note: record the number of assumption 1 not meet, for a certain ini config file(same workload), the number is a constant
        if ((_Z * _n * _HTLCtime > _P) || (_Z * _n * _HTLCtime * _F1 + _Z * _n * _F2 < _P) ){
            if (_loggingEnabled && transactionId == 1){ //Note: we do not want to output this every time a tx arrive, since for a same setting, it will always be the same
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                cout << "FAIL8: Assumption 1 not meet!" << endl;
                cout << "(_P, _Z * _n * _HTLCtime, _Z * _n * _HTLCtime * _F1 + _Z * _n * _F2):  " << "(" << _P << " , " << _Z * _n * _HTLCtime << " , " << _Z * _n * _HTLCtime * _F1 + _Z * _n * _F2 << endl;
            }
            fail_8_count++;
        }


        set<int>::iterator it;
        if (_loggingEnabled) {
            cout << "transaction ID set is :" << endl;
            for(it = transactionIdSet.begin(); it != transactionIdSet.end(); it++) 
                cout << (*it) << " ";
        }

        if (_loggingEnabled) {
            cout << '\n' << "failed transaction ID are : " << endl;
            for(it = failed_transactionIdSet.begin(); it != failed_transactionIdSet.end(); it++) 
                cout << (*it) << " ";    
            cout << '\n' << endl;
        }
            
        //Note: find the shortest path based on network's weight - which is unit cost of 
        // cout << " \n start find the shortest path" << endl;
        vector<vector<int>> kShortestRoutes = getKPaths2(transMsg->getSender(), destNode, _kValue); // OnlineScheme: use new weightfind shortest path, kValue should be 1
        
        if (_loggingEnabled) {
            cout << "_kValue: " << _kValue << endl; 
            cout << "set the route for this transaction message: " << endl;
        }


        if (kShortestRoutes.size() > 0){ //Note: if can find at least one shortest path
            ttmsg->setRoute(kShortestRoutes[0]); //Note: Note: Note: here is the place update the path in the message
        }
        else{ //Note: if cannot find a path, insert into the failed_transactionIdSet, not to generate a failedACK, do not need 
            //Note:in our connected network setting, expect no FAIL2
            if (_loggingEnabled) { //level_0
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                cout << "FAIL2: Cannot find even one path for this transaction from " <<transMsg->getSender() << "   to    " <<destNode << "   and insert this tx into failed_transactionIdSet!" << endl;
            }
            fail_2_count++;
            total_amount_reject = total_amount_reject + transMsg->getAmount();
            // Note: to keep every thing follow the same line, generate a failed ACK
            routerMsg * failedAckMsg = generateAckMessage(ttmsg, false); //Note: generate a 
            if (_loggingEnabled) {
            cout << "after generate a failed ack message!" << endl;
            }
            handleAckMessageSpecialized(failedAckMsg); 
            if (_loggingEnabled) {
            cout << "after handle a ack message specilialized" << endl;
            }
            // failed_transactionIdSet.insert(transactionId); //Note: seems not use at all
            // ttmsg->decapsulate();
            // delete ttmsg;
            return; // return is needed, the process cannot continue since no path find
        }


        // ConvolutionalFee: try to get the cost of each edge
        vector <double> eachEdgeCost = getEachEdgeCost(kShortestRoutes);
        double tmp_pathCost = 0; //Note: for add the cost of each edge together
        
        // For log only
        if (_loggingEnabled)
            cout << "xxxxxxxxxx EACH EDGE UNIT COST start xxxxxxxxxxx" << endl;
        for (int i =0; i<eachEdgeCost.size(); i++){
            tmp_pathCost = tmp_pathCost + eachEdgeCost[i];
            if (_loggingEnabled) cout << eachEdgeCost[i] << ' ';
        }
        if (_loggingEnabled){
        cout << " " << endl;
        cout << "xxxxxxxxxx EACH EDGE UNIT COST end xxxxxxxxxxx" << endl;
        }


        //Note: then we can get the unit cost of this path
        double pathCost = tmp_pathCost;  // Note: this story tell us to return more results for stat...
        double real_path_cost = pathCost * dest_amount; // Equation (10), real path cost is the unit_cost * demand

        //ConvolutionalFee: this flow storage the flow that sender expect in that hop, but maybe not the real flow that a router decide to send out
        vector <double> convolution_flow = ConvolutionalFlow(dest_amount,eachEdgeCost); 
        if (_loggingEnabled){
            cout << "Convolutional flow: " << endl;
            for (int i =0; i<convolution_flow.size(); i++){
                cout << convolution_flow[i] << ' ';
            }
            cout << "   & demand: " << dest_amount << endl;
            cout << "convolution_flow[0]   " << convolution_flow[0] << endl; //already reverse it 
        }   


        //Note: the send out flow is total_amount if sendFeeFlag is on
        //Note: the send out flow should be dest+valuation if _sendValuationFlag is on
        double real_valuation = _P * dest_amount; // line 4 in Alg of the paper. assume _P is C for now... _P practically should <=0.1 [C absorb the _Z]
        double total_amount;
        if (_sendFeeFlag){
            total_amount = convolution_flow[0]; // Note: already reversed, it's in the real sequence
        }else if (_sendValuationFlag){
            total_amount = dest_amount + real_valuation;
        }else{
            total_amount = convolution_flow[0]; // Note: default send feeFlag on 
        }

        if (total_amount > dest_amount * (1 + _P)){
            admission_criteria_violation_count++;
            addimssion_violation_txIdSet.insert(transactionId);
            // if we reject these txs that not meet the admission criteria
            // if (_capacityScaleEnable){
            //     //generate an negative ack
            //     routerMsg * failedAckMsg = generateAckMessage(ttmsg, false); //Note: generate a 
            //     if (_loggingEnabled) {
            //     cout << "after generate a failed ack message!" << endl;
            //     }
            //     handleAckMessageSpecialized(failedAckMsg); 
            //     if (_loggingEnabled) {
            //     cout << "after handle a ack message specilialized" << endl;
            //     }
            //     return;
            // }

        }
        

        //given the total_amount should be send out, the total_fee of this tx 
        double total_fee = total_amount - dest_amount;
        if (_loggingEnabled) {
            cout << "total_amount and dest_amount are: ( " << total_amount << ", " << dest_amount << " )"<< endl;
            cout << ">-< Valuation , Cost >-< : " << real_valuation << ", " <<  real_path_cost << endl;
            cout << ">=< total fee >=< : " << total_fee << endl; //Note: the cost should equal to total_fee, else you calculate a wrong total_flow Note: No, the cost should not equal to total_fee, since the cost calculate from demand, while the total_fee from the real convolutional flow, these two should not be the same
        }
        

        bool flag = checkCost(real_path_cost, real_valuation); // OnlineScheme: to check if the cost is over the valuation
        if (flag){ //if the path cost is less than valuation, this tx can be send out, if cannot pass this check, return
            transMsg->setTx_fee_amount(total_fee); // Note: set the total tx fee of this tx
            transMsg->setTx_fee_total(total_fee); // Note: 4.17 stat the tx fee part
            // cout << 'tx fee is in the first place: ' << total_fee << endl;
            transMsg->setConvolution_flow(convolution_flow); //ConvolutionalFee: to check if the flow in each hop is same as the init (Mismatch flow)
            transMsg->setTotal_amount(total_amount);
            _hostnode_txid_flow[myIndex()][transactionId] = total_amount; //OnlineScheme: record the total amount send out, for the ack message to unlock
            
            if (_loggingEnabled) {
                cout << "decide send out a transaction from " << myIndex() << endl;
                cout << "Accpet transaction:" << transactionId << endl;
                cout << "-------------------------" << endl;
            }
                
            // // FIXME: I don't think we need to update the data structure...
            // nodeToShortestPathsMap[destNode] = {}; //destination, pathindex, pathinfo
            // for (int pathIdx = 0; pathIdx < kShortestRoutes.size(); pathIdx++){ // one path 
            //     PathInfo temp = {};
            //     nodeToShortestPathsMap[destNode][pathIdx] = temp;
            //     nodeToShortestPathsMap[destNode][pathIdx].path = kShortestRoutes[pathIdx];
            // }
        }
        else{ // if the path cost is less than valuation, FAIL4
            if (_loggingEnabled) { //level_0
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                cout << "total_amount and dest_amount are: ( " << total_amount << ", " << dest_amount << " )"<< endl;
                cout << ">-< Valuation , Cost >-< : " << real_valuation << ", " <<  real_path_cost << endl;
                cout << ">=< total fee >=< : " << total_fee << endl; //Note: the real_path_cost <= total_fee, since the cost calculate from demand, while the total_fee from the real convolutional flow, these two should not be the same
                cout << "FAIL4: flag is false!!! Reject this transaction: " << transactionId << endl;
                cout << "-------------------------" << endl;
            }

            fail_4_count++;
            
            //generate an negative ack
            routerMsg * failedAckMsg = generateAckMessage(ttmsg, false); //Note: generate a 
            if (_loggingEnabled) {
            cout << "after generate a failed ack message!" << endl;
            }
            handleAckMessageSpecialized(failedAckMsg); 
            if (_loggingEnabled) {
            cout << "after handle a ack message specilialized" << endl;
            }
            return;
        }
    }

    //destination do noting, only sender accept or reject
    // Note: at destination, trigger ack
    /* if myself is receiver, check if the arrive amount is greater than dest amount, 
        1. if not, generate failedACK
        2. if yes, count the mismatch, handleTransactionMessage
    */
    int prevNode = ttmsg->getRoute()[ttmsg->getHopCount()-1];
    int vector_size = _channel2[myIndex()].size(); // how many channels this node have 
    if (transMsg->getReceiver() == myIndex()) { // if myself is receiver
        if (_loggingEnabled) {
            cout << "myself is destination: " << myIndex() << endl;
        }

        //OnlineScheme:
        // nodeToPaymentChannel[prevNode].nValue += transMsg->getAmount(); 

        if (_loggingEnabled){
            cout << "(total, dest) : " <<  total_amount << ", " << dest_amount << endl;
            cout << "next Node is " << prevNode << endl;
            cout << "tx_fee is " << tx_fee << endl;
        }


        //Note: if dest_amount <= total_amount, we should accept it, not only the case equal 
        // if (abs(dest_amount-total_amount) <= eps){ //Note: this line only for equal
        //Note: _sendFeeFlag & _sendValuationFlag have same behavious when they arrive receiver
        if ((dest_amount <= total_amount) || abs(dest_amount-total_amount) <= _eps){
            handleTransactionMessage(ttmsg, false); // Note: if destination is myself, false: not revisit
            //Note: stat the mismatch at receiver end
            if (!(abs(dest_amount-total_amount) <= _eps))
                mismatch_atSender_count++;
        }
        else{ // at receiver: total_amount not equal to dest_amount, failed 
            //Note: do not to update the previous channel lambda1 and lambda2 and cost because this host is connected to its own router.
            /*6.5 Note: since the destination is a host, and the previous hop is its router, the router should not care about the 
            cost of this edge, so the utilization do not need to update here
             */
            if (_loggingEnabled) { //level_0
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                cout << "FAIL5: total_amount(arrived) not enough as dest_amount(receiver)! : (total, dest) : " <<  total_amount << ", " << dest_amount << endl;
            }
            fail_5_count++;
            
            //Note: generate failed ack message send to previous router
            routerMsg * failedAckMsg = generateAckMessage(ttmsg, false);
            forwardMessage(failedAckMsg);
            // cout << "After forward the negative ACK message " << endl;
            return;
        }
    }
    /* if myself is sender, forward this tx out
    */
    else if (ttmsg->isSelfMessage()) { // if myself is the sender 
        if (_loggingEnabled) {
            cout << "myself is sender: " << myIndex() << endl;
        }
        forwardMessage(ttmsg); //Note: forward this message to next hop here? 
        if (_loggingEnabled) {
            cout << "forward this message: " << ttmsg << "to next hop!" << endl;
        }
        return; // OnlineScheme: Question: Need to send this message to next hop, where is it? 

    }
}



// TODO: change the description
/* if the ack is failed, count in FAIL3, and delete it*/
// OnlineScheme: every time a host receive the ack, lamda and cost do not update since they connect to their own router
void hostOnlineFeeBase::handleAckMessageSpecialized(routerMsg* ttmsg){
    ackMsg *aMsg = check_and_cast<ackMsg*>(ttmsg->getEncapsulatedPacket()); // the message indeed a ack message  actually get the ack part from routerMsg
    int transactionId = aMsg->getLargerTxnId();
    // int pathIndex = aMsg->getPathIndex();
    // int destNode = ttmsg->getRoute()[0];
    // double largerTxnId = aMsg->getLargerTxnId(); // the largerTxnId is the real tx id
    
    /* if the ack is failed, count in FAIL3*/
    if(aMsg->isSuccess() == true){ 
        //Note: 4.17 update the tx_amount_feeList for final output
        // cout << "ack total fee output: " << aMsg->getTx_fee_amount() << endl;
        vector<pair<double,double>> tempVector = {make_pair(aMsg->getAmount(),aMsg->getTx_fee_amount())}; 
        tx_amount_feeList[transactionId] = tempVector;

        if (_loggingEnabled)
            cout << "transaction: " << transactionId << " is success!" << endl; 
    }
    else {
        if (_loggingEnabled) { //level_0
            cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
            cout << "FAIL3: The ACK is failed so insert this tx into failed_transactionIdSet" << endl;
        }
        fail_3_count++;
        total_amount_reject = total_amount_reject + aMsg->getAmount();
        failed_transactionIdSet.insert(transactionId);
        failed_tx_timeStamp.push_back(simTime().dbl());
    }  
    hostNodeBase::handleAckMessage(ttmsg); // Note: mainly for delete this tx 
}




//Assumption 2
double hostOnlineFeeBase::Assumption2_demand_threshold(double min_ce, double mu_1, double mu_2, double alpha_e, double beta_e){
    double part_1 = min_ce/(log2(mu_1));
    double part_2 = (min_ce * (alpha_e - beta_e)) / (log2(mu_2));
    // if (_loggingEnabled) {
    //     cout << "min_ce: " << min_ce << endl;
    //     cout << "mu_1: " << mu_1 << endl;
    //     cout << "mu_2: " << mu_2 << endl;
    //     cout << "part_1" << part_1 << endl;
    //     cout << "part_2" << part_2 << endl;
    // }fai
    return min(part_1,part_2);
}


//OnlineScheme: to check if the path cost is greater than valuation, line 4 in Algrithom of the paper
bool hostOnlineFeeBase::checkCost(double path_total_cost, double valuation){
        if (path_total_cost <= valuation){
            return true;
        }
        else {
            return false;
        }
}


// Note: return the total_cost, we need more details, this function not used
//Note: the getCost should not include the first channel cost
double hostOnlineFeeBase::getCost(vector<vector<int>> kShortestPaths){
    if (_loggingEnabled) {
    cout << "start to getCost" << endl;
    }
    
    double total_cost = 0;
    for (int pathIdx = 0; pathIdx < kShortestPaths.size(); pathIdx++){ // FIXME: only 1 path
        // initialize pathInfo
        PathInfo temp = {};
        temp.path = kShortestPaths[pathIdx]; // Note: get one of the shortest path
        
        vector<pair<int,double>>::iterator vectIter;
        double tmp_cost = 0;
        // OnlineScheme: traverse this path to get the cost of this path
        // Note: path len is the node number
        for (size_t i = 1; i < temp.path.size()-2; i++) // Note: not to include the first channel: start from i=1 Note: actually, do not include the last channel too, it is itself, one node
        {
            int first_node = temp.path[i];
            int second_node = temp.path[i+1];
            //Note: get the weight of the channel between this two nodes
            for (vectIter = _channel2[first_node].begin(); vectIter != _channel2[first_node].end(); vectIter++){
                if (vectIter->first == second_node){ // if the target node is second node, we need this channel
                    tmp_cost = vectIter->second;
                    if (_loggingEnabled) {
                        cout << "from node " << first_node << " to node " << second_node << endl; 
                        cout << "tmp_cost is:" << tmp_cost << endl;
                    }
                    break;    // one loop  
                }   
            }
            total_cost = total_cost + tmp_cost;
        }
        if (_loggingEnabled) {
            cout << "total cost of this path is :" << total_cost << endl; 
        }
    }
    return total_cost;
}



//Note: not used
/* new getPathCost for ConvolutionalFee:
*/
double hostOnlineFeeBase::getPathCost(vector <double> eachEdgeCost, vector <double> convolution_flow){
    if (_loggingEnabled) {
        cout << "start to getPathCost convolutional" << endl;
    }
    double total_cost = 0;
    // the length of this two vector should be the same
    if (eachEdgeCost.size() != convolution_flow.size()){
        cout <<"The edge cost len is not match to flow len" << endl;
    }
    
    for (int i = 0; i < convolution_flow.size(); i++){
        total_cost = total_cost + eachEdgeCost[i] * convolution_flow[i]; //the cost here already count the time HTLCtime
    }
    return total_cost;
}



//Note: the getCost should not include the first channel cost
/*
return the unit cost vector read from the graph
*/
//Note: return each hop's cost 
vector <double> hostOnlineFeeBase::getEachEdgeCost(vector<vector<int>> kShortestPaths){
    vector <double> eachEdgeCost{};
    if (_loggingEnabled) {
        cout << "start to getEachEdgeCost" << endl;
    }


    for (int pathIdx = 0; pathIdx < kShortestPaths.size(); pathIdx++){ // Note: only 1 path in our case
        // initialize pathInfo
        PathInfo temp = {};
        temp.path = kShortestPaths[pathIdx]; // Note: get one of the shortest path
        vector<pair<int,double>>::iterator vectIter;
        double tmp_cost = 0;


        // OnlineScheme: traverse this path to get the cost of this path
        // cout << "Path size: " << temp.path.size() << endl;
        // Note: the path len here is the node number, which is 4 for a path: 0 4 5 1, this path connected directly, should not charge any fee....
        for (size_t i = 1; i < temp.path.size()-2; i++) 
        // Note: not to include the first channel: start from i=1 Note: actually, do not include the last channel too, it is itself, one node
        // 0-4-7-5-1, node 4 and 7 charge fee for channel 4-7 and 7-5 
        // so traverse from 1(4) to 2(7)
        // path.size() is 5, so from i=1 to i=2(<5-2)
        {
            // cout << "i is " << i << endl;
            int first_node = temp.path[i];
            int second_node = temp.path[i+1];
            //Note: get the weight of the channel between this two nodes
            for (vectIter = _channel2[first_node].begin(); vectIter != _channel2[first_node].end(); vectIter++){
                if (vectIter->first == second_node){ // if the target node is second node, we need this channel
                    tmp_cost = vectIter->second;
                    
                    if (_loggingEnabled) {
                        cout << "from node " << first_node << " to node " << second_node << endl; 
                        cout << "tmp_cost is:" << tmp_cost << endl;
                    }

                    break;    // one loop  
                }   
            }
            eachEdgeCost.push_back(tmp_cost);
        }
    }
    return eachEdgeCost;
}




// ConvolutionalFee: flow
/* this function given dest amount (demand) and the each edge cost (fee rate)
the cost here actually is the flow... total flow.. 
consider the first and last hop, the fee rate is 0, no effect
*/
vector <double> hostOnlineFeeBase::ConvolutionalFlow(double dest_amount,vector <double> eachEdgeCost){
    vector <double> convolution_flow{};
    convolution_flow.push_back(dest_amount); // 0-4-7-5-1 : 5-1 flow
    convolution_flow.push_back(dest_amount); // 7-5 flow
    double hop_flow = 0;
    for (int i = eachEdgeCost.size()-1; i >= 0; i--) { // from the last hop to first hop: eachEdgeCost: [7-5 cost, 4-7 cost]
        // cout << "eachEdgeCost.size(): " << eachEdgeCost.size() << endl;
        // cout << i << endl;
        hop_flow = dest_amount + dest_amount*eachEdgeCost[i]; // equation (1) Note: here the egde cost already x _Z
        dest_amount = hop_flow; // the amount send out from this hop node
        convolution_flow.push_back(dest_amount);
    }
    convolution_flow.push_back(dest_amount); // Note: the last hop from router to host: 0-4 is same as 4-7
    reverse(convolution_flow.begin(), convolution_flow.end()); // Note: reverse this list
    
    if (_loggingEnabled)
        cout << "convolution_flow size  " << convolution_flow.size() << endl;
    
    // the convolution_flow here is the flow should in the channel
    return convolution_flow; 
}



/* if you want to init more 
 * add here 
 */
void hostOnlineFeeBase::initialize() {
    hostNodeBase::initialize();
}
