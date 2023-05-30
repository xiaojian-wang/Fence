#include "routerOnlineFeeBase.h"
#include <typeinfo>
#include <iomanip>

unordered_map<int, unordered_map<int, vector<double>>> _node_txid_flow; // OnlineScheme: storage the flow amount that foward through this router
// vector<double> Note: vector have two value, [0] is the flow amount get from previous node, [1] is the flow send out
unordered_map<int, unordered_map<int, double>> _node_txid_txfee; //Note: to record the txid and txfee go through this node, in case there is a ack back and need to decuce
int frequency_counter = 0;
// unordered_map<int, vector<pair<int,double>>> newest_cost; //Note: the cost that up to date, newest cost, local, not broadcast yet

Define_Module(routerOnlineFeeBase);

/* if you want to init other spectial parameters in routerOnlineFeeBase
 */
void routerOnlineFeeBase::initialize() {
    if (_loggingEnabled) {
        cout << "initialize() of routerOnlineFeeBase" << myIndex() << endl;
    }
    routerNodeBase::initialize();
    if (_loggingEnabled) {
        cout << "initialize() in routerOnlineFeeBase from Base" << endl;
    }
}


/******* MESSAGE HANDLERS **************************/
/* if we want to add LSA message, change here
 */
void routerOnlineFeeBase::handleMessage(cMessage *msg) {
    // cout << "into handleMessage in the routerOnlineFeeBase.cc" << endl;
    routerMsg *ttmsg = check_and_cast<routerMsg *>(msg);
    // if (simTime() > _simulationLength){
    //     auto encapMsg = (ttmsg->getEncapsulatedPacket());
    //     ttmsg->decapsulate();
    //     delete ttmsg;
    //     delete encapMsg;
    //     return;
    // } 
    routerNodeBase::handleMessage(msg);

    // switch(ttmsg->getMessageType()) {
    //     case TRIGGER_PRICE_UPDATE_MSG:
    //          if (_loggingEnabled) cout<< "[ROUTER "<< myIndex() 
    //              <<": RECEIVED TRIGGER_PRICE_UPDATE MSG] "<< ttmsg->getName() << endl;
    //          handleTriggerPriceUpdateMessage(ttmsg);
    //          if (_loggingEnabled) cout<< "[AFTER HANDLING:]  "<< endl;
    //          break;
    //     default:
    //          routerNodeBase::handleMessage(msg);

    // }
}



//TODO: description
/* 1. if previous node is host, do nothing,
If the previous node is a router: update the backward cost and the balance of previous channel since this tx already go through to this node
*/

/* 2. update _node_txid_flow [0] (from previous Node)*/

/* 3. start to deal with this tx try to see if this tx should be send out*/
    /* count the mismatch at router*/
    /* 1. check if the total amount is enough (compare to demand) to send out , check if the fee is enough (-current_channel_cost > 0), if not, FAIL6
            1.1. before generate failedACK, if previous node is not sender, we need first to update the previous channel cost for my view, need to cancel that action at step 1 when I first saw this tx
            1.2. generate failedACK and forward it  
       2. if total amount is enough, then maybe send this tx out, set new total amount and tx fee
            2.1 if next node is receiver, do not need to modify the the tx message, just need to record the flow send out
            2.2 if next node is router, 
                2.2.1 balance violation count
                2.2.2 capacity violation count 
                2.2.2 check if the balance is enough to suppor the flow will be send out (new_total_amount > current balance), means this channel in practice cannot send out this tx, FAIL7
                    2.2.2.1 if FAIL7, need to update the previous channel before generate the failed ack, just like 1.1
                    2.2.2.2 generate failedACK and forward it 
                2.2.3 if the BalanceCheck is success, can finally send out!
                    2.2.3.1 record the flow send out [1]
                    2.2.3.2 record the capacity useage of this channel 
                    2.2.3.3 set new tx fee and new_total_amount
                    2.2.3.4 forward new message to next hop
                    2.2.3.5 update next channel's cost and balance view
    */ 
void routerOnlineFeeBase::handleTransactionMessageSpecialized(routerMsg *ttmsg){
    //Note: count the frequency_counter every time encountere a transaction
    frequency_counter = frequency_counter + 1;
    transactionMsg *transMsg = check_and_cast<transactionMsg *>(ttmsg->getEncapsulatedPacket());  // get the transaction message
    double tx_fee = transMsg->getTx_fee_amount(); 
    double total_amount = transMsg->getTotal_amount(); // Note: the amount arrive at this hop
    double dest_amount = transMsg->getAmount();
    int sender = ttmsg->getRoute()[0];
    int receiver = (ttmsg->getRoute())[(ttmsg->getRoute()).size() -1];
    
    // OnlineScheme: assume the flow amount is total amount, decrease in every hop
    // 6.5 Note: In our algorithm, the cost is set for demand,i.e. the destination amount, but the update of balance is real flow...
    // double flow_amount = total_amount; //Note: if we want to use the real arrive flow to update the cost
    double flow_amount = dest_amount;
    if (_loggingEnabled) {
        cout << "total_amount(arrive at this hop) and dest_amount! : (total, dest) : " <<  total_amount << ", " << dest_amount << endl;
    }

    //OnlineScheme: after get this message, this node should update the backward cost, since got this message means previous channel already changed total_amount flow
    int prevNode = ttmsg->getRoute()[ttmsg->getHopCount() - 1];
    PaymentChannel *neighbor_backward = &(nodeToPaymentChannel[prevNode]); //prevNeighbor

    int vector_size = _channel2[myIndex()].size(); // how many channels I have

    //Note: function Update the previous hop's cost and balance distribution
    // TransactionUpdateCostAndBalance_backward (when a tx arrive at a router)
    // TransactionUpdateCostAndBalance_backward(PaymentChannel *neighbor_backward, ) 
    //forget it... it seems hard to use a function to update these things
    
    //Note: do not init a variable in if-else...
    // these varables are for update the previous hop when receive a tx message and previous hop is not a sender
    double old_lambda_1_backward; // previous channel lambda_1
    double old_lambda_2_backward;
    double new_lambda_1_backward;
    double new_lambda_2_backward;
    double cost1_backward;
    double cost2_backward;
    double new_unit_cost12_backward;
    double old_balance_1; // me to prevNode
    double old_balance_2; // prevNode to me
    double new_lambda_2_calc_by_balance;
    double new_cost_2_calc_by_balance;

    /* 1. if previous node is host, do nothing,
    If the previous node is a router: update the backward cost and the balance of previous channel since this tx already go through to this node
    */
    //OnlineScheme: Note: if the prevNode is sender, then we do not to update anything 
    if (prevNode == sender){
        if (_loggingEnabled) {
            cout << "prevNode is sender! Do not to update this channel's everything!" << endl;
        }
    }
    else{ // If the previous node is a router Note: first update the backward cost since this tx already go through to this node
        old_lambda_1_backward = neighbor_backward->lambda_1; // Note: init in the routerNodeBase.cc initial()
        old_lambda_2_backward = neighbor_backward->lambda_2;
        new_lambda_1_backward = old_lambda_1_backward + flow_amount / nodeToPaymentChannel[prevNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
        new_lambda_2_backward = old_lambda_2_backward - flow_amount / ((_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity ); // Note: backward -
        
        //Note: try to stat the cost1 and cost2 seperately to see if their behavior is correct
        cost_traverse_go_through_edges_count++;
        cost1_total = cost1_total + get_lambda1_cost(old_lambda_1_backward);
        cost2_total = cost2_total + get_lambda2_cost(old_lambda_2_backward);

        if (_loggingEnabled) {
            cout << std::setprecision (20) << "old lambda 1 backward : " << old_lambda_1_backward << endl;
            cout << std::setprecision (20) << "old lambda 2 backward : " << old_lambda_2_backward << endl;
            cout << std::setprecision (20) << "new lambda 1 backward : " << new_lambda_1_backward << endl;
            cout << std::setprecision (20) << "new lambda 2 backward : " << new_lambda_2_backward << endl;
            cout << std::setprecision (20) << "((_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
            cout << std::setprecision (20) << "nodeToPaymentChannel[prevNode].origTotalCapacity: " << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
            cout << std::setprecision (20) << "flow_amount: " << flow_amount << endl;
            cout << std::setprecision (20) << "dest_amount: " << dest_amount << endl;
        }

        // new cost1 and cost2 calculate
        cost1_backward = get_lambda1_cost(new_lambda_1_backward); // so we recalculate it, not use the record one?
        cost2_backward = get_lambda2_cost(new_lambda_2_backward);

        //update the new lambda and cost
        nodeToPaymentChannel[prevNode].lambda_1 = new_lambda_1_backward; //Note: update lambda_1
        nodeToPaymentChannel[prevNode].lambda_2 = new_lambda_2_backward;
        nodeToPaymentChannel[prevNode].cost_1 = cost1_backward;
        nodeToPaymentChannel[prevNode].cost_2 = cost2_backward;

        if (_loggingEnabled) {
            cout << "update the lambda and cost backward" << endl;
            cout << "ttt cost 1 backward : " << cost1_backward << endl;
            cout << "ttt cost 2 backward : " <<  cost2_backward << endl;
        }

        // Note: use the new cost1 and cost2 calculate the new unit cost of the previous channel
        new_unit_cost12_backward = _HTLCtime * cost1_backward + cost2_backward;

        //Update the graph weight to new_unit_cost12_backward (every one's view in the network for centralized version)
        //Note: add frequency check
        if (_updateFrequency == 0){
            for (int i = 0; i < vector_size; i++){ //Note: from my to previous node, update the weight of this channel
                if (_channel2[myIndex()][i].first == prevNode){
                    // _channel2[myIndex()][i].second = new_unit_cost12_backward;
                    if (corruptionStatusList[myIndex()] == 0){
                        _channel2[myIndex()][i].second = new_unit_cost12_backward;
                    }
                    else{
                        _channel2[myIndex()][i].second = new_unit_cost12_backward * _corruptionFeeRatio; // Note: if one node is corrupt, he always set a fee lower _corruptionFeeRatio than the original cost
                    }
                    break;    // one loop 
                }
            }
        }
        else{
            if (frequency_counter % _updateFrequency == nodeToPaymentChannel[prevNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){ //Note: from my to previous node, update the weight of this channel
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_backward * _corruptionFeeRatio; // Note: if one node is corrupt, he always set a fee lower _corruptionFeeRatio than the original cost
                        }
                        break;    // one loop 
                    }
                }
            }
        }
        

        if (_loggingEnabled) {
            cout << "myIndex() is : " << myIndex() << endl;
            cout << "make pair content: (" << prevNode << "," << new_unit_cost12_backward << " )" << endl;
            cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_backward << endl;
        }
        // OnlineScheme: -----end of previous channel update

        // backward update the balance
        // update amount is the flow_amount here
        // 6.5 Note: the update amount is real total amount received total_amount
        old_balance_1 = _balances[make_tuple(myIndex(),prevNode)]; // from me to previous node
        old_balance_2 = _balances[make_tuple(prevNode,myIndex())]; // from previous node to me
        _balances[make_tuple(myIndex(),prevNode)] = old_balance_1 + total_amount; // Note: myself can only have the balance view of my side?yes andNote: here the balance update should be the arrive amount, which is total flow here
        // _balances[make_tuple(prevNode,myIndex())] = old_balance_2 - flow_amount;
        

        //FIXME: try to see why balance is negative 
        // this part should never excute since if a router receive a tx message, it means that the previous router decide send it out and already update 
        // the balance, and in this special branch, the balance is addition, will never be negative
        if (_balances[make_tuple(myIndex(),prevNode)] < 0  ||  _balances[make_tuple(prevNode,myIndex())] <0){
            cout << "Check why your balance is negative??? - transaction backward part" << endl; 
            cout << "old_balance_1 is " << old_balance_1 << endl;
            cout << "old_balance_2 is " << old_balance_2 << endl;
            cout << "total_amount is " << total_amount << endl;
            cout << "from node: " << prevNode << "to " << myIndex() << endl;
            cout << "_balances[make_tuple(myIndex(),prevNode)]: " << _balances[make_tuple(myIndex(),prevNode)] << endl;
            cout << "_balances[make_tuple(prevNode,myIndex())]" <<  _balances[make_tuple(prevNode,myIndex())] << endl;
            cout << "original capacity is : " << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
        }

        //6.29 Note: we do not want to update balance here because maybe it will be canceled, not yet confirmed, we 
        //should update it when receive a success ack, if we receive a failed ack, we do not need to update the balance
        if (_loggingEnabled) {
            cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_1 << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
        }

        // stat the negative
        if (_balances[make_tuple(myIndex(),prevNode)] < 0  ||  _balances[make_tuple(prevNode,myIndex())] <0){
            balance_negative_count++;
        }


        /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
        the value should be the same as update them using algorithm 
        */
        double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),prevNode)]; // from me to previous Node
        _balances_protocol[make_tuple(myIndex(),prevNode)] = old_balance_protocol_1 + dest_amount; // protocol should update the demand amount
        if (_loggingEnabled) {
            cout << std::setprecision (15) << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
        }
        
        // update the lambda1 use equation (6) 
        double total_flow_not_acked_on_channel = 0;
        for (auto iter = nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.end(); ++iter) {
            // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
            total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second; // Note: Note: Note: the flow count here is the real flow, but we need update the flow with dest_flow!!!
        }
        double new_total_flight_flow = total_flow_not_acked_on_channel + dest_amount;
        double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[prevNode].origTotalCapacity;
        double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
        // update the lambda2 use equation (7)
        new_lambda_2_calc_by_balance = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),prevNode)], nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)]);
        new_cost_2_calc_by_balance = get_lambda2_cost(new_lambda_2_calc_by_balance);
        
        if (((new_lambda_1_protocol-new_lambda_1_backward) > _eps) || ((new_lambda_2_calc_by_balance-new_lambda_2_backward) > _eps)) {
            protocol_wrong++;
            if (_loggingEnabled) { //level_0
                cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                cout << "lambda update wrong!!!" << endl;
                cout << std::setprecision (20) << "updated lambda_1 : " << new_lambda_1_backward << endl;
                cout << std::setprecision (20) << "updated lambda_2: " << new_lambda_2_backward << endl;
                cout << std::setprecision (20) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                cout << std::setprecision (20) << "clac lambda_2: " << new_lambda_2_calc_by_balance << endl;
                cout << std::setprecision (20) << "clac lambda_2: _balances_protocol[make_tuple(myIndex(),prevNode)] =  " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                cout << std::setprecision (20) << "clac lambda_2: nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)] =  " << nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                cout << std::setprecision (20) << "clac lambda_2: origTotalCapacity =  " << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << std::setprecision (20) << "clac lambda_1: total_flow_not_acked_on_channel = " << total_flow_not_acked_on_channel << endl;
                cout << std::setprecision (20) << "clac lambda_1: new_total_flight_flow = " << new_total_flight_flow << endl;
                cout << std::setprecision (20) << "clac lambda_1: new_lambda_1_protocol = " << new_lambda_1_protocol << endl;
            }
        }
        /* --------------- end of get two lambda update using balance and total flow */


        //6.23 Note: get the updated lambda_2 use balance
        // _balances[make_tuple(myIndex(),prevNode)] my side balance
        // the previous node expect already update its view of balance
        // new_lambda_2_calc_by_balance = get_lambda2_utilization(_balances[make_tuple(myIndex(),prevNode)], old_balance_2);
        // new_cost_2_calc_by_balance = get_lambda2_cost(new_lambda_2_calc_by_balance);

        // Monitor a channel: MonitorChannel(int node1, int node2, old_lambda_1, old_lambda_2, new_lambda_1, new_lambda_2, capacity, alpha_beta_capacity, flow_amount, total_amount, new_cost1, new_cost2, )
        if (_loggingEnabled && (myIndex()==161 & prevNode == 196)) {
            cout << "old_lambda_1_backward : " << old_lambda_1_backward << endl;
            cout << "old_lambda_2_backward : " << old_lambda_2_backward << endl;
            cout << "new_lambda_1_backward : " << new_lambda_1_backward << endl;
            cout << "new_lambda_2_backward : " << new_lambda_2_backward << endl;
            cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
            cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
            cout << "flow_amount: " << flow_amount << endl;
            cout << "total_amount: " << total_amount << endl;
            cout << "new cost1_backward: " << cost1_backward << endl;
            cout << "new cost2_backward: " << cost2_backward << endl;
            cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_backward << endl;
            cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_1 << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
            cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
            cout << "new_lambda_2_calc_by_balance: " << new_lambda_2_calc_by_balance << endl;
            cout << "new_cost_2_calc_by_balance: " << new_cost_2_calc_by_balance << endl;
            cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_backward - new_lambda_2_calc_by_balance << endl;
        }
    } 
    // OnlineScheme: Note: end of previous channel cost and balance update

    
    /* 2. update _node_txid_flow [0] (from previous Node)*/
    //here we do not care if the previous node is sender or other router, just record it 
    int transactionId = transMsg->getLargerTxnId();
    if (_node_txid_flow[myIndex()][transactionId].size() < 2) { // Note: init the _node_txid_flow: record the flow from previous node
        _node_txid_flow[myIndex()][transactionId].push_back(0); // Note: make sure the index 0 exist
    }
    _node_txid_flow[myIndex()][transactionId][0] = total_amount;  // The real arrive flow amount Note: this node, get the flow from previous node id, flow value; 
    if (_loggingEnabled) {
        cout << "update my _txid_flow: key is " << transactionId << " & value is " << total_amount << endl;
    }


    if (_loggingEnabled) {
        cout << "before Check if the message send from host is changed:" << endl;
        cout << "total_amount is :" << total_amount << endl;
        cout << "dest_amount is :" << dest_amount << endl;
        cout << "total tx_fee(read from message) is :" << tx_fee << endl;
    }
    

    // OnlineScheme: Note: start to deal with this tx try to see if this tx should be send out
    int nextNode = ttmsg->getRoute()[ttmsg->getHopCount()+1]; //Note: myIndex return node index in network, first host, second router
    double current_channel_cost; // Note: from me to next node, I will charge this fee

    // Get this node to next node's channel cost
    vector<pair<int,double>>::iterator vectIter;
    for (vectIter = _channel2[myIndex()].begin(); vectIter != _channel2[myIndex()].end(); vectIter++){
        if (vectIter->first == nextNode){ // if the target node is second node, we need this channel
            current_channel_cost = vectIter->second; // Note: unit cost of this channel
            break;    // one loop  
        }   
    }
    // ConvolutionalFee: count the mismatch
    double expected_hop_flow = transMsg->getConvolution_flow()[ttmsg->getHopCount()+1]; // Note: this flow is the sender expected flow in next hop, maybe not the real flow. we need to use the real flow: 
    double real_hop_flow = total_amount/(1+current_channel_cost); // Note: a router calculate the next hop's flow use the unit cost 
    if (abs(expected_hop_flow - real_hop_flow) > _eps){
        if (_loggingEnabled) { //level_0
            cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
            cout << "MISMATCH: expected flow not equal to real calculated flow! (expected, calculated):  " << "(" << expected_hop_flow << ", " << real_hop_flow << ")" << endl;
        }
        mismatch_count++; //Note: the mismatch count is at router side, so the ratio could greater than 1
    }


    double next_hop_flow = real_hop_flow;
    double unit_current_channel_cost = current_channel_cost;
    current_channel_cost = next_hop_flow * current_channel_cost; //Note: the cost that need to deduct from arrive flow and send out
    double current_balance = _balances[make_tuple(myIndex(),nextNode)]; // from me to nextNode
    // double channel_capacity = current_balance + _balances[make_tuple(nextNode,myIndex())]; 
    double channel_capacity = nodeToPaymentChannel[nextNode].origTotalCapacity; //Note: if we want to use the channel capacity, make sure use the constant one, the blance view is different at nodes, the update is not sync
    
    
    if (_loggingEnabled && (myIndex()==161 & nextNode == 196)) {
        cout << "BALANCE TRACE: " << endl;
        cout << std::setprecision (20) << "Original capacity: " << nodeToPaymentChannel[nextNode].origTotalCapacity << endl; 
        cout << std::setprecision (20) << "add two balance together: " << _balances[make_tuple(myIndex(),nextNode)] + _balances[make_tuple(nextNode,myIndex())] << endl;
        cout << std::setprecision (20) << "from " << myIndex() << "to next " << nextNode << " balance: " << _balances[make_tuple(myIndex(),nextNode)] << endl;
        cout << std::setprecision (20) << "from " << nextNode << "to next " << myIndex() << " balance: " << _balances[make_tuple(nextNode,myIndex())] << endl;
        if (abs(nodeToPaymentChannel[nextNode].origTotalCapacity - (_balances[make_tuple(myIndex(),nextNode)] + _balances[make_tuple(nextNode,myIndex())])) > _eps){
            cout << "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssstop here!xx" << endl;
            cout << "time: " << simTime() << endl;
        }
    }

    if (_loggingEnabled && (myIndex()==196 & prevNode == 161)) {
        cout << "BALANCE TRACE: " << endl;
        cout << std::setprecision (20) << "Original capacity: " << nodeToPaymentChannel[prevNode].origTotalCapacity << endl; 
        cout << std::setprecision (20) << "add two balance together: " << _balances[make_tuple(myIndex(),prevNode)] + _balances[make_tuple(prevNode,myIndex())] << endl;
        cout << std::setprecision (20) << "from " << myIndex() << "to next " << prevNode << " balance: " << _balances[make_tuple(myIndex(),prevNode)] << endl;
        cout << std::setprecision (20) << "from " << prevNode << "to next " << myIndex() << " balance: " << _balances[make_tuple(prevNode,myIndex())] << endl;
        if (abs(nodeToPaymentChannel[prevNode].origTotalCapacity - (_balances[make_tuple(myIndex(),prevNode)] + _balances[make_tuple(prevNode,myIndex())])) > _eps){
            cout << "ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssstop here!" << endl;
            cout << "time: " << simTime() << endl;
        }
    }

    
    // Note: these variables are for if the total_amount is greater than and not equal to the demand amount, if not, FAIL6 
    double old_lambda_1_router_failedack;
    double old_lambda_2_router_failedack;
    double new_lambda_1_router_failedack;
    double new_lambda_2_router_failedack;
    double cost2_1to2_router_failedack;
    double cost1_router_failedack;
    double new_unit_cost12_router_failedack;
    double cancel_previous_action_old_balance_of_my_side;
    double cancel_previous_action_old_balance_of_other_side;
    double cancel_previous_action_new_lambda_2_calc_by_balance;
    double cancel_previous_action_new_cost_2_calc_by_balance;


    // these variables are for the send out amount is greater than balance of this side and cannot send out
    double old_lambda_1_router_failedack_2;
    double old_lambda_2_router_failedack_2;
    double new_lambda_1_router_failedack_2;
    double new_lambda_2_router_failedack_2;
    double cost2_1to2_router_failedack_2;
    double cost1_router_failedack_2;
    double new_unit_cost12_router_failedack_2;
    double cancel_previous_action_old_balance_of_my_side_2;
    double cancel_previous_action_old_balance_of_other_side_2;
    double cancel_previous_action_new_lambda_2_calc_by_balance_2;
    double cancel_previous_action_new_cost_2_calc_by_balance_2;
    
    double old_lambda_1_forward;
    double old_lambda_2_forward;
    double new_lambda_1_forward;
    double new_lambda_2_forward;
    double cost2_1to2;
    double cost1_forward;
    double new_unit_cost12_forward;
    double old_balance_3;
    double old_balance_4;
    double new_lambda_2_calc_by_balance_forward;
    double new_cost_2_calc_by_balance_forward;



    // Note: check if the total arrived amount is less than destination, if it is already smaller than dest_amount, it should be definitly failed, and also check the tx_fee part whether or not less than next channel cost
    /* 1. check if the total amount is enough (compare to demand) to send out , check if the fee is enough (-current_channel_cost > 0), if not, FAIL6
            1.1. before generate failedACK, if previous node is not sender, we need first to update the previous channel cost for my view, need to cancel that action at step 1 when I first saw this tx
            1.2. generate failedACK and forward it  
       2. if total amount is enough, then maybe send this tx out, set new total amount and tx fee
            2.1 if next node is receiver, do not need to modify the the tx message, just need to record the flow send out
            2.2 if next node is router, 

                2.2.2 check if the balance is enough to suppor the flow will be send out (new_total_amount > current balance), means this channel in practice cannot send out this tx, FAIL7
                    2.2.2.1 if FAIL7, need to update the previous channel before generate the failed ack, just like 1.1
                    2.2.2.2 generate failedACK and forward it 
                2.2.3 if the BalanceCheck is success, can finally send out!
                    2.2.3.1 record the flow send out [1]
                    
                    2.2.3.3 set new tx fee and new_total_amount
                    2.2.3.4 forward new message to next hop
                    2.2.3.5 update next channel's cost and balance view

                    2.2.3.2 record the capacity useage of this channel should be the last one since this tx may count itself in next channel...
                    2.2.3.6 balance violation count
                    2.2.3.7 capacity violation count 
    */ 
    double new_total_amount; // Note: the new_total_amount should be send out
    double new_tx_fee; // Note: the total tx fee should be send to next hop
    if ((total_amount - dest_amount < 0 && (!(abs(total_amount-dest_amount) <= _eps))) || (tx_fee - current_channel_cost < 0  && (!(abs(tx_fee-current_channel_cost) <= _eps)))){ 
        if (_loggingEnabled) { //level_0
            cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
            cout << "total_amount : " << total_amount << endl;
            cout << "dest_amount : " << dest_amount << endl;
            cout << "tx_fee : " << tx_fee << endl;
            cout << "unit_current_channel_cost" << unit_current_channel_cost << endl;
            cout << "current_channel_cost : " << current_channel_cost << endl;
            cout << "total_amount < dest_amount  : " << (total_amount - dest_amount < 0) << endl;
            cout << "total_amount - dest_amount  : " << (total_amount - dest_amount) << endl;
            cout << "abs(total_amount-dest_amount) : " << abs(total_amount-dest_amount) << endl;
            cout << "eps is : " << _eps << endl;
            cout << "second abs eps : " << (!(abs(total_amount-dest_amount) <= _eps)) << endl;
            cout << "tx_fee - new_unit_cost12_forward < 0  :" << (tx_fee - current_channel_cost ) << endl;    
            //this tranaction cannot reach destination, negative ack, clear state
            cout << "FAIL6: NOT TO forward this transaction message to next hop since the fee is not enough!" << endl;
        }
        fail_6_count++;
        // cout << "fail6 --------------------------------------------------------------------------------------: " << fail_6_count << endl;
        // cout << "fail3 --------------------------------------------------------------------------------------: " << fail_3_count << endl;
        
        // PaymentChannel *neighbor = &(nodeToPaymentChannel[prevNode]); 
        //OnlineScheme: Note: if the prevNode is sender, then we do not to update anything for it
        if (prevNode == sender){
            if (_loggingEnabled) {
                cout << "prevNode is sender! Do not to update this channel's everything!" << endl;
            }
        }
        else{ // the previous node is not sender, we need to update the previous channel before generate the failed ack
            // Note: here is the thing related to before generate failed ack!!! if the total amount is not enough to go to destination
            // update the cost, but do not need to update the balance of my own view, the previous node will update the view of balance when they get the failed ack
            // is that a problem when lots of overlap? maybe not... since overlap will lead to lots not sync beside the balance...
            // why I need to update the cost1 and cost2 of the previous channel? this flow arrive to me, the first thing I do is to update the previous channel view cost and balance
            // so I need to cancel that action, 
        
            // the ack is generated by yourself, not to update next channel, but need to update the previous channel
            old_lambda_1_router_failedack = neighbor_backward->lambda_1;
            old_lambda_2_router_failedack = neighbor_backward->lambda_2;
            // try to cancel the action of update the previous channel
            new_lambda_1_router_failedack= old_lambda_1_router_failedack - flow_amount / nodeToPaymentChannel[prevNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
            new_lambda_2_router_failedack = old_lambda_2_router_failedack + flow_amount / ((_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity ); // FIXME: the cost may be negative, need to change it

            //Note: try to stat the cost1 and cost2 seperately to see if their behavior is correct
            cost_traverse_go_through_edges_count++;
            cost1_total = cost1_total + get_lambda1_cost(old_lambda_1_router_failedack);
            cost2_total = cost2_total + get_lambda2_cost(old_lambda_2_router_failedack);

            if (_loggingEnabled ) {
                cout << "old_lambda_1_router_failedack : " << old_lambda_1_router_failedack << endl;
                cout << "old_lambda_2_router_failedack : " << old_lambda_2_router_failedack << endl;
                cout << "new_lambda_1_router_failedack : " << new_lambda_1_router_failedack << endl;
                cout << "new_lambda_2_router_failedack : " << new_lambda_2_router_failedack << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "flow_amount: " << flow_amount << endl;
            }

            cost2_1to2_router_failedack = get_lambda2_cost(new_lambda_2_router_failedack); // cost2 for previous hop
            cost1_router_failedack = get_lambda1_cost(new_lambda_1_router_failedack); // cost1 for previous hop

            nodeToPaymentChannel[prevNode].lambda_1 = new_lambda_1_router_failedack;
            nodeToPaymentChannel[prevNode].lambda_2 = new_lambda_2_router_failedack;
            nodeToPaymentChannel[prevNode].cost_1 = cost1_router_failedack;
            nodeToPaymentChannel[prevNode].cost_2 = cost2_1to2_router_failedack;

            new_unit_cost12_router_failedack  = _HTLCtime * cost1_router_failedack + cost2_1to2_router_failedack;

            //Note: add frequency check
            if (_updateFrequency == 0){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_router_failedack;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_router_failedack;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_router_failedack * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            else{
                if (frequency_counter % _updateFrequency == nodeToPaymentChannel[prevNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_router_failedack;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_router_failedack;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_router_failedack * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            }
            

            if (_loggingEnabled ) {
                cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_router_failedack << endl;
            }

            
            //Note: 6.23 also need to cancel the balance view of myself
            cancel_previous_action_old_balance_of_my_side = _balances[make_tuple(myIndex(),prevNode)]; // from me to previous node
            cancel_previous_action_old_balance_of_other_side = _balances[make_tuple(prevNode,myIndex())]; // from previous node to me
            _balances[make_tuple(myIndex(),prevNode)] = cancel_previous_action_old_balance_of_my_side - total_amount; // Note: myself can only have the balance view of my side
            if (_loggingEnabled) {
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << cancel_previous_action_old_balance_of_my_side << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
            }

            // stat the negative
             if (_balances[make_tuple(myIndex(),prevNode)] < 0  ||  _balances[make_tuple(prevNode,myIndex())] <0){
                balance_negative_count++;
             }


            /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
            the value should be the same as update them using algorithm 
            */
            double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),prevNode)]; // from me to previous Node
            _balances_protocol[make_tuple(myIndex(),prevNode)] = old_balance_protocol_1 - dest_amount; // protocol should update the demand amount
            if (_loggingEnabled) {
                cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
            }
            
            // update the lambda1 use equation (6) 
            double total_flow_not_acked_on_channel = 0;
            for (auto iter = nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.end(); ++iter) {
                // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
            }
            double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
            double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[prevNode].origTotalCapacity;
            double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);

            // update the lambda2 use equation (7)
            cancel_previous_action_new_lambda_2_calc_by_balance = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),prevNode)], nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)]);
            cancel_previous_action_new_cost_2_calc_by_balance = get_lambda2_cost(cancel_previous_action_new_lambda_2_calc_by_balance);
            
            if (((new_lambda_1_protocol-new_lambda_1_router_failedack) > _eps) || ((cancel_previous_action_new_lambda_2_calc_by_balance-new_lambda_2_router_failedack) > _eps)) {
                protocol_wrong++;
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                    cout << "lambda update wrong!!!" << endl;
                    cout << std::setprecision (15) << "new_lambda_1_router_failedack : " << new_lambda_1_router_failedack << endl;
                    cout << std::setprecision (15) << "new_lambda_2_router_failedack : " << new_lambda_2_router_failedack << endl;
                    cout << std::setprecision (15) << "new_lambda_1_protocol" << new_lambda_1_protocol << endl;
                    cout << std::setprecision (15) << "cancel_previous_action_new_lambda_2_calc_by_balance" << cancel_previous_action_new_lambda_2_calc_by_balance << endl;
                }
            }
            /* --------------- end of get two lambda update using balance and total flow */


            //log 
            if (_loggingEnabled && (myIndex()==161 & prevNode == 196)) {
                cout << "old_lambda_1_router_failedack : " << old_lambda_1_router_failedack << endl;
                cout << "old_lambda_2_router_failedack : " << old_lambda_2_router_failedack << endl;
                cout << "new_lambda_1_router_failedack : " << new_lambda_1_router_failedack << endl;
                cout << "new_lambda_2_router_failedack : " << new_lambda_2_router_failedack << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "flow_amount: " << flow_amount << endl;
                cout << "total_amount: " << total_amount << endl;
                cout << "new cost2_1to2_router_failedack: " << cost2_1to2_router_failedack << endl;
                cout << "new cost1_router_failedack: " << cost1_router_failedack << endl;
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_router_failedack << endl;
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << cancel_previous_action_old_balance_of_my_side << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
                cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                cout << "cancel_previous_action_new_lambda_2_calc_by_balance: " << cancel_previous_action_new_lambda_2_calc_by_balance << endl;
                cout << "cancel_previous_action_new_cost_2_calc_by_balance: " << cancel_previous_action_new_cost_2_calc_by_balance << endl;
                cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_router_failedack - cancel_previous_action_new_lambda_2_calc_by_balance << endl;
            }
        } // Note: end of action before generate the failedACK because of total_amount is not enough compare to dest_amount

        // start to generate the failed ACK and forward it back to sender
        routerMsg * failedAckMsg = generateAckMessage(ttmsg, false); //Note: it is the reason why segementation fault...
        if (_loggingEnabled) 
            cout << "Router generate a failed ACK since the tx total amount less than dest amount or the tx fee is 0!!" << endl;

        forwardMessage(failedAckMsg);
        if (_loggingEnabled) {
            cout << "After forward the negative ACK message " << endl;
        }
    } // end of the total amount is not enough compare to demand 
    else{// Note: if total amount is enough, then send this tx out, set new total amount and tx fee
        if (nextNode == receiver){ // if next node is receiver, do not need to modify the the tx message, just need to record the flow send out
            if (_loggingEnabled) {
                cout << "nextNode is receiver! do not update this channel since it is itself! " << endl;
                cout << "total amount is enough to support this tx!" << endl;
            }
            if (_node_txid_flow[myIndex()][transactionId].size() < 2) { // make sure _node_txid_flow have at least two element
                _node_txid_flow[myIndex()][transactionId].push_back(0);
            }
            _node_txid_flow[myIndex()][transactionId][1] = total_amount; // Note: the flow amount send out from this node
            // If next node is receiver, we do not need to consider the capacity violation of this channel 

            // transMsg->setTx_fee_amount(tx_fee);
            // transMsg->setTotal_amount(total_amount); // Question: indeed change the content in the ttMsg??? encapsulate? hopefully
            if (_loggingEnabled) {
                cout << "forward this transaction message to next hop (reciver)!" << endl;
                cout << "the total amount value try to set to this message: " << total_amount << endl;
                cout << "the total fee value try to set to this message: " << tx_fee << endl;
                cout << "the message send out from this router: the total_amount is : " << transMsg->getTotal_amount() << endl;
                cout << "the message send out from this router: the total_amount is : " << transMsg->getTx_fee_amount()<< endl;
            }
            forwardMessage(ttmsg);// old message do not change then forward
        }
        else{ //Note: it's a middle router 
              //Note: count the balance violation and capacity violation (in paper)
              // count capacity violate we need to record how many tx still in this channel, how to do that?
              // we need a data structure to store this information, store the flow amount that arrive this channel before from 30ms to 0ms(1 timeslot)  (overlap with this tx)
              // no, we only need to record the txs that not yet ack, we think these txs are still in this channel, has affect on this channel
            if (_loggingEnabled) {
                cout << "total amount is enough to support this tx!" << endl;
            }

            new_total_amount = total_amount - current_channel_cost; // the amount need to send out after deduct the fee
            // BalanceCheck: need to check if the balance is enough to the new total amount!
            
            
            if (new_total_amount > current_balance) { // Note: if the new_total_amount is greater than current balance, means this channel in practice cannot send out this tx, FAIL7
                // cout << "current_channel_cost : " << current_channel_cost << endl; 
                fail7_totalAmount.push_back(total_amount);
                fail7_currentBalance.push_back(current_balance); // Note: record the gap
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                    cout << "total_amount   " << total_amount << endl;
                    cout << "current_balance   " << current_balance << endl; // FIXME: why current balance is negative? 
                    cout << "FAIL7: Capacity check: BalanceCheck: then this tx is failed, generate the failed ack" << endl;
                }
                fail_7_count++;

                // if FAIL7, need to update the previous channel before generate the failed ack, just like 1.1
                //OnlineScheme: Note: if the prevNode is sender, then we do not to update anything for 
                if (prevNode == sender){
                    if (_loggingEnabled) {
                        cout << "prevNode is sender! Do not to update this channel's everything!" << endl;
                    }
                }
                else{ // Note: FAIL7 prevNode is not the sender, need to update the previous channel's cost and balance
                // Note: here is the thing related to before generate failed ack!!!
                    old_lambda_1_router_failedack_2 = neighbor_backward->lambda_1;
                    old_lambda_2_router_failedack_2 = neighbor_backward->lambda_2;
                    // try to cancel the action of update the previous channel
                    new_lambda_1_router_failedack_2= old_lambda_1_router_failedack_2 - flow_amount / nodeToPaymentChannel[prevNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
                    new_lambda_2_router_failedack_2 = old_lambda_2_router_failedack_2 + flow_amount / ((_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity ); // FIXME: the cost may be negative, need to change it

                    //Note: try to stat the cost1 and cost2 seperately to see if their behavior is correct
                    cost_traverse_go_through_edges_count++;
                    cost1_total = cost1_total + get_lambda1_cost(old_lambda_1_router_failedack_2);
                    cost2_total = cost2_total + get_lambda2_cost(old_lambda_2_router_failedack_2);

                    if (_loggingEnabled ) {
                        cout << "old_lambda_1_router_failedack_2 : " << old_lambda_1_router_failedack_2 << endl;
                        cout << "old_lambda_2_router_failedack_2 : " << old_lambda_2_router_failedack_2 << endl;
                        cout << "new_lambda_1_router_failedack_2 : " << new_lambda_1_router_failedack_2 << endl;
                        cout << "new_lambda_2_router_failedack_2 : " << new_lambda_2_router_failedack_2 << endl;
                        cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                        cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                        cout << "flow_amount: " << flow_amount << endl;
                    }

                    cost2_1to2_router_failedack_2 = get_lambda2_cost(new_lambda_2_router_failedack_2); // cost2 for previous hop
                    cost1_router_failedack_2 = get_lambda1_cost(new_lambda_1_router_failedack_2); // cost1 for previous hop

                    nodeToPaymentChannel[prevNode].lambda_1 = new_lambda_1_router_failedack_2;
                    nodeToPaymentChannel[prevNode].lambda_2 = new_lambda_2_router_failedack_2;
                    nodeToPaymentChannel[prevNode].cost_1 = cost1_router_failedack_2;
                    nodeToPaymentChannel[prevNode].cost_2 = cost2_1to2_router_failedack_2;

                    new_unit_cost12_router_failedack_2  = _HTLCtime * cost1_router_failedack_2 + cost2_1to2_router_failedack_2;

                    //Note: add frequency check
                    if (_updateFrequency == 0){
                        for (int i = 0; i < vector_size; i++){
                            if (_channel2[myIndex()][i].first == prevNode){
                                // _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2;
                                if (corruptionStatusList[myIndex()] == 0){
                                    _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2;
                                }
                                else{
                                    _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2 * _corruptionFeeRatio;
                                }
                                break;    // one loop 
                            }
                        }
                    }
                    else{
                        if (frequency_counter % _updateFrequency == nodeToPaymentChannel[prevNode].updateFrequencyEntry){
                        for (int i = 0; i < vector_size; i++){
                            if (_channel2[myIndex()][i].first == prevNode){
                                // _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2;
                                if (corruptionStatusList[myIndex()] == 0){
                                    _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2;
                                }
                                else{
                                    _channel2[myIndex()][i].second = new_unit_cost12_router_failedack_2 * _corruptionFeeRatio;
                                }
                                break;    // one loop 
                            }
                        }
                    }
                    }
                    

                    if (_loggingEnabled ) {
                        cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_router_failedack_2 << endl;
                    }

                    
                    //Note: 6.23 also need to cancel the balance view of myself
                    cancel_previous_action_old_balance_of_my_side_2 = _balances[make_tuple(myIndex(),prevNode)]; // from me to previous node
                    cancel_previous_action_old_balance_of_other_side_2 = _balances[make_tuple(prevNode,myIndex())]; // from previous node to me
                    _balances[make_tuple(myIndex(),prevNode)] = cancel_previous_action_old_balance_of_my_side_2 - total_amount; // Note: myself can only have the balance view of my side
                    if (_loggingEnabled) {
                        cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_1 << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
                    }

                    // stat the negative
                    if (_balances[make_tuple(myIndex(),prevNode)] < 0  ||  _balances[make_tuple(prevNode,myIndex())] <0){
                        balance_negative_count++;
                    }

                    /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
                    the value should be the same as update them using algorithm 
                    */
                    double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),prevNode)]; // from me to previous Node
                    _balances_protocol[make_tuple(myIndex(),prevNode)] = old_balance_protocol_1 - dest_amount; // protocol should update the demand amount
                    if (_loggingEnabled) {
                        cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                    }
                    
                    // update the lambda1 use equation (6) 
                    double total_flow_not_acked_on_channel = 0;
                    for (auto iter = nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.end(); ++iter) {
                        // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                        total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
                    }
                    double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
                    double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[prevNode].origTotalCapacity;
                    double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
                    // update the lambda2 use equation (7)
                    cancel_previous_action_new_lambda_2_calc_by_balance_2 = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),prevNode)], nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)]);
                    cancel_previous_action_new_cost_2_calc_by_balance_2 = get_lambda2_cost(cancel_previous_action_new_lambda_2_calc_by_balance_2);
                    
                    if (((new_lambda_1_protocol-new_lambda_1_router_failedack_2) > _eps) || ((cancel_previous_action_new_lambda_2_calc_by_balance_2-new_lambda_2_router_failedack_2) > _eps)) {
                        protocol_wrong++;
                        if (_loggingEnabled) { //level_0
                            cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                            cout << "lambda update wrong!!!" << endl;
                            cout << std::setprecision (15) << "updated lambda_1 : " << new_lambda_1_router_failedack_2 << endl;
                            cout << std::setprecision (15) << "updated lambda_2: " << new_lambda_2_router_failedack_2 << endl;
                            cout << std::setprecision (15) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                            cout << std::setprecision (15) << "clac lambda_2: " << cancel_previous_action_new_lambda_2_calc_by_balance_2 << endl;
                        }
                    }
                    /* --------------- end of get two lambda update using balance and total flow */

                    // cancel_previous_action_new_lambda_2_calc_by_balance_2 = get_lambda2_utilization(_balances[make_tuple(myIndex(),prevNode)], cancel_previous_action_old_balance_of_other_side_2);
                    // cancel_previous_action_new_cost_2_calc_by_balance_2 = get_lambda2_cost(cancel_previous_action_new_lambda_2_calc_by_balance_2);

                    //log 
                    if (_loggingEnabled && (myIndex()==161 & prevNode == 196)) {
                        cout << "old_lambda_1_router_failedack_2 : " << old_lambda_1_router_failedack_2 << endl;
                        cout << "old_lambda_2_router_failedack_2 : " << old_lambda_2_router_failedack_2 << endl;
                        cout << "new_lambda_1_router_failedack_2 : " << new_lambda_1_router_failedack_2 << endl;
                        cout << "new_lambda_2_router_failedack_2 : " << new_lambda_2_router_failedack_2 << endl;
                        cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                        cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                        cout << "flow_amount: " << flow_amount << endl;
                        cout << "total_amount: " << total_amount << endl;
                        cout << "new cost2_1to2_router_failedack_2: " << cost2_1to2_router_failedack_2 << endl;
                        cout << "new cost1_router_failedack_2: " << cost1_router_failedack_2 << endl;
                        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_router_failedack_2 << endl;
                        cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << cancel_previous_action_old_balance_of_my_side_2 << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
                        cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                        cout << "cancel_previous_action_new_lambda_2_calc_by_balance_2: " << cancel_previous_action_new_lambda_2_calc_by_balance_2 << endl;
                        cout << "cancel_previous_action_new_cost_2_calc_by_balance_2: " << cancel_previous_action_new_cost_2_calc_by_balance_2 << endl;
                        cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_router_failedack_2 - cancel_previous_action_new_lambda_2_calc_by_balance_2 << endl;
                    }
                } // end of the actions(update cost and balance) before generate the failedACK

                routerMsg * failedAckMsg = generateAckMessage(ttmsg, false); //Note: we generate a failed ack here because the balance of channel's this side cannot support this tx send out
                forwardMessage(failedAckMsg);
                if (_loggingEnabled) {
                    cout << "After forward the negative ACK message " << endl;
                }
            } // end of balance check failed 
            else{ // Note: the BalanceCheck is success, can finally send out!
                // record the tx flow
                if (_node_txid_flow[myIndex()][transactionId].size() < 2) {
                    _node_txid_flow[myIndex()][transactionId].push_back(0);
                }
                _node_txid_flow[myIndex()][transactionId][1] = new_total_amount; // Note: the flow amount send out from this node



                // set new tx fee and new_total_amount
                new_tx_fee = tx_fee - current_channel_cost;
                transMsg->setTx_fee_amount(new_tx_fee);
                transMsg->setTotal_amount(new_total_amount); 

                //Note: 4.15 update the node txid txfee, in case failback, for finally incomelist update
                _node_txid_txfee[myIndex()][transactionId] = current_channel_cost;
                // node_incomeList[myIndex()].push_back(current_channel_cost);
                

                
                if (_loggingEnabled) {
                    cout << "forward this transaction message to next hop!" << endl;
                }
                forwardMessage(ttmsg);// new message

                
                if (_loggingEnabled) {
                    cout << "Check if the message send from host is changed:" << endl;
                    cout << "new set total_amount is :" << transMsg->getTotal_amount() << endl;
                    cout << "dest_amount is :" << dest_amount << endl;
                    cout << "new set tx_fee is :" << transMsg->getTx_fee_amount() << endl;
                }

 
                PaymentChannel *neighbor = &(nodeToPaymentChannel[nextNode]); // Note: get the channel from current node to nextNode
                
                // forward path cost and balance update
                old_lambda_1_forward = neighbor->lambda_1;
                old_lambda_2_forward = neighbor->lambda_2;
                new_lambda_1_forward = old_lambda_1_forward + dest_amount / nodeToPaymentChannel[nextNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
                new_lambda_2_forward = old_lambda_2_forward + dest_amount / ((_alphae - _betae) * nodeToPaymentChannel[nextNode].origTotalCapacity);
                
                //Note: try to stat the cost1 and cost2 seperately to see if their behavior is correct
                cost_traverse_go_through_edges_count++;
                cost1_total = cost1_total + get_lambda1_cost(old_lambda_1_forward);
                cost2_total = cost2_total + get_lambda2_cost(old_lambda_2_forward);

                cost2_1to2 = get_lambda2_cost(new_lambda_2_forward); // cost2 forward
                cost1_forward = get_lambda1_cost(new_lambda_1_forward); // cost1 forward

                // update the lambda and cost
                nodeToPaymentChannel[nextNode].lambda_1 = new_lambda_1_forward; 
                nodeToPaymentChannel[nextNode].lambda_2 = new_lambda_2_forward;
                nodeToPaymentChannel[nextNode].cost_1 = cost1_forward;
                nodeToPaymentChannel[nextNode].cost_2 = cost2_1to2;
                
                if (_loggingEnabled) {
                    cout << "update the lambda and cost forward" << endl; 
                }

                if (_loggingEnabled) {
                    cout << std::setprecision (20) << "old lambda 1 forward : " << old_lambda_1_forward << endl;
                    cout << std::setprecision (20) << "old lambda 2 forward : " << old_lambda_2_forward << endl;
                    cout << std::setprecision (20) << "new_total_amount : " << new_total_amount << endl;
                    cout << std::setprecision (20) << " nodeToPaymentChannel[nextNode].origTotalCapacity: " << nodeToPaymentChannel[nextNode].origTotalCapacity<< endl;
                    cout << std::setprecision (20) << "new lambda 1 forward : " << new_lambda_1_forward << endl;
                    cout << std::setprecision (20) << "new lambda 2 forward : " << new_lambda_2_forward << endl;
                    cout << std::setprecision (20) << "ttt cost1_forward : " << cost1_forward << endl;
                    cout << std::setprecision (20) << "ttt cost 2 forward : " <<  cost2_1to2 << endl;
                    cout << "_HTLCtime : " << _HTLCtime << endl;
                }

                new_unit_cost12_forward = _HTLCtime * cost1_forward + cost2_1to2;
                
                if (_loggingEnabled) {
                cout << "previous weight 0: " << _channel2[myIndex()][0].second << endl;
                cout << "previous weight 1: " << _channel2[myIndex()][1].second << endl;
                }

                // _channel2[myIndex()].push_back(make_pair(nextNode, new_unit_cost12_forward));
                //Note: add frequency check
                if (_updateFrequency == 0){
                    for (int i = 0; i < vector_size; i++){
                        if (_channel2[myIndex()][i].first == nextNode){
                            // _channel2[myIndex()][i].second = new_unit_cost12_forward;
                            if (corruptionStatusList[myIndex()] == 0){
                                _channel2[myIndex()][i].second = new_unit_cost12_forward;
                            }
                            else{
                                _channel2[myIndex()][i].second = new_unit_cost12_forward * _corruptionFeeRatio;
                            }
                            break;    // one loop 
                        }
                    }
                }
                else{
                    if (frequency_counter % _updateFrequency == nodeToPaymentChannel[nextNode].updateFrequencyEntry){
                    for (int i = 0; i < vector_size; i++){
                        if (_channel2[myIndex()][i].first == nextNode){
                            // _channel2[myIndex()][i].second = new_unit_cost12_forward;
                            if (corruptionStatusList[myIndex()] == 0){
                                _channel2[myIndex()][i].second = new_unit_cost12_forward;
                            }
                            else{
                                _channel2[myIndex()][i].second = new_unit_cost12_forward * _corruptionFeeRatio;
                            }
                            break;    // one loop 
                        }
                    }
                }
                }
                

                if (_loggingEnabled) {
                    cout << "myIndex() is : " << myIndex() << endl;
                    cout << "make pair content: (" << nextNode << "," << new_unit_cost12_forward << " )" << endl;
                    cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_forward << endl;
                }

                
                // update amount is the total_amount here
                old_balance_3 = _balances[make_tuple(myIndex(),nextNode)];
                old_balance_4 = _balances[make_tuple(nextNode,myIndex())];
                // cout << "why my balance deduct is not correct???? " << old_balance_3 << " - " <<  total_amount << " = " <<  old_balance_3 - total_amount << endl;
                _balances[make_tuple(myIndex(),nextNode)] = old_balance_3 - new_total_amount; //Note: balance here need to update the new_total_amount, not the old one!!!!
                // _balances[make_tuple(nextNode,myIndex())] = old_balance_4 + total_amount; // this is other's view

                if (_loggingEnabled) {
                    cout << std::setprecision (20) << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_3 << "  to   " << _balances[make_tuple(myIndex(),nextNode)] << endl;
                }

                // stat the negative
                if (_balances[make_tuple(myIndex(),nextNode)] < 0  ||  _balances[make_tuple(nextNode,myIndex())] <0){
                    balance_negative_count++;
                }

                /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
                the value should be the same as update them using algorithm 
                */
                double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),nextNode)]; // from me to next Node
                _balances_protocol[make_tuple(myIndex(),nextNode)] = old_balance_protocol_1 - dest_amount; // protocol should update the demand amount
                if (_loggingEnabled) {
                    cout << std::setprecision (20) << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),nextNode)] << endl;
                }
                
                // update the lambda1 use equation (6) 
                double total_flow_not_acked_on_channel = 0;
                for (auto iter = nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.end(); ++iter) {
                    // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                    total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
                }
                double new_total_flight_flow = total_flow_not_acked_on_channel + dest_amount;
                double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[nextNode].origTotalCapacity;
                double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
                // update the lambda2 use equation (7)
                new_lambda_2_calc_by_balance_forward = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),nextNode)], nodeToPaymentChannel[nextNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),nextNode)]);
                new_cost_2_calc_by_balance_forward = get_lambda2_cost(new_lambda_2_calc_by_balance_forward);
                
                if (((new_lambda_1_protocol-new_lambda_1_forward) > _eps) || ((new_lambda_2_calc_by_balance_forward-new_lambda_2_forward) > _eps)) {
                    protocol_wrong++;
                    if (_loggingEnabled) { //level_0
                        cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                        cout << "lambda update wrong!!!" << endl;
                        cout << std::setprecision (20) << "updated lambda_1 : " << new_lambda_1_forward << endl;
                        cout << std::setprecision (20) << "updated lambda_2: " << new_lambda_2_forward << endl;
                        cout << std::setprecision (20) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                        cout << std::setprecision (20) << "clac lambda_2: " << new_lambda_2_calc_by_balance_forward << endl;
                    }
                }
                /* --------------- end of get two lambda update using balance and total flow */
                // new_lambda_2_calc_by_balance_forward = get_lambda2_utilization(_balances[make_tuple(myIndex(),nextNode)], old_balance_4);
                // new_cost_2_calc_by_balance_forward = get_lambda2_cost(new_lambda_2_calc_by_balance_forward);

                if ( _loggingEnabled && (myIndex()==161 & nextNode == 196)) {
                    cout << "old_lambda_1_forward : " << old_lambda_1_forward << endl;
                    cout << "old_lambda_2_forward : " << old_lambda_2_forward << endl;
                    cout << "new_lambda_1_forward : " << new_lambda_1_forward << endl;
                    cout << "new_lambda_2_forward : " << new_lambda_2_forward << endl;
                    cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                    cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                    cout << "flow_amount (dest_amount): " << flow_amount << endl;
                    cout << "new_total_amount: " << new_total_amount << endl;
                    cout << "new cost1_forward: " << cost1_forward << endl;
                    cout << "new cost2_1to2: " << cost2_1to2 << endl;
                    cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_forward << endl;
                    cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_3 << "  to   " << _balances[make_tuple(myIndex(),nextNode)] << endl;
                    cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),nextNode)] << endl;
                    cout << "new_lambda_2_calc_by_balance_forward: " << new_lambda_2_calc_by_balance_forward << endl;
                    cout << "new_cost_2_calc_by_balance_forward: " << new_cost_2_calc_by_balance_forward << endl;
                    cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_forward - new_lambda_2_calc_by_balance_forward << endl;
                }

                // capacityRecord: record the capacity usage of this channel 
                nodeToPaymentChannel[nextNode].not_yet_acked_tx[transactionId] = new_total_amount;
                nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol[transactionId] = dest_amount;

                // Note: if _capacityScaleEnable, we want to compare all the stuff with real capacity which need to roll back to the real capacity
                double scaled_up_capacity;
                if (_capacityScaleEnable){
                    // cout << "channel_capacity scaled before: " << channel_capacity << endl;
                    scaled_up_capacity = channel_capacity * (1+_P);
                    // cout << "channel_capacity after scaled: " << channel_capacity << endl;
                }

                // after finally send out, we should check if the two constrains violate or not
                // 6.5 Note: in our new algo, balance check is not need but need to stat it if violate
                // if (_alphaeCheckFlag && (current_balance - new_total_amount) < (1-_alphae) * scaled_up_capacity){ // wierd... why always violate this constrains
                if (_alphaeCheckFlag && (current_balance - new_total_amount) < (channel_capacity-channel_capacity * _alphae * (1+_P))){ // 
                    if (_loggingEnabled) { //level_0
                        cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                        cout << "VIOLATE BALANCE CONSTRAINT! (current balance, new_total_amount, (1-alpha_e c_e)):   (" << current_balance << "," << new_total_amount << "," << (1-_alphae) * scaled_up_capacity << ")"<< endl;
                        cout << "new_total_amount (the amount want to send out from this channel)   " << new_total_amount << endl;
                        cout << "current_balance   " << current_balance << endl; // FIXME: why current balance is negative? 
                        cout << "_alphae * channel_capacity   " << (_alphae * channel_capacity) << endl;
                        cout << "(1-_alphae) * channel_capacity   " << (1-_alphae) * channel_capacity << endl;
                        cout << "\n" << endl;
                        cout << "original capacity: " << channel_capacity << endl;
                        cout << "scaled up capacity: " << scaled_up_capacity << endl;
                        cout << "(channel_capacity-channel_capacity * _alphae * (1+_P)): " << (channel_capacity-channel_capacity * _alphae * (1+_P)) << endl;
                        cout << "(1-_alphae) * scaled_up_capacity: " << (1-_alphae) * scaled_up_capacity << endl;
                    }
                    violate_balance_count++;
                    balance_violation_txIdSet.insert(transactionId);
                    // Note: try to see why violate balance always exist even if the txs no overlap
                }

                // capacityRecord: check capapcity violation Note: use the real flow amount to check
                total_flow_not_acked_on_channel = 0;
                for (auto iter = nodeToPaymentChannel[nextNode].not_yet_acked_tx.begin(); iter != nodeToPaymentChannel[nextNode].not_yet_acked_tx.end(); ++iter) {
                    // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                    total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
                }
                if (total_flow_not_acked_on_channel > channel_capacity) { // do not need to add this flow, line 873 already updated
                    if (_loggingEnabled) { //level_0
                        cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " | " << "Tx ID: " << transactionId << " ]" << endl;
                        cout << "VIOLATE CAPACITY CONSTRAINT! (total_flow_not_acked_on_channel, new_total_amount, c_e):   (" << total_flow_not_acked_on_channel << "," << new_total_amount << "," << channel_capacity << ")"<< endl;
                    }
                    violate_capacity_count++;
                }
                if (_loggingEnabled)
                    cout << "capacityRecord: check capapcity violation total_flow_not_acked_on_channel: " << total_flow_not_acked_on_channel << endl;


            }

        }

    }
    

    if (_loggingEnabled) {
        cout << "-=-=-= end of router handle tx message special -=-=-=" << endl;
    }

}



/* if want to handle LSA message
 */
void routerOnlineFeeBase::handleTransactionMessage(routerMsg* ttmsg){ 
    // cout << "into routerOnlineFeeBase::handleTransactionMessage" << endl;
    handleTransactionMessageSpecialized(ttmsg);

}



//TODO: add description
// capacityRecord: cancel this tx since this tx no longer use these two channels
/* 1. if the router receive a success ack
        1.1 if the previous node of this ack message is ack sender(the receiver of this tx who generate this success ack), do not need to update this channel
        1.2 if the previous node is not sender, the success ack is received,  
            1.2.1 first to update the previous channel's cost
                // do not need to update the balance, the success ack only make me more sure that the previous balance update is success
                // only need to update the cost since the success ack back cause a tx no longer use that channel and cost2 keep the same but cost1 deduct
                // 1.2.1.1 the update also should be dest_amount since we update the cost in dest_amount before
            1.2.2 after update the previous hop, consider update the next hop's cost
                1.2.2.1 if next node is the ack receiver, do not update this hop since this hop is to sender (ack destination) 
                1.2.2.2 if next node is a router, update next channel's cost
            1.2.3 forward this ack to next node
    2. if the router receive a failed ACK, since this ack is not generate by itself, need to update the backward and forward both
        2.1 if the previous node is ack sender, do not need to update cost and balance
        2.2 if previous node is another router, 
            2.2.1 update the previous hop's cost and balance 
            2.2.2 update the forward hop's cost and balance
        2.3 forward this failed ack out
*/
// handleAckMessageSpecialized
void routerOnlineFeeBase::handleAckMessage(routerMsg *ttmsg){
    // cout << "into routerOnlineFeeBase::handleAckMessage" << endl;
    assert(myIndex() == ttmsg->getRoute()[ttmsg->getHopCount()]); // Note: if false, this ack should not send to me
    ackMsg *aMsg = check_and_cast<ackMsg *>(ttmsg->getEncapsulatedPacket());// Note: get the ack message
    double dest_amount = aMsg->getAmount();
    int sender = ttmsg->getRoute()[0]; //ack's sender
    int receiver = (ttmsg->getRoute())[(ttmsg->getRoute()).size() -1];
    int vector_size = _channel2[myIndex()].size();
    int transactionId = aMsg->getLargerTxnId();

    // Note: this flow is the real flow amount on this channel, for update the balance of channel 
    double flow_amount = _node_txid_flow[myIndex()][transactionId][1]; // the flow amount send out previously // Note: [0] is the forward flow, for ack message  0-[0]--0--[1]-0
    double flow_amount_forward = _node_txid_flow[myIndex()][transactionId][0];

    int prevNode = ttmsg->getRoute()[ttmsg->getHopCount() - 1];
    int nextNode = ttmsg->getRoute()[ttmsg->getHopCount()+1]; //Note: myIndex return node index in network, first host, second router
    
    // capacityRecord: cancel this tx since this tx no longer use these two channels
    nodeToPaymentChannel[nextNode].not_yet_acked_tx.erase(transactionId);
    nodeToPaymentChannel[prevNode].not_yet_acked_tx.erase(transactionId);
    nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.erase(transactionId);
    nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.erase(transactionId);

    PaymentChannel *neighbor_backward = &(nodeToPaymentChannel[prevNode]); //ack's prevNeighbor

    
    // these variables are for a success ack update its backward
    double old_lambda_1_backward; 
    double old_lambda_2_backward;
    double new_lambda_1_backward;
    // double new_lambda_2_backward;
    double cost1_backward;
    double cost2_backward; // use the old_lambda_2_backward calculate
    double new_unit_cost12_backward;
    // double old_balance_1;
    // double old_balance_2;
    double lambda_2_calc_by_balance;
    double cost_2_calc_by_balance;

    //these variables are for a success ack update its forward
    double old_lambda_1_forward;
    double old_lambda_2_forward;
    double new_lambda_1_forward;
    double cost1_forward;
    double cost2_1to2;
    double new_unit_cost12_forward;
    // double old_balance_3;
    // double old_balance_4;
    double lambda_2_calc_by_balance_forward;
    double cost_2_calc_by_balance_forward;

    //these variables are for a failed ack update its backward
    double old_lambda_1_failACK_backward; 
    double old_lambda_2_failACK_backward;
    double new_lambda_1_failACK_backward;
    double new_lambda_2_failACK_backward;
    double cost1_failACK_backward;
    double cost2_failACK_backward; 
    double new_unit_cost12_failACK_backward;
    double old_balance_1_failACK;
    double old_balance_2_failACK;
    double new_lambda_2_calc_by_balance_failACK;
    double new_cost_2_calc_by_balance_failACK;

    //these variables are for a failed ack update its forward
    double old_lambda_1_failACK_forward; 
    double old_lambda_2_failACK_forward;
    double new_lambda_1_failACK_forward;
    double new_lambda_2_failACK_forward;
    double cost1_failACK_forward;
    double cost2_failACK_forward; 
    double new_unit_cost12_failACK_forward;
    double old_balance_3_failACK;
    double old_balance_4_failACK;
    double new_lambda_2_calc_by_balance_failACK_forward;
    double new_cost_2_calc_by_balance_failACK_forward;


    if (aMsg->isSuccess() == true){ // if the router receive a success ack
        if (_loggingEnabled) {
            cout << "-*- ack flow amount backward [1]: " << flow_amount << endl;
            cout << "get my _txid_flow: key is " << transactionId << " & value is " << flow_amount << endl;
        }

        node_incomeList[myIndex()].push_back(_node_txid_txfee[myIndex()][transactionId]); //Note: 4.15 update income list
        
        
        if (sender == prevNode){ // if the ack sender is previous Node
            if (_loggingEnabled) {
                cout << "do not update this hop since this hop is to destination (ack sender) !" << endl; 
            }
        }
        else{ // Note: if the previous node is not sender, the success ack is received, 
            // do not need to update the balance, the success ack only make me more sure that the previous balance update is success
            // only need to update the cost since the success ack back cause a tx no longer use that channel and cost2 keep the same but cost1 deduct
            old_lambda_1_backward = neighbor_backward->lambda_1; // FIXME: who update the lambda? this function // TODO: need to inital it?
            old_lambda_2_backward = neighbor_backward->lambda_2;
            new_lambda_1_backward = old_lambda_1_backward - dest_amount / nodeToPaymentChannel[prevNode].origTotalCapacity;  // here the update also should be dest_amount since we update the cost in dest_amount before
            // new_lambda_2_backward = old_lambda_2_backward; // Note: backward -
            
            if (_loggingEnabled || _loggingEnabled && (myIndex()==161 & prevNode == 196)) {
                cout << "old lambda 1 backward : " << old_lambda_1_backward << endl;
                cout << "old lambda 2 backward : " << old_lambda_2_backward << endl;
                cout << "new lambda 1 backward : " << new_lambda_1_backward << endl;
                // cout << "new lambda 2 backward : " << new_lambda_2_backward << endl;
            }

            cost1_backward = get_lambda1_cost(new_lambda_1_backward);
            cost2_backward = get_lambda2_cost(old_lambda_2_backward);

            nodeToPaymentChannel[prevNode].lambda_1 = new_lambda_1_backward;
            // nodeToPaymentChannel[prevNode].lambda_2 = new_lambda_2_backward;
            nodeToPaymentChannel[prevNode].cost_1 = cost1_backward;
            // nodeToPaymentChannel[prevNode].cost_2 = cost2_2to1;

            new_unit_cost12_backward = _HTLCtime * cost1_backward + cost2_backward;
            
            // //Note: try to stat the cost1 and cost2 seperately to see if their behavior is correct
            // cost_traverse_go_through_edges_count++;
            // cost1_total = cost1_total + cost1_backward;
            // cost2_total = cost2_total + cost2_backward;

            //Note: add frequency check
            if (_updateFrequency == 0){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_backward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            else{
                if (frequency_counter % _updateFrequency == nodeToPaymentChannel[prevNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_backward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_backward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            }
            

            if (_loggingEnabled) {
                cout << "ACK +++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_backward << endl;
            }

            // old_balance_1 = _balances[make_tuple(myIndex(),prevNode)]; // from me to previous node
            // old_balance_2 = _balances[make_tuple(prevNode,myIndex())]; // from previous node to me
            // lambda_2_calc_by_balance = get_lambda2_utilization(old_balance_1, old_balance_2);
            // cost_2_calc_by_balance = get_lambda2_cost(lambda_2_calc_by_balance);

            /* ---------------start of get lambda1 and lambda2, according to equation (6) and (7) in paper
            the value should be the same as update them using algorithm 
            */
            // calc the lambda1 use equation (6) 
            double total_flow_not_acked_on_channel = 0;
            for (auto iter = nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.end(); ++iter) {
                // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
            }
            double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
            double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[prevNode].origTotalCapacity;
            double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
            // update the lambda2 use equation (7)
            lambda_2_calc_by_balance = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),prevNode)], nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)]);
            cost_2_calc_by_balance = get_lambda2_cost(lambda_2_calc_by_balance);
            
            if (((new_lambda_1_protocol-new_lambda_1_backward) > _eps) || ((lambda_2_calc_by_balance-old_lambda_2_backward) > _eps)) {
                protocol_wrong++;
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " ]" << endl;
                    cout << "lambda update wrong!!!" << endl;
                    cout << std::setprecision (15) << "updated lambda_1 : " << new_lambda_1_backward << endl;
                    cout << std::setprecision (15) << "old lambda_2: " << old_lambda_2_backward << endl;
                    cout << std::setprecision (15) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                    cout << std::setprecision (15) << "clac lambda_2: " << lambda_2_calc_by_balance << endl;
                }
            }
            /* --------------- end of get two lambda update using balance and total flow */


            // Monitor a channel: MonitorChannel(int node1, int node2, old_lambda_1, old_lambda_2, new_lambda_1, new_lambda_2, capacity, alpha_beta_capacity, flow_amount, total_amount, new_cost1, new_cost2, )
            if (_loggingEnabled && (myIndex()==161 & prevNode == 196)) {
                cout << "old_lambda_1_backward : " << old_lambda_1_backward << endl;
                cout << "old_lambda_2_backward : " << old_lambda_2_backward << endl;
                cout << "new_lambda_1_backward : " << new_lambda_1_backward << endl;
                // cout << "new_lambda_2_backward : " << new_lambda_2_backward << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "dest_amount: " << dest_amount << endl;
                cout << "new cost1_backward: " << cost1_backward << endl;
                cout << "new cost2_backward: " << cost2_backward << endl;
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_backward << endl;
                // cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^not update the balance of this channel from " << old_balance_1 << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
                cout << "lambda_2_calc_by_balance: " << lambda_2_calc_by_balance << endl;
                cout << "cost_2_calc_by_balance: " << cost_2_calc_by_balance << endl;
                cout << "different between the two lambda_2:(+- - bal) " << old_lambda_2_backward - lambda_2_calc_by_balance << endl;
            }
        } // Note: end of if a router (not ack sender) receive a success ack 



        // 1.2.2 Note: Note: Note: after update the previous hop, consider update the next hop's cost
        PaymentChannel *neighbor = &(nodeToPaymentChannel[nextNode]); // Note: get the channel from current node to nextNode
        if (nextNode == receiver){ // if next node is the ack receiver, do not update this hop since this hop is to sender (ack destination) 
            if (_loggingEnabled) {
                cout << "do not update this hop since this hop is to sender (ack destination) !" << endl; 
            }
        }
        else{ // if next node is another router, 
            double flow_amount_forward = _node_txid_flow[myIndex()][transactionId][0]; // get the flow that I recorded previously
            
            if (_loggingEnabled) {
                cout << "-*- ack flow amount forward [0]: " << flow_amount_forward << endl;
            }


        
            old_lambda_1_forward = neighbor->lambda_1;
            old_lambda_2_forward = neighbor->lambda_2;
            new_lambda_1_forward = old_lambda_1_forward - dest_amount / nodeToPaymentChannel[nextNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
            // double new_lambda_2_forward = old_lambda_2_forward; // OnlineScheme: not to update lambda_2

            cost2_1to2 = get_lambda2_cost(old_lambda_2_forward); // cost2
            cost1_forward = get_lambda1_cost(new_lambda_1_forward);


            nodeToPaymentChannel[nextNode].lambda_1 = new_lambda_1_forward;
            // nodeToPaymentChannel[nextNode].lambda_2 = new_lambda_2_forward;
            nodeToPaymentChannel[nextNode].cost_1 = cost1_forward;
            // nodeToPaymentChannel[nextNode].cost_2 = cost2_1to2;
       
            new_unit_cost12_forward = _HTLCtime * cost1_forward + cost2_1to2;
            
            // //Note: for stat
            // cost_traverse_go_through_edges_count++;
            // cost1_total = cost1_total + cost1_forward;
            // cost2_total = cost2_total + cost2_1to2;

            // old_balance_3 = _balances[make_tuple(myIndex(),nextNode)];
            // old_balance_4 = _balances[make_tuple(nextNode,myIndex())];
            // lambda_2_calc_by_balance_forward = get_lambda2_utilization(_balances[make_tuple(myIndex(),nextNode)], old_balance_4);
            // cost_2_calc_by_balance_forward = get_lambda2_cost(lambda_2_calc_by_balance_forward);

            //Note: add frequency check
            if (_updateFrequency == 0){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == nextNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_forward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_forward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_forward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            else{
                if (frequency_counter % _updateFrequency == nodeToPaymentChannel[nextNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == nextNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_forward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_forward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_forward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            }
            

            if (_loggingEnabled) {
                cout << "ACK +++++++++++++++++++++++++++++++++++++++++++++++update the _channel2_copy_otherview! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_forward << endl;
            }

            /* ---------------start of get lambda1 and lambda2, according to equation (6) and (7) in paper
            the value should be the same as update them using algorithm 
            */
            // calc the lambda1 use equation (6) 
            double total_flow_not_acked_on_channel = 0;
            for (auto iter = nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.end(); ++iter) {
                // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
            }
            double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
            double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[nextNode].origTotalCapacity;
            double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
            // update the lambda2 use equation (7)
            lambda_2_calc_by_balance_forward = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),nextNode)], nodeToPaymentChannel[nextNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),nextNode)]);
            cost_2_calc_by_balance_forward = get_lambda2_cost(lambda_2_calc_by_balance_forward);
            
            if (((new_lambda_1_protocol-new_lambda_1_forward) > _eps) || ((lambda_2_calc_by_balance_forward-old_lambda_2_forward) > _eps)) {
                protocol_wrong++;
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime() << " ]" << endl;
                    cout << "lambda update wrong!!!" << endl;
                    cout << std::setprecision (15) << "updated lambda_1 : " << new_lambda_1_forward << endl;
                    cout << std::setprecision (15) << "old lambda_2: " << old_lambda_2_forward << endl;
                    cout << std::setprecision (15) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                    cout << std::setprecision (15) << "clac lambda_2: " << lambda_2_calc_by_balance_forward << endl;
                }
            }
            /* --------------- end of get two lambda update using balance and total flow */

            if (_loggingEnabled && (myIndex()==161 & nextNode == 196)) {
                cout << "old_lambda_1_forward : " << old_lambda_1_forward << endl;
                cout << "old_lambda_2_forward : " << old_lambda_2_forward << endl;
                cout << "new_lambda_1_forward : " << new_lambda_1_forward << endl;
                // cout << "new_lambda_2_backward : " << new_lambda_2_backward << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                cout << "dest_amount: " << dest_amount << endl;
                cout << "new cost1_forward: " << cost1_forward << endl;
                cout << "old cost2_1to2: " << cost2_1to2 << endl;
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_backward << endl;
                // cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^not update the balance of this channel from " << old_balance_3 << "  to   " << _balances[make_tuple(myIndex(),nextNode)] << endl;
                cout << "lambda_2_calc_by_balance: " << lambda_2_calc_by_balance_forward << endl;
                cout << "cost_2_calc_by_balance: " << cost_2_calc_by_balance_forward << endl;
                cout << "different between the two lambda_2:(+- - bal) " << old_lambda_2_forward - lambda_2_calc_by_balance_forward << endl;
            }
        }
        forwardMessage(ttmsg); // if the ack received is success
    }
    else{
        // Note: if it is a failed ack, if the router receive a failed ACK, since this ack is not generate by itself, need to update the backward and forward both
        if (_loggingEnabled) {
            cout << "get a failed ack message!" << endl;
        }

        if (_loggingEnabled) {
            cout << "-*- ack flow amount backward [1]: " << flow_amount << endl;
            cout << "get my _txid_flow: key is " << transactionId << " & value is " << flow_amount << endl;
        }

        if (sender == prevNode){ // if the previous node is ack sender, do not need to update cost and balance
            if (_loggingEnabled) {
                cout << "do not update this hop since this hop is to destination (ack sender) !" << endl; 
            }
        }
        else{ // receive failed ack, if previous node is another router, update the previous hop's cost and balance 
            old_lambda_1_failACK_backward = neighbor_backward->lambda_1; // FIXME: who update the lambda? this function // TODO: need to inital it? yes, in initial() of host and router
            old_lambda_2_failACK_backward = neighbor_backward->lambda_2;
            new_lambda_1_failACK_backward = old_lambda_1_failACK_backward - dest_amount / nodeToPaymentChannel[prevNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
            new_lambda_2_failACK_backward = old_lambda_2_failACK_backward - dest_amount / ((_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity );; // Note: backward -
            
            if (_loggingEnabled) {
                cout << "old lambda 1 backward : " << old_lambda_1_failACK_backward << endl;
                cout << "old lambda 2 backward : " << old_lambda_2_failACK_backward << endl;
                cout << "new lambda 1 backward : " << new_lambda_1_failACK_backward << endl;
                cout << "new lambda 2 backward : " << new_lambda_2_failACK_backward << endl;
            }

            cost1_failACK_backward = get_lambda1_cost(new_lambda_1_failACK_backward);
            cost2_failACK_backward = get_lambda2_cost(new_lambda_2_failACK_backward);

            nodeToPaymentChannel[prevNode].lambda_1 = new_lambda_1_failACK_backward;
            nodeToPaymentChannel[prevNode].lambda_2 = new_lambda_2_failACK_backward;
            nodeToPaymentChannel[prevNode].cost_1 = cost1_failACK_backward;
            nodeToPaymentChannel[prevNode].cost_2 = cost2_failACK_backward;

            new_unit_cost12_failACK_backward = _HTLCtime * cost1_failACK_backward + cost2_failACK_backward;
            
            // //Note: for stat
            // cost_traverse_go_through_edges_count++;
            // cost1_total = cost1_total + cost1_failACK_backward;
            // cost2_total = cost2_total + cost2_failACK_backward;
            
            //Note: add frequency check
            if (_updateFrequency == 0){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            else{
                if (frequency_counter % _updateFrequency == nodeToPaymentChannel[prevNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == prevNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_backward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            }
            

            if (_loggingEnabled) {
                cout << "ACK +++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_backward << endl;
            }


            old_balance_1_failACK = _balances[make_tuple(myIndex(),prevNode)];
            old_balance_2_failACK = _balances[make_tuple(prevNode,myIndex())];
            _balances[make_tuple(myIndex(),prevNode)] = old_balance_1_failACK + flow_amount;
            // _balances[make_tuple(prevNode,myIndex())] = old_balance_2 - flow_amount;

            // new_lambda_2_calc_by_balance_failACK = get_lambda2_utilization(_balances[make_tuple(myIndex(),prevNode)], old_balance_2_failACK);
            // new_cost_2_calc_by_balance_failACK = get_lambda2_cost(new_lambda_2_calc_by_balance_failACK);


            if (_loggingEnabled) {
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_1_failACK << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
            }


            /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
            the value should be the same as update them using algorithm 
            */
            double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),prevNode)]; // from me to previous Node
            _balances_protocol[make_tuple(myIndex(),prevNode)] = old_balance_protocol_1 + dest_amount; // protocol should update the demand amount
            if (_loggingEnabled) {
                cout << "Balance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
            }

            
            // stat the negative
             if (_balances[make_tuple(myIndex(),prevNode)] < 0  ||  _balances[make_tuple(prevNode,myIndex())] <0){
                balance_negative_count++;
             }
            
            // update the lambda1 use equation (6) 
            double total_flow_not_acked_on_channel = 0;
            for (auto iter = nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[prevNode].not_yet_acked_tx_protocol.end(); ++iter) {
                // cout << "<" << iter->first << ", " << iter->second << ">" << endl;
                total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
            }
            double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
            double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[prevNode].origTotalCapacity;
            double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
            // update the lambda2 use equation (7)
            new_lambda_2_calc_by_balance_failACK = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),prevNode)], nodeToPaymentChannel[prevNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),prevNode)]);
            new_cost_2_calc_by_balance_failACK = get_lambda2_cost(new_lambda_2_calc_by_balance_failACK);
            
            if (((new_lambda_1_protocol-new_lambda_1_failACK_backward) > _eps) || ((new_lambda_2_calc_by_balance_failACK-new_lambda_2_failACK_backward) > _eps)) {
                protocol_wrong++;
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                    cout << "lambda update wrong!!!" << endl;
                    cout << std::setprecision (15) << "updated lambda_1 : " << new_lambda_1_failACK_backward << endl;
                    cout << std::setprecision (15) << "updated lambda_2: " << new_lambda_2_failACK_backward << endl;
                    cout << std::setprecision (15) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                    cout << std::setprecision (15) << "clac lambda_2: " << new_lambda_2_calc_by_balance_failACK << endl;
                }
            }
            /* --------------- end of get two lambda update using balance and total flow */
            

            if (_loggingEnabled && (myIndex()==161 & prevNode == 196)) {
                cout << "old_lambda_1_backward : " << old_lambda_1_failACK_backward << endl;
                cout << "old_lambda_2_backward : " << old_lambda_2_failACK_backward << endl;
                cout << "new_lambda_1_backward : " << new_lambda_1_failACK_backward << endl;
                cout << "new_lambda_2_backward : " << new_lambda_2_failACK_backward << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[prevNode].origTotalCapacity << endl;
                cout << "dest_amount: " << dest_amount << endl;
                cout << "flow_amount: (from the record)  " << flow_amount << endl;
                cout << "new cost1_backward: " << cost1_failACK_backward << endl;
                cout << "new cost2_backward: " << cost2_failACK_backward << endl;
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << prevNode << " to " << new_unit_cost12_failACK_backward << endl;
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^not update the balance of this channel from " << old_balance_1_failACK << "  to   " << _balances[make_tuple(myIndex(),prevNode)] << endl;
                cout << "Balance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),prevNode)] << endl;
                cout << "lambda_2_calc_by_balance: " << new_lambda_2_calc_by_balance_failACK << endl;
                cout << "cost_2_calc_by_balance: " << new_cost_2_calc_by_balance_failACK << endl;
                cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_failACK_backward - new_lambda_2_calc_by_balance_failACK << endl;
            }
        }



        // 2.2.2 update the forward hop's cost and balance when failed ack
       PaymentChannel *neighbor = &(nodeToPaymentChannel[nextNode]); // Note: get the channel from current node to nextNode
        if (nextNode == receiver){ // if the nextNode is the ack receiver, do not update
            if (_loggingEnabled) {
                cout << "do not update this hop since this hop is to sender (ack destination) !" << endl; 
            }
        }
        else{ // if the next Node is another router, need to update cost and balance
            if (_loggingEnabled) {
                cout << "-*- ack flow amount forward [0]: " << flow_amount_forward << endl;
            }
            
            old_lambda_1_failACK_forward = neighbor->lambda_1;
            old_lambda_2_failACK_forward = neighbor->lambda_2;
            new_lambda_1_failACK_forward = old_lambda_1_failACK_forward - dest_amount / nodeToPaymentChannel[nextNode].origTotalCapacity; //Question: nodeTOPaymentChannel?? the key is ? <me to the key>
            new_lambda_2_failACK_forward = old_lambda_2_failACK_forward + dest_amount / ((_alphae - _betae) * nodeToPaymentChannel[nextNode].origTotalCapacity ); ; // OnlineScheme: not to update lambda_2
            
            if (_loggingEnabled) {
                cout << "old lambda 1 forward : " << old_lambda_1_failACK_forward << endl;
                cout << "old lambda 2 forward : " << old_lambda_2_failACK_forward << endl;
                cout << " nodeToPaymentChannel[nextNode].origTotalCapacity: " << nodeToPaymentChannel[nextNode].origTotalCapacity<< endl;
                cout << "new lambda 1 forward : " << new_lambda_1_failACK_forward << endl;
                cout << "new lambda 2 forward : " << new_lambda_2_failACK_forward << endl;
                cout << "_HTLCtime : " << _HTLCtime << endl;
            }

            cost2_failACK_forward = get_lambda2_cost(new_lambda_2_failACK_forward); // cost2
            cost1_failACK_forward = get_lambda1_cost(new_lambda_1_failACK_forward); // cost1

            nodeToPaymentChannel[nextNode].lambda_1 = new_lambda_1_failACK_forward;
            nodeToPaymentChannel[nextNode].lambda_2 = new_lambda_2_failACK_forward;
            nodeToPaymentChannel[nextNode].cost_1 = cost1_failACK_forward;
            nodeToPaymentChannel[nextNode].cost_2 = cost2_failACK_forward;

            new_unit_cost12_failACK_forward = _HTLCtime * cost1_failACK_forward + cost2_failACK_forward;
            
            // //Note: for stat
            // cost_traverse_go_through_edges_count++;
            // cost1_total = cost1_total + cost1_failACK_forward;
            // cost2_total = cost2_total + cost2_failACK_forward;

            //Note: add frequency check
            if (_updateFrequency == 0){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == nextNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            else{
                if (frequency_counter % _updateFrequency == nodeToPaymentChannel[nextNode].updateFrequencyEntry){
                for (int i = 0; i < vector_size; i++){
                    if (_channel2[myIndex()][i].first == nextNode){
                        // _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward;
                        if (corruptionStatusList[myIndex()] == 0){
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward;
                        }
                        else{
                            _channel2[myIndex()][i].second = new_unit_cost12_failACK_forward * _corruptionFeeRatio;
                        }
                        break;    // one loop 
                    }
                }
            }
            }
            

            if (_loggingEnabled) {
                cout << "ACK +++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_forward << endl;
            }
            
            old_balance_3_failACK = _balances[make_tuple(myIndex(),nextNode)];
            old_balance_4_failACK = _balances[make_tuple(nextNode,myIndex())];
            _balances[make_tuple(myIndex(),nextNode)] = old_balance_3_failACK - flow_amount_forward;
            
            // new_lambda_2_calc_by_balance_failACK_forward = get_lambda2_utilization(_balances[make_tuple(myIndex(),nextNode)], old_balance_4_failACK);
            // new_cost_2_calc_by_balance_failACK_forward = get_lambda2_cost(new_lambda_2_calc_by_balance_failACK_forward);


            if (_loggingEnabled) {
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_3_failACK << "  to   " << _balances[make_tuple(myIndex(),nextNode)] << endl;
            }

            // stat the negative
             if (_balances[make_tuple(myIndex(),nextNode)] < 0  ||  _balances[make_tuple(nextNode,myIndex())] <0){
                balance_negative_count++;
             }

             /* ---------------start of update _balances_protocol and calculate the updated lambda1 and lambda2, i.e. equation (6) and (7) in paper
            the value should be the same as update them using algorithm 
            */
            double old_balance_protocol_1 = _balances_protocol[make_tuple(myIndex(),nextNode)]; // from me to next Node
            _balances_protocol[make_tuple(myIndex(),nextNode)] = old_balance_protocol_1 - dest_amount; // protocol should update the demand amount
            if (_loggingEnabled) {
                cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),nextNode)] << endl;
            }
            
            // update the lambda1 use equation (6) 
            double total_flow_not_acked_on_channel = 0;
            for (auto iter = nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.begin(); iter != nodeToPaymentChannel[nextNode].not_yet_acked_tx_protocol.end(); ++iter) {
                // cout << "<" << iter->first << ", " << iter->second << ">" << endl; // here is the <1,500> things...
                total_flow_not_acked_on_channel = total_flow_not_acked_on_channel + iter->second;
            }
            double new_total_flight_flow = total_flow_not_acked_on_channel - dest_amount;
            double new_lambda_1_protocol = new_total_flight_flow/nodeToPaymentChannel[nextNode].origTotalCapacity;
            double new_cost_1_protocol = get_lambda1_cost(new_lambda_1_protocol);
            // update the lambda2 use equation (7)
            new_lambda_2_calc_by_balance_failACK_forward = get_lambda2_utilization(_balances_protocol[make_tuple(myIndex(),nextNode)], nodeToPaymentChannel[nextNode].origTotalCapacity-_balances_protocol[make_tuple(myIndex(),nextNode)]);
            new_cost_2_calc_by_balance_failACK_forward = get_lambda2_cost(new_lambda_2_calc_by_balance_failACK_forward);
            
            if (((new_lambda_1_protocol-new_lambda_1_failACK_forward) > _eps) || ((new_lambda_2_calc_by_balance_failACK_forward-new_lambda_2_failACK_forward) > _eps)) {
                protocol_wrong++;
                if (_loggingEnabled) { //level_0
                    cout << "[ Node " << myIndex() << " | " << "Time: " << simTime()  << " ]" << endl;
                    cout << "lambda update wrong!!!" << endl;
                    cout << std::setprecision (15) << "updated lambda_1 : " << new_lambda_1_failACK_forward << endl;
                    cout << std::setprecision (15) << "updated lambda_2: " << new_lambda_2_failACK_forward << endl;
                    cout << std::setprecision (15) << "calc lambda_1: " << new_lambda_1_protocol << endl;
                    cout << std::setprecision (15) << "clac lambda_2: " << new_lambda_2_calc_by_balance_failACK_forward << endl;
                }
            }
            /* --------------- end of get two lambda update using balance and total flow */

            


            if ( _loggingEnabled && (myIndex()==161 & nextNode == 196)) {
                cout << "old_lambda_1_failACK_forward : " << old_lambda_1_failACK_forward << endl;
                cout << "old_lambda_2_failACK_forward : " << old_lambda_2_failACK_forward << endl;
                cout << "new_lambda_1_failACK_forward : " << new_lambda_1_failACK_forward << endl;
                cout << "new_lambda_2_failACK_forward : " << new_lambda_2_failACK_forward << endl;
                cout << "(_alphae - _betae) * nodeToPaymentChannel[prevNode].origTotalCapacity: " << (_alphae - _betae) * nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                cout << "nodeToPaymentChannel[prevNode].origTotalCapacity" << nodeToPaymentChannel[nextNode].origTotalCapacity << endl;
                cout << "flow_amount (dest_amount): " << flow_amount << endl;
                cout << "flow_amount_forward: " << flow_amount_forward << endl;
                cout << "new cost1_failACK_forward: " << cost1_failACK_forward << endl;
                cout << "new cost2_failACK_forward: " << cost2_failACK_forward << endl;
                cout << "+++++++++++++++++++++++++++++++++++++++++++++++update the _channel2! from " << myIndex() << " to " << nextNode << " to " << new_unit_cost12_failACK_forward << endl;
                cout << "Blanceupdate^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^update the balance of this channel from " << old_balance_3_failACK << "  to   " << _balances[make_tuple(myIndex(),nextNode)] << endl;
                cout << "Blance Protocol update^^^^^^^^^^^^^^^^^^^^^^^^^^ ^update the balance of this channel from " << old_balance_protocol_1 << "  to   " << _balances_protocol[make_tuple(myIndex(),nextNode)] << endl;
                cout << "new_lambda_2_calc_by_balance_forward: " << new_lambda_2_calc_by_balance_failACK_forward << endl;
                cout << "new_cost_2_calc_by_balance_forward: " << new_cost_2_calc_by_balance_failACK_forward << endl;
                cout << "different between the two lambda_2:(+- - bal) " << new_lambda_2_failACK_forward - new_lambda_2_calc_by_balance_failACK_forward << endl;
            }
        } // end of update forward hop after recive a failed ack
        forwardMessage(ttmsg); // 2.3 forward this failed ack out
    }
}
