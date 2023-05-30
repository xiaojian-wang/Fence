#include <math.h>
#include <float.h>
#ifndef INITIALIZE_H
#define INITIALIZE_H
#include "hostInitialize.h"


bool probesRecent(unordered_map<int, PathInfo> probes){ // probe Recent? 
    for (auto iter : probes){
        int key = iter.first;
        if ((iter.second).lastUpdated == -1  || ((simTime() - (iter.second).lastUpdated) > _maxTravelTime) )
            return false;
    }
    return true;
}

// TODO: FIXME: we want to generate a global graph with cost property
/* generate_channels_balances_map - reads from file and constructs adjacency list of graph topology (channels), and hash map
 *      of directed edges to initial balances, modifies global maps in place
 *      each line of file is of form
 *      [node1] [node2] [1->2 delay] [2->1 delay] [balance at node1 end] [balance at node2 end]
 */
void generateChannelsBalancesMap(string topologyFile) {
    string line;
    ifstream myfile (topologyFile); // input file stream, open this file
    int lineNum = 0;
    int numEdges = 0;
    double sumDelays = 0.0;
    if (myfile.is_open()) // if the file can be open
    {
        srand(244); //fix seed, so that each channel's entry point is different
        while ( getline (myfile,line) ) // read this file
        {
            lineNum++;
            vector<string> data = split(line, ' '); // line is sperated by space
            // parse all the landmarks from the first line, the first line is landmark line
            if (lineNum == 1) {
                for (auto node : data) { // auto another way to write for loop, select each element in data
                    char nodeType = node.back(); // get the last element in vector
                    int nodeNum = stoi((node).substr(0,node.size()-1)); //get the node number
                    if (nodeType == 'r') {
                        nodeNum = nodeNum + _numHostNodes; // all the number of nodes? : why if the node is a router, the nodenumber should add. since in the topology file, router numbered from 0
                    }
                    _landmarks.push_back(nodeNum); // add element at the end
                    _landmarksWithConnectivityList.push_back(make_tuple(_channels[nodeNum].size(), nodeNum));
                }
                // don't do anything else
                continue;
            }
            // for other lines
            //generate _channels - adjacency map
            char node1type = data[0].back();  // node 1
            char node2type = data[1].back();  // node 2

            // if (_loggingEnabled) {
            //     cout <<"node2type: " << node2type << endl;
            //     cout <<"node1type: " << node1type << endl;
            //     cout << "data size" << data.size() << endl;
            // }

            int node1 = stoi((data[0]).substr(0,data[0].size()-1)); //
            if (node1type == 'r')  // if the node is a router, its index should + number of the host nodes
                node1 = node1 + _numHostNodes;
                node_incomeList[node1] = {}; //Note: to init the income list for a router node

            int node2 = stoi(data[1].substr(0,data[1].size()-1)); //
            if (node2type == 'r')
                node2 = node2 + _numHostNodes;
                node_incomeList[node2] = {}; //Note: to init the income list for a router node


            int delay1to2 = stoi(data[2]);
            int delay2to1 = stoi(data[3]);
            if (_channels.count(node1)==0){ //node 1 is not in map count() return the number of nodes1
                vector<pair<int,int>> tempVector = {};
                tempVector.push_back(make_pair(node2,delay1to2));
                _channels[node1] = tempVector;
            }
            else //(node1 is in map)
                _channels[node1].push_back(make_pair(node2,delay1to2));    

            if (_channels.count(node2)==0){ //node 1 is not in map
                vector<pair<int,int>> tempVector = {make_pair(node1,delay2to1)};
                _channels[node2] = tempVector;
            }
            else //(node2 is in map)
                _channels[node2].push_back(make_pair(node1, delay2to1));

            sumDelays += delay1to2 + delay2to1;
            numEdges += 2;  // record the number of edges



            //generate _balances map
            double balance1 = stod( data[4]); // get the balance left
            double balance2 = stod( data[5]); // right side

            // if (!_capacityScaleEnable){ // Note: we do not need to scale the capacity down here, just in violation count use the new capacity is enough
            _balances[make_tuple(node1,node2)] = balance1; // key is node pair
            _balances[make_tuple(node2,node1)] = balance2;
            //Note: update frequency entry point
            //Note: revise on 2023.4.14, _updateFrequency=0 will cause problem, add if here
            int frequencyEntryPoint = 0;
            if (_updateFrequency !=0){
                frequencyEntryPoint = (rand () % (_updateFrequency))+ 1;
            }

            // int frequencyEntryPoint = (rand () % (_updateFrequency))+ 1;;
            _updateFrequencyEntryList[make_tuple(node1,node2)] = frequencyEntryPoint;
            _updateFrequencyEntryList[make_tuple(node2,node1)] = frequencyEntryPoint;
            // cout << "test the randomness of the frequency entry point: " << frequencyEntryPoint << endl;
            // }
            // else{
            //     _balances[make_tuple(node1,node2)] = balance1/(1-_P); // key is node pair
            //     _balances[make_tuple(node2,node1)] = balance2/(1-_P);
            // }

            //OnlineScheme: 
            if (_onlineSchemeEnabled == true){
                _balances_protocol[make_tuple(node1,node2)] = balance1; // key is node pair
                _balances_protocol[make_tuple(node2,node1)] = balance2;
            }

            tuple<int, int> senderReceiverPair = (node1 < node2) ? make_tuple(node1, node2) :
                make_tuple(node2, node1);
            _capacities[senderReceiverPair] = balance1 + balance2; // record the capacity, only has the <1,2> pair

            // OnlineScheme: init _channel2
            
            if (_loggingEnabled) {
            cout << "in generateChannelsBalancesMap myIndex() = " << endl;
            }
            // cout << "_n=" << _n << endl;
            // cout << "_F1=" << _F1 << endl;
            // cout << "_F2=" << _F2 << endl;
            // cout << "_mu2 = " << _mu2 << endl;
            // cout << "_mu1 = " << _mu1 << endl;
            // cout << "_alphae = " << _alphae << endl;
            // cout << "_betae" << _betae << endl;
            // cout << "_P = " << _P << endl;
            // cout << "_HTLCtime = " << _HTLCtime << endl;
            double cost1to2 = get_lambda2_cost(get_lambda2_utilization(balance1, balance2));
            double cost2to1 = get_lambda2_cost(get_lambda2_utilization(balance2, balance1));
            //printout the init cost
            if (_loggingEnabled) {
            cout << "cost1to2, (balance1, balance2): " << cost1to2 << " (" << balance1<< ", " << balance2 << endl;
            cout << "cost2to1, (balance2, balance1): " << cost2to1 << " (" << balance2<< ", " << balance1<< endl;
            }
            if (_onlineSchemeEnabled == true){
                if (_channel2.count(node1)==0){ //node 1 is not in map count() return the number of nodes1
                vector<pair<int,double>> tempVector = {};
                tempVector.push_back(make_pair(node2,cost1to2));
                _channel2[node1] = tempVector;
                // _channel2[node1][node2].second = cost1to2;
                if (_loggingEnabled) {
                cout << "if from node1: " << node1 << " to node2: " << node2 << " cost init is :" << cost1to2 << endl;
                }
                }
                else{ //(node1 is in map)
                    _channel2[node1].push_back(make_pair(node2,cost1to2));
                    // _channel2[node1][node2].second = cost1to2;
                    if (_loggingEnabled) {
                    cout << "else from node1: " << node1 << " to node2: " << node2 << " cost init is :" << cost1to2 << endl;   
                    } 
                }    
                if (_channel2.count(node2)==0){ //node 1 is not in map
                    vector<pair<int,double>> tempVector = {make_pair(node1,cost2to1)};
                    _channel2[node2] = tempVector;
                    // _channel2[node2][0].second = cost2to1;
                    if (_loggingEnabled) {
                    cout << "if from node2: " << node2 << " to node1: " << node1 << " cost init is :" << cost2to1 << endl;
                    }
                }
                else{ //(node2 is in map)
                    _channel2[node2].push_back(make_pair(node1, cost2to1));
                    // _channel2[node2][node1].second = cost2to1;
                    if (_loggingEnabled) {
                    cout << "else from node2: " << node2 << " to node1: " << node1 << " cost init is :" << cost2to1 << endl;
                    }
                }
            }

            data = split(line, ' ');

        }

        myfile.close();
        _avgDelay = sumDelays/numEdges; // average delat of this graph
        //Note: initial the corruption data structure hardcode the number of routers
        // generate a random list, according to _corruptionRatio
        // random select 128*corruptionRatio nodes to be corrupted, the node index is from 128 to 256
        //init all the node is honest
        for (int i = 128; i <=256; i++){
            corruptionStatusList[i] = 0; // init all the node is honest
        }
        int corrupt_number = (int) (128 * _corruptionRatio);
        //corruption node set
        set<int> corruptedNodes = {};
        while (corruptedNodes.size() < corrupt_number) {
            int nodeIndex = (rand () % (128))+ 128;
            corruptedNodes.insert(nodeIndex);
        } 
        set<int>::iterator it;
        for (it = corruptedNodes.begin(); it != corruptedNodes.end(); it++){
            corruptionStatusList[*it] = 1; //Note: this node is corrupted, others is honest not sure if the it here is correct...
        }  
    }
    else 
        cout << "Unable to open file " << topologyFile << endl;

    // cout<< "into the hostinitialze.cc file" << endl;
    // cout << "finished generateChannelsBalancesMap whose size: " << _capacities.size() << endl;
    
    // cout << "-------------------------check if the returned _channel2 is correct" <<endl;
    // cout << "-------------------------out local view of the graph start" << endl;
    // int channel_number = _channel2.size(); //key size
    // for (int j=0; j < channel_number; j++) {
    //     int vector_size = _channel2[j].size();
        
    //     for (int i = 0; i < vector_size; i++){ 
    //         cout << "from node: " << j << " to node: " << _channel2[j][i].first << " cost is : " << _channel2[j][i].second << endl;
    //     }
    // }
    // cout << "--------------------------- out local view of the graph end" << endl;
    return;
}


/* OnlineScheme: _lambda2_utilization() right side -
*/
/* calculate lambda2 based on balance, i.e. equation (7) in paper */
// balance2 is the right side of the channel, the balance at other side
double get_lambda2_utilization(double balance1, double balance2){
    double lambda2 = (balance2 - _betae * (balance1 + balance2)) / ((_alphae - _betae) * (balance1 + balance2));
    // cout << "init lambda_2:" << lambda2 << endl;
    // cout << "balance1:" << balance1 << endl;
    // cout << "balance2:" << balance2 << endl;
    // cout << "_betae:" << _betae << endl;
    // cout << "_alphae:" << _alphae << endl;
    return lambda2;
}

/* OnlineScheme: _lambda2_cost()
*/
double get_lambda2_cost(double lambda2){
    double cost2;
    double cost2_afterScale;
    if (lambda2 >= 0){
        cost2 = pow(_mu2, lambda2) - 1;
        cost2_afterScale = cost2 * _Z;
    }
    else{
        cost2 = 0;
        cost2_afterScale = cost2 * _Z;
    }
    return cost2_afterScale;
}

// OnlineScheme: Note: Question: lambda_1 related to the time slot, ?? the time slot?
// Note: no one use this function...
double get_lambda1_utilization(double total_flow_amount, double balance1, double balance2){
    double lambda1 = total_flow_amount / (balance1 + balance2);
    cout << "init lambda_1" << lambda1 << endl;
    return lambda1;
}

/* OnlineScheme: _lambda1_cost()
*/
double get_lambda1_cost(double lambda1){
    double cost1 = pow(_mu1, lambda1) - 1;
    double cost1_afterScale = cost1 * _Z;
    return cost1_afterScale;  
}






/* get the number of nodes from topology file
 */
void setNumNodes(string topologyFile){
    int maxHostNode = -1;
    int maxRouterNode = -1;
    string line;
    int lineNum = 0;
    ifstream myfile (topologyFile);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            lineNum++;
            // skip landmark line
            if (lineNum == 1) {
                continue;
            }
            vector<string> data = split(line, ' ');
            //generate channels - adjacency map
            char node1type = data[0].back();
            //  cout <<"node1type: " << node1type << endl;
            char node2type = data[1].back();
            //   cout <<"node2type: " << node2type << endl;

            int node1 = stoi((data[0]).substr(0,data[0].size()-1)); //
            if (node1type == 'r' && node1 > maxRouterNode){
                maxRouterNode = node1;
                //node1 = node1+ _numHostNodes;
            }
            else if (node1type == 'e' && node1 > maxHostNode){
                maxHostNode = node1;
            }

            int node2 = stoi(data[1].substr(0,data[1].size()-1)); //
            if (node2type == 'r' && node2 > maxRouterNode){
                maxRouterNode = node2;
                //node2 = node2 + _numHostNodes;
            }
            else if (node2type == 'e' && node2 > maxHostNode){
                maxHostNode = node2;
            }
        }
        myfile.close();
    }
    else 
        cout << "Unable to open file" << topologyFile << endl;
    _numHostNodes = maxHostNode + 1;
    _numRouterNodes = maxRouterNode + 1;
    _numNodes = _numHostNodes + _numRouterNodes; 
    return;
}

/* read from the workload file
 *  generate_trans_unit_list - reads from file and generates global transaction unit job list.
 *      each line of file is of form:
 *      [amount] [timeSent] [sender] [receiver] [priorityClass]
 * FIXME: [amount] [timeSent] [sender] [receiver] [priorityClass] [timeout_value] 
 */
//Radhika: do we need to pass global variables as arguments?
void generateTransUnitList(string workloadFile){
    // cout << "in generateTransUnitList" << endl;
    string line;
    ifstream myfile (workloadFile);
    double lastTime = -1; 
    int lineNum = 0;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            vector<string> data = split(line, ' ');
            lineNum++;
            // cout<<"line number: " << lineNum << endl;

            //data[0] = amount, data[1] = timeSent, data[2] = sender, data[3] = receiver, data[4] = priority class; (data[5] = time out)
            double amount = stod(data[0]);
            double timeSent = stod(data[1]);
            int sender = stoi(data[2]);
            int receiver = stoi(data[3]);
            int priorityClass = stoi(data[4]);
            // double timeout_value = stod(data[5]); // add by myself do not need...
            double timeOut=-1;
            double largerTxnID = lineNum;
            double hasTimeOut = _timeoutEnabled;
            // cout<<"line number: " << lineNum << endl;

            vector<pair<double,double>> tempVector = {make_pair(0,0)}; //Note: 4.17 init tx amount fee list
            tx_amount_feeList[largerTxnID] = tempVector;
            
            if (timeSent >= _transStatStart && timeSent <= _transStatEnd) {
                if (_transactionArrivalBySize.count(amount) > 0) {
                    _transactionArrivalBySize[amount] += 1;
                }
                else {
                    _transactionCompletionBySize[amount] = 0;
                    _transactionArrivalBySize[amount] = 1;
                    _txnAvgCompTimeBySize[amount] = 0;
                }
            }

            if (data.size()>5 && _timeoutEnabled){
                timeOut = stoi(data[5]);
                // cout << "timeOut: " << timeOut << endl;
            }
            else if (_timeoutEnabled) {
                timeOut = 5.0; //default 5.0
            }

            if (_waterfillingEnabled) { 
                if (timeSent < _waterfillingStartTime || timeSent > _shortestPathEndTime) {
                    continue;
                 }
            }
            else if (_landmarkRoutingEnabled || _lndBaselineEnabled) { 
                if (timeSent < _landmarkRoutingStartTime || timeSent > _shortestPathEndTime) 
                    continue;
            }
            // else if (!_priceSchemeEnabled && !_dctcpEnabled) {// shortest path Note: delete it since the 100000 cannot read? 
            //     if (timeSent < _shortestPathStartTime || timeSent > _shortestPathEndTime) 
            //         continue;
            // }
            
            if (timeSent > lastTime)
                 lastTime = timeSent;
            // instantiate all the transUnits that need to be sent
            //FIXME: in our case, the transaction not to split into multiple MTU
            int numSplits = 0;
            double totalAmount = amount;

            // OnlineScheme: do not to split the amount
            // Transaction tmp_full_tx = Transaction(amount, sender, receiver, hasTimeOut, timeOut);
            // full_tx[sender].push(tmp_full_tx);

            // if splitting enable, will into this while 
            while (amount >= _splitSize && (_waterfillingEnabled || _priceSchemeEnabled || _lndBaselineEnabled || _dctcpEnabled || _celerEnabled) && _splittingEnabled) {
                TransUnit tempTU = TransUnit(_splitSize, timeSent, 
                        sender, receiver, priorityClass, hasTimeOut, timeOut, largerTxnID); //pass all the 
                amount -= _splitSize;
                _transUnitList[sender].push(tempTU);
                numSplits++;
                cout << "split the transaction!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
            }
            // no splitting
            if (amount > 0) { // FIXME: deal with the last tx without enough amount
                TransUnit tempTU = TransUnit(amount, timeSent, sender, receiver, priorityClass, 
                        hasTimeOut, timeOut, largerTxnID);
                _transUnitList[sender].push(tempTU);
                // cout << "a new tx " << largerTxnID << ": from " << sender << " to " << receiver << endl;
                numSplits++;
            }

            // push the transUnit into a priority queue indexed by the sender, Question: why have multiple receivers?
            _destList[sender].insert(receiver);
            
            SplitState temp = {};
            temp.numTotal = numSplits;
            temp.numReceived = 0;
            temp.numArrived = 0;
            temp.numAttempted = 0;
            temp.totalAmount = totalAmount; 
            temp.firstAttemptTime = -1;
            _numSplits[sender][largerTxnID] = temp;

        }
        cout << "finished generateTransUnitList" << endl;
        cout << "generated the transaction list without splitting too splitting no enable" << endl;

        myfile.close();
        /*if (lastTime + 5 < _simulationLength) {
            cout << "Insufficient txns" << endl;
            assert(false);
        }*/
    }
    else 
        cout << "Unable to open file" << workloadFile << endl;
    return;
}


/* updateMaxTravelTime - calculate max travel time, called on each new route, and updates _maxTravelTime value
 */
void updateMaxTravelTime(vector<int> route){
    int nextNode;
    vector<pair<int,int>>* channel; 
    double maxTime = 0;

    for (int i=0; i< ( route.size()-1) ; i++){
        //find the propogation delay per edge of route
        //Radhika TODO: might be better to store channel map indexed using both nodes. check if it breaks anything.
        channel = &(_channels[route[i]]);
        nextNode = route[i+1];

        auto it = find_if( (*channel).begin(), (*channel).end(),
                [&nextNode](const pair<int, int>& element){ return element.first == nextNode;} );
        if (it != (*channel).end()){
            double deltaTime = it->second;
            if (deltaTime > _maxOneHopDelay)
                _maxOneHopDelay = deltaTime/1000;
            maxTime = maxTime + deltaTime;
        }
        else{
            cout << "ERROR IN updateMaxTravelTime, could not find edge" << endl;
            cout << "node to node " << route[i] << "," << route[i+1] << endl;
        }
    }
    maxTime = (maxTime)/1000 *2; //double for round trip, and convert from milliseconds to seconds
    if (maxTime > _maxTravelTime){
        _maxTravelTime = maxTime;
    }
    _delta = _maxTravelTime;
    return;
}


/*get_route- take in sender and receiver graph indicies, and returns
 *  BFS shortest path from sender to receiver in form of node indicies,
 *  includes sender and reciever as first and last entry
 */
// Note: get one path
vector<int> getRoute(int sender, int receiver){
    vector<int> route = dijkstraInputGraph(sender, receiver, _channels);
    updateMaxTravelTime(route);
    return route;
}

vector<int> getRoute2(int sender, int receiver){
    vector<int> route = dijkstraInputGraph(sender, receiver, _channel2);
    updateMaxTravelTime(route);
    return route;
}

//OnlineScheme: 
vector<int> getKShortestRoutes2(int sender, int receiver){
    vector<int> route = dijkstraInputGraph(sender, receiver, _channel2);
    updateMaxTravelTime(route);
    return route;
}

double bottleneckOnPath(vector<int> route) {
   double min = 10000000;
    // ignore end hosts
    for (int i = 1; i < route.size() - 2; i++) {
        double cap = _balances[make_tuple(i, i + 1)] + _balances[make_tuple(i+1, i)];
        if (cap < min)
            min = cap;
    }
    return min; // ?
}

/* find the bottleneck "capacity" on the path
 * so that windows are not allowed to grow larger than them 
 */
double bottleneckCapacityOnPath(vector<int> route) {
   double min = 10000000;
    // ignore end hosts
    for (int i = 1; i < route.size() - 2; i++) {
        int thisNode = route[i];
        int nextNode = route[i + 1];
        tuple<int, int> senderReceiverTuple = (thisNode < nextNode) ? make_tuple(thisNode, nextNode) :
                make_tuple(nextNode, thisNode);
        double cap = _capacities[senderReceiverTuple];
        if (cap < min)
            min = cap;
    }
    return min;
}

void updateCannonicalRTT(vector<int> route) {
        // update cannonical RTT
        double sumRTT = (route.size() - 1) * 2 * _avgDelay / 1000.0;
        sumRTT += _cannonicalRTT * _totalPaths;
        _totalPaths += 1;
        _cannonicalRTT = sumRTT / _totalPaths;
}

// Note: k=1 - with only one path
vector<vector<int>> getKShortestRoutes(int sender, int receiver, int k){
    //do searching without regard for channel capacities, DFS right now
    if (_loggingEnabled) {
        printf("sender: %i; receiver: %i \n ", sender, receiver);
        cout<<endl;
    }
    vector<vector<int>> shortestRoutes = {};
    vector<int> route;
    auto tempChannels = _channels;
    for ( int it = 0; it < k; it++ ){
        route = dijkstraInputGraph(sender, receiver, tempChannels);
        
        if (route.size() <= 1){
            return shortestRoutes;
        }
        else{
            updateMaxTravelTime(route);
            updateCannonicalRTT(route);
            shortestRoutes.push_back(route);
        }
        if (_loggingEnabled) {
            cout << "getKShortestRoutes 1" <<endl;
            cout << "route size: " << route.size() << endl;
            cout << "getKShortestRoutes 2" <<endl;
        }
        tempChannels = removeRoute(tempChannels,route);
        if (_loggingEnabled) {
        cout << "Route Num " << it + 1 << " " << " ";
        
        printVector(route);
        }

    }
    if (_loggingEnabled)
        cout << "Number of Routes between " << sender << " and " << receiver << " is " << shortestRoutes.size() << endl;
    return shortestRoutes;
}


// OnlineScheme: use cost as weight
vector<vector<int>> getKShortestRoutes2(int sender, int receiver, int k){ // cost and corresponding path
    //do searching without regard for channel capacities, DFS right now
    if (_loggingEnabled) {
        printf("sender: %i; receiver: %i  in getKShortestRoutes2 \n", sender, receiver);
        cout<<endl;
    }
    vector<vector<int>> shortestRoutes = {};
    vector<int> route;
    unordered_map<int, vector<pair<int,double>>> tempChannels = _channel2;
    // cout << "getKShortestRoutes2 error??? no" << endl;
    if (_loggingEnabled) {
    cout << "k in getKShortestRoutes2 is: " << k << endl;
    }
    for ( int it = 0; it < k; it++ ){
        if (_loggingEnabled) {
        cout << "getKShortestRoutes2 before dijkstraInputGraph" << endl;
        }
        route = dijkstraInputGraph(sender, receiver, tempChannels);
        if (_loggingEnabled) {
        cout << "getKShortestRoutes2 route from dijkstraInputGraph" << endl;
        }
        
        if (route.size() <= 1){
            return shortestRoutes;
        }
        else{
            updateMaxTravelTime(route);
            updateCannonicalRTT(route);
            shortestRoutes.push_back(route);
        }
        if (_loggingEnabled) {
            cout << "getKShortestRoutes2 1" <<endl;
            cout << "route size: " << route.size() << endl;
            cout << "getKShortestRoutes2 2" <<endl;
        }
        tempChannels = removeRoute(tempChannels,route);
        if (_loggingEnabled) {
        cout << "Route Num " << it + 1 << " " << ": ";
        
        printVector(route);
        }

    }
    if (_loggingEnabled)
        cout << "getKShortestRoutes2 Number of Routes between " << sender << " and " << receiver << " is " << route.size() << endl;
    return shortestRoutes;
}






void initializePathMaps(string filename) {
    string line;
    int lineNum = 0;
    ifstream myfile (filename);
    if (myfile.is_open())
    {
        int sender = -1;
        int receiver = -1;
        vector<vector<int>> pathList;
        while ( getline (myfile,line) ) 
        {
            vector<string> data = split(line, ' ');
            lineNum++;
            if (data[0].compare("pair") == 0) {
                if (lineNum > 1) {
                    _pathsMap[sender][receiver] = pathList;
                    // cout << data[0] <<  " " << data[1] << endl;
                }
                sender = stoi(data[1]);
                receiver = stoi(data[2]);
                pathList.clear();
                //cout << " sender " << sender << " receiver " << receiver << endl;
            }
            else {
                vector<int> newPath;
                for (string d : data) {
                    newPath.push_back(stoi(d));
                }
                pathList.push_back(newPath);
                if (_loggingEnabled) 
                    printVector(newPath);
            }
        }
        
        // push the last path in
        _pathsMap[sender][receiver] = pathList;
    }
    else {
        cout << "unable to open paths file " << filename << endl;
    }
}


vector<vector<int>> getKPaths(int sender, int receiver, int k) {
    if (!_widestPathsEnabled && !_kspYenEnabled && !_obliviousRoutingEnabled && !_heuristicPathsEnabled) 
        return getKShortestRoutes(sender, receiver, k);

    if (_pathsMap.empty()) {
        cout << "path Map uninitialized" << endl;
        throw std::exception();
    }

    if (_pathsMap.count(sender) == 0) {
        cout << " sender " << sender << " has no paths at all " << endl;
        throw std::exception();
    }

    if (_pathsMap[sender].count(receiver) == 0) {
        cout << " sender " << sender << " has no paths to receiver " << receiver << endl;
        throw std::exception();
    }
    
    vector<vector<int>> finalPaths;
    int numPaths = 0;
    double sumRTT = 0;
    for (vector<int> path : _pathsMap[sender][receiver]) {
        if (numPaths >= k)
            break;
        numPaths++;
        finalPaths.push_back(path);
        updateMaxTravelTime(path);
        updateCannonicalRTT(path);
    }

    return finalPaths;
}


// OnlineScheme: 
vector<vector<int>> getKPaths2(int sender, int receiver, int k) {
    // if (!_widestPathsEnabled && !_kspYenEnabled && !_obliviousRoutingEnabled && !_heuristicPathsEnabled) 
    return getKShortestRoutes2(sender, receiver, k);

    // if (_pathsMap.empty()) {
    //     cout << "path Map uninitialized" << endl;
    //     throw std::exception();
    // }

    // if (_pathsMap.count(sender) == 0) {
    //     cout << " sender " << sender << " has no paths at all " << endl;
    //     throw std::exception();
    // }

    // if (_pathsMap[sender].count(receiver) == 0) {
    //     cout << " sender " << sender << " has no paths to receiver " << receiver << endl;
    //     throw std::exception();
    // }
    
    // vector<vector<int>> finalPaths;
    // int numPaths = 0;
    // double sumRTT = 0;
    // for (vector<int> path : _pathsMap[sender][receiver]) {
    //     if (numPaths >= k)
    //         break;
    //     numPaths++;
    //     finalPaths.push_back(path);
    //     updateMaxTravelTime(path);
    //     updateCannonicalRTT(path);
    // }

    // return finalPaths;
}

// get the next path after the kth one when changing paths
tuple<int, vector<int>> getNextPath(int sender, int receiver, int k) {
    if (!_widestPathsEnabled && !_kspYenEnabled && !_obliviousRoutingEnabled && !_heuristicPathsEnabled) {
        cout << "no path Map" << endl;
        throw std::exception();
    }

    if (_pathsMap.empty()) {
        cout << "path Map uninitialized" << endl;
        throw std::exception();
    }

    if (_pathsMap.count(sender) == 0) {
        cout << " sender " << sender << " has no paths at all " << endl;
        throw std::exception();
    }

    if (_pathsMap[sender].count(receiver) == 0) {
        cout << " sender " << sender << " has no paths to receiver " << receiver << endl;
        throw std::exception();
    }
        
    if (_pathsMap[sender][receiver].size() >= k + 2) 
        return make_tuple(k + 1, _pathsMap[sender][receiver][k + 1]);
    else 
        return make_tuple(0, _pathsMap[sender][receiver][0]);
}

bool vectorContains(vector<int> smallVector, vector<vector<int>> bigVector) {
    for (auto v : bigVector) {
        if (v == smallVector)
            return true;
    }
    return false;
}

vector<vector<int>> getKShortestRoutesLandmarkRouting(int sender, int receiver, int k){
    int landmark;
    vector<int> pathSenderToLandmark;
    vector<int> pathLandmarkToReceiver;
    vector<vector<int>> kRoutes = {};
    int numPaths = minInt(_landmarksWithConnectivityList.size(), k);
    for (int i=0; i< numPaths; i++){
        landmark = get<1>(_landmarksWithConnectivityList[i]);
        pathSenderToLandmark = breadthFirstSearch(sender, landmark); //use breadth first search
        pathLandmarkToReceiver = breadthFirstSearch(landmark, receiver); //use breadth first search
			
	pathSenderToLandmark.insert(pathSenderToLandmark.end(), 
                pathLandmarkToReceiver.begin() + 1, pathLandmarkToReceiver.end());
        if ((pathSenderToLandmark.size() < 2 ||  pathLandmarkToReceiver.size() < 2 || 
                    vectorContains(pathSenderToLandmark, kRoutes))) { 
            if (numPaths < _landmarksWithConnectivityList.size()) {
                numPaths++;
            }
        } else {
            kRoutes.push_back(pathSenderToLandmark);
        } 
    }
    return kRoutes;
}



vector<int> breadthFirstSearchByGraph(int sender, int receiver, unordered_map<int, set<int>> graph){
    //TODO: fix, and add to header
    deque<vector<int>> nodesToVisit = {};
    bool visitedNodes[_numNodes];
    for (int i=0; i<_numNodes; i++){
        visitedNodes[i] =false;
    }
    visitedNodes[sender] = true;

    vector<int> temp;
    temp.push_back(sender);
    nodesToVisit.push_back(temp);

    while ((int) nodesToVisit.size() > 0){
        vector<int> current = nodesToVisit[0];
        nodesToVisit.pop_front();
        int lastNode = current.back();
        for (auto iter = graph[lastNode].begin(); iter != graph[lastNode].end(); iter++){
            int thisNode = *iter;
            if (!visitedNodes[thisNode]){
                temp = current; // assignment copies in case of vector
                temp.push_back(thisNode);
                nodesToVisit.push_back(temp);
                visitedNodes[thisNode] = true;
                if (thisNode == receiver)
                    return temp;
            } 
        }
    }
    vector<int> empty = {};
    return empty;
}


vector<int> breadthFirstSearch(int sender, int receiver){
    deque<vector<int>> nodesToVisit;
    bool visitedNodes[_numNodes];
    for (int i=0; i<_numNodes; i++){
        visitedNodes[i] =false;
    }
    visitedNodes[sender] = true;

    vector<int> temp;
    temp.push_back(sender);
    nodesToVisit.push_back(temp);

    while ((int)nodesToVisit.size()>0){
        vector<int> current = nodesToVisit[0];
        nodesToVisit.pop_front();
        int lastNode = current.back();
        for (int i=0; i<(int)_channels[lastNode].size();i++){
            if (!visitedNodes[_channels[lastNode][i].first]){
                temp = current; // assignment copies in case of vector
                temp.push_back(_channels[lastNode][i].first);
                nodesToVisit.push_back(temp);
                visitedNodes[_channels[lastNode][i].first] = true;
                if (_channels[lastNode][i].first==receiver)
                    return temp;
            } //end if (!visitedNodes[_channels[lastNode][i]])
        }//end for (i)
    }//end while
    vector<int> empty = {};
    return empty;
}

template <class T,class S> struct pair_equal_to : binary_function <T,pair<T,S>,bool> {
    bool operator() (const T& y, const pair<T,S>& x) const
    {
        return x.first==y;
    }
};

/* removeRoute - function used to remove paths found to get k shortest disjoint paths
 */
unordered_map<int, vector<pair<int,int>>> removeRoute( unordered_map<int, vector<pair<int,int>>> channels, vector<int> route){
    int start, end;
    for (int i=0; i< (route.size() -1); i++){
        start = route[i];
        end = route[i+1];
        //only erase if edge is between two router nodes
        if (start >= _numHostNodes && end >= _numHostNodes ) {
            vector< pair <int, int> >::iterator it = find_if(channels[start].begin(),channels[start].end(),
                    bind1st(pair_equal_to<int,int>(),end));
            channels[start].erase(it);
        }
    }
    return channels;
}

//OnlineScheme: 
unordered_map<int, vector<pair<int,double>>> removeRoute( unordered_map<int, vector<pair<int,double>>> channels, vector<int> route){
    int start, end;
    for (int i=0; i< (route.size() -1); i++){
        start = route[i];
        end = route[i+1];
        //only erase if edge is between two router nodes
        if (start >= _numHostNodes && end >= _numHostNodes) {
            vector< pair <int, double> >::iterator it = find_if(channels[start].begin(),channels[start].end(),
                    bind1st(pair_equal_to<int,double>(),end));
            channels[start].erase(it);
        }
    }
    return channels;
}

int minInt(int x, int y){
    if (x< y) return x;
    return y;
}
/* split - same as split function in python, convert string into vector of strings using delimiter
 */
vector<string> split(string str, char delimiter){
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;
    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}


/*  A utility function to find the vertex with minimum distance
 * value, from the set of vertices not yet included in shortest path tree
 */
//FIXME: int dist?? not correct
int minDistance(int dist[], 
        bool sptSet[])
{

    // Initialize min value
    int min = INT_MAX;
    int min_index = -1;

    for (int v = 0; v < _numNodes; v++)
        if (sptSet[v] == false &&
                dist[v] <= min){
                    // cout << "[v] is : " << v << " ;   dist[v] is : " << dist[v] << " ;   min is : " << min << endl; 
                    min = dist[v], min_index = v;
                }
            

    if (min == INT_MAX){
        cout << "min == INT_MAX" << endl;
        return -1;
    }
    else{
        return min_index;
    }
}



//FIXME: int dist?? not correct
int minDistance(double dist[], 
        bool sptSet[])
{
    if (_loggingEnabled) {
    // cout << "into the minDistance - double" << endl;
    }
    // Initialize min value
    double min = DBL_MAX;
    int min_index = -1;

    for (int v = 0; v < _numNodes; v++)
    {
        //  if (_loggingEnabled) {
        // cout << "[v] is : " << v << endl;
        // cout << "sptSet[v] is " << sptSet[v] << endl;
        // cout << "dist[v] is " << dist[v] << endl;  
        //  }
        if (sptSet[v] == false &&
                dist[v] <= min){
                    if (_loggingEnabled) {
                    // cout << "[v] is : " << v << " ;   dist[v] is : " << dist[v] << " ;   min is : " << min << endl; 
                    }
                    min = dist[v], min_index = v;
                }
    }
            
    const double eps = 1e-8;

    if (abs(min-DBL_MAX) <= eps){
        cout << "min == DBL_MAX" << endl;
        return -1;
    }
    else{
        if (_loggingEnabled) {
        // cout << "min_index is : " << min_index << endl;
        }
        return min_index;
    }
}

// Function to print shortest
// path from source to j
// using parent array
void printPath(int parent[], int j)
{

    // Base Case : If j is source
    if (parent[j] == - 1)
        return;

    printPath(parent, parent[j]);

    printf("%d ", j);
}


vector<int> getPath(int parent[], int j)
{
    vector<int> result = {};
    // Base Case : If j is source
    if (parent[j] == - 1){
        result.push_back(j);
        return result;
    }
    else if (j == -2){
        vector<int> empty = {};
        return empty;

    }

    result = getPath(parent, parent[j]);
    result.push_back(j);
    return result;
}

// A utility function to print
// the constructed distance
// array
void printSolution(int dist[], int source,
        int parent[])
{
    int src = source;
    printf("Vertex\t Distance\tPath");
    for (int i = 0; i < _numNodes; i++)
    {
        printf("\n%d -> %d \t\t %d\t\t%d ",
                src, i, dist[i], src);
        printPath(parent, i);

        printf("\n getResultSolution: \n");
        vector<int> resultVect = getPath(parent, i);
        for (int i =0; i<resultVect.size(); i++){
            printf("%i, ", resultVect[i]);
        }
    }
    cout << "end print solution " << endl;
}

// Note: the weight used in this function is delay, 

vector<int> dijkstraInputGraph(int src,  int dest, unordered_map<int, vector<pair<int,int>>> channels){
    // The output array. dist[i] will hold the shortest distance from src to i
    int dist[_numNodes];

    // sptSet[i] will true if vertex i is included / in shortest path tree or shortest distance from src to i is finalized
    bool sptSet[_numNodes];

    // Parent array to store shortest path tree
    int parent[_numNodes];

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < _numNodes; i++)
    {
        dist[i] = INT_MAX;
        parent[i] = -2;
        sptSet[i] = false;
    }

    // Parent of source is -1 (used for identifying source later) 
    parent[src] = -1;
    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < _numNodes - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not yet processed.
        // u is always equal to src in first iteration.
        int u = minDistance(dist, sptSet);
        if (u == -1){
            vector<int> empty = {};
            return empty;
        }

        // Mark the picked vertex as processed
        sptSet[u] = true;

        vector<pair<int,int>>::iterator vectIter;
        // Update dist value of the adjacent vertices of the picked vertex.
        for (vectIter = channels[u].begin(); vectIter != channels[u].end(); vectIter++){

            // Update dist[v] only if is not in sptSet, there is an edge from u to v, and
            // total weight of path from src to v through u is smaller than current value of dist[v]
            if (!sptSet[vectIter->first]){
                if(dist[u] + (vectIter->second) < dist[vectIter->first]){
                    parent[vectIter->first] = u;
                    dist[vectIter->first] = dist[u] + vectIter->second;

                }
            }
        }
    }
    return getPath(parent, dest);
}

//OnlineScheme: 
vector<int> dijkstraInputGraph(int src,  int dest, unordered_map<int, vector<pair<int,double>>> channels){
    if (_loggingEnabled) {
    cout << "in dijkstraInputGraph funcition - double" << endl;
    cout << "source : " << src << "   dest : " << dest << endl;
    cout << "channel size: " << channels.size() << endl;
    }
    // The output array. dist[i] will hold the shortest distance from src to i
    double dist[_numNodes];

    // sptSet[i] will true if vertex i is included / in shortest path tree or shortest distance from src to i is finalized
    bool sptSet[_numNodes];

    // Parent array to store shortest path tree
    int parent[_numNodes];

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < _numNodes; i++)
    {
        dist[i] = DBL_MAX; // Note: the input init as infinity
        parent[i] = -2;
        sptSet[i] = false;
    }

    // Parent of source is -1 (used for identifying source later) 
    parent[src] = -1;
    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < _numNodes - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not yet processed.
        // u is always equal to src in first iteration.
        if (_loggingEnabled) {
        // cout << "consider the " << count << " th iteration: " << endl;
        }
        int u = minDistance(dist, sptSet);
        if (u == -1){
            if (!_loggingEnabled) {
            // cout << "in dijkstraInputGraph override function, u == -1, return empty" << endl;
            }
            vector<int> empty = {};
            return empty;
        }

        // Mark the picked vertex as processed
        sptSet[u] = true;

        vector<pair<int,double>>::iterator vectIter;
        // Update dist value of the adjacent vertices of the picked vertex.
        for (vectIter = channels[u].begin(); vectIter != channels[u].end(); vectIter++){

            // Update dist[v] only if is not in sptSet, there is an edge from u to v, and
            // total weight of path from src to v through u is smaller than current value of dist[v]
            if (!sptSet[vectIter->first]){
                if(dist[u] + (vectIter->second) < dist[vectIter->first]){
                    parent[vectIter->first] = u;
                    dist[vectIter->first] = dist[u] + vectIter->second;
                    if (_loggingEnabled) {
                    // cout << "vectIter->second : " << vectIter->second << endl;
                    // cout << "dist[vectIter->first] " << dist[vectIter->first] << endl;
                    }

                }
            }
        }
    }
    if (_loggingEnabled) {
    cout << "end dijkstraInputGraph funcition - double" << endl;
    }
    return getPath(parent, dest);
}


void dijkstraInputGraphTemp(int src,  int dest, unordered_map<int, vector<pair<int,int>>> channels){
    // The output array. dist[i] will hold the shortest distance from src to i
    int dist[_numNodes];

    // sptSet[i] will true if vertex i is included / in shortest path tree or shortest distance from src to i is finalized
    bool sptSet[_numNodes];

    // Parent array to store shortest path tree
    int parent[_numNodes];

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < _numNodes; i++)
    {
        parent[src] = -1;
        parent[i] = -2;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < _numNodes - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not yet processed.
        // u is always equal to src in first iteration.
        int u = minDistance(dist, sptSet);
        if (u==-1){
            vector<int> empty = {};
            return ;

        }

        // Mark the picked vertex as processed
        sptSet[u] = true;

        vector<pair<int,int>>::iterator vectIter;
        // Update dist value of the adjacent vertices of the picked vertex.
        for (vectIter = channels[u].begin(); vectIter != channels[u].end(); vectIter++){

            /*for (int ka=0; ka<_numNodes; ka++){
                printf("[%i]: %i,  ", ka, parent[ka] );

            }*/
            // Update dist[v] only if is not in sptSet, there is an edge from u to v, and
            // total weight of path from src to v through u is smaller than current value of dist[v]

            // find first element with first == 42
            if (!sptSet[vectIter->first]){
                if(dist[u] + (vectIter->second) < dist[vectIter->first]){
                    parent[vectIter->first] = u;
                    dist[vectIter->first] = dist[u] + vectIter->second;

                }
            }
        }
    }

    // print the constructed
    // distance array
    /*for (int ka=0; ka<_numNodes; ka++)
        printf("[%i]: %i,  ", ka, parent[ka] );*/
    return;// getPath(parent, dest);
}
// Function that implements Dijkstra's  single source shortest path algorithm for a graph represented
// using adjacency matrix representation
//TODO: FIXME: the weight this dijkstra algotithm use?
// Note: this function no one use.... they all use the function dijkstraInputGraph function
vector<int> dijkstra(int src,  int dest)
{

    // The output array. dist[i] will hold the shortest distance from src to i
    int dist[_numNodes];

    // sptSet[i] will true if vertex i is included / in shortest path tree or shortest distance from src to i is finalized
    bool sptSet[_numNodes];

    // Parent array to store shortest path tree
    int parent[_numNodes];

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < _numNodes; i++)
    {
        parent[src] = -1;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < _numNodes - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not yet processed.
        // u is always equal to src in first iteration.
        int u = minDistance(dist, sptSet);

        // Mark the picked vertex as processed
        sptSet[u] = true;

        vector<pair<int,int>>::iterator vectIter;
        // Update dist value of the adjacent vertices of the picked vertex.
        for (vectIter = _channels[u].begin(); vectIter != _channels[u].end(); vectIter++){

            // Update dist[v] only if is not in sptSet, there is an edge from u to v, and
            // total weight of path from src to v through u is smaller than current value of dist[v]

            // find first element with first == 42
            //= find_if(channels[u].begin(),channels[u].end(), CompareFirst(v));
            if (!sptSet[vectIter->first]){ // the first node of this channel?
                //if (vectIter != channels[u].end() ){
                if(dist[u] + (vectIter->second) < dist[vectIter->first]){
                    parent[vectIter->first] = u;
                    dist[vectIter->first] = dist[u] + vectIter->second; // Note: the second value in the pair is weight
                    //  }

            }
            }
        }
    }
    return getPath(parent, dest);
}

bool sortHighToLowConnectivity(tuple<int,int> x, tuple<int,int> y){
    if (get<0>(x) > get<0>(y)) 
        return true;
    else if (get<0>(x) < get<0>(y)) 
        return false;
    else
        return get<1>(x) < get<1>(y);
}

/*
 * sortFunction - helper function used to sort queued transUnit list by ascending priorityClass, then by
 *      ascending amount
 *      note: largest element gets accessed first
 */
bool sortPriorityThenAmtFunction(const tuple<int,double, routerMsg*, Id> &a,
        const tuple<int,double, routerMsg*, Id> &b)
{
    if (get<0>(a) < get<0>(b)){
        return false;
    }
    else if (get<0>(a) == get<0>(b)){
        return (get<1>(a) > get<1>(b));
    }
    return true;
}


double minVectorElemDouble(vector<double> v){
    double min = v[0];
    for (double d: v){
        if (d < min)
            min=d;
    }
    return min;
}



double maxDouble(double x, double y){
    if (x>y) return x;
    return y;
}


void printChannels(){
    printf("print of channels\n" );
    for (auto i : _channels){
        printf("key: %d [",i.first);
        for (auto k: i.second){
            printf("(%d, %d) ",get<0>(k), get<1>(k));
        }
        printf("] \n");
    }
    cout<<endl;
}


void printVector(vector<int> v){
    for (auto temp : v) {
        cout << temp << ", ";
    }
    cout << endl;
}

void printVectorReverse(vector<int> v){
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        cout << *it << ", ";
    }
    cout << endl;
}



/*
 * sortFunction - helper function used to sort queued transUnit list by ascending priorityClass, then by
 *      ascending amount
 *      note: largest element gets accessed first
 */
bool sortPriorityThenAmtFunction(const tuple<int,double, routerMsg*, Id, simtime_t> &a,
      const tuple<int,double, routerMsg*, Id, simtime_t> &b)
{
   if (get<0>(a) < get<0>(b)){
      return false;
   }
   else if (get<0>(a) == get<0>(b)){
      return (get<1>(a) > get<1>(b));
   }
   return true;
}



/*
 * sortFunction - to do FIFO sorting 
 */
bool sortFIFO(const tuple<int,double, routerMsg*, Id, simtime_t> &a,
      const tuple<int,double, routerMsg*, Id, simtime_t> &b)
{
    return (get<4>(a).dbl() < get<4>(b).dbl());
}

/*
 * sortFunction - to do LIFO sorting 
 */
bool sortLIFO(const tuple<int,double, routerMsg*, Id, simtime_t> &a,
      const tuple<int,double, routerMsg*, Id, simtime_t> &b)
{
    return (get<4>(a).dbl() > get<4>(b).dbl());
}

/*
 * sortFunction - to do shortest payment first sorting 
 */
bool sortSPF(const tuple<int,double, routerMsg*, Id, simtime_t> &a,
      const tuple<int,double, routerMsg*, Id, simtime_t> &b)
{
    transactionMsg *transA = check_and_cast<transactionMsg *>((get<2>(a))->getEncapsulatedPacket());
    transactionMsg *transB = check_and_cast<transactionMsg *>((get<2>(b))->getEncapsulatedPacket());
    
    SplitState splitInfoA = _numSplits[transA->getSender()][transA->getLargerTxnId()];
    SplitState splitInfoB = _numSplits[transB->getSender()][transB->getLargerTxnId()];

    if (splitInfoA.totalAmount != splitInfoB.totalAmount)
        return splitInfoA.totalAmount < splitInfoB.totalAmount;
    else
        return (get<4>(a).dbl() > get<4>(b).dbl());
}

/*
 * sortFunction - to do earliest deadline first sorting 
 */
bool sortEDF(const tuple<int,double, routerMsg*, Id, simtime_t> &a,
      const tuple<int,double, routerMsg*, Id, simtime_t> &b)
{
    transactionMsg *transA = check_and_cast<transactionMsg *>((get<2>(a))->getEncapsulatedPacket());
    transactionMsg *transB = check_and_cast<transactionMsg *>((get<2>(b))->getEncapsulatedPacket());
    
    return (transA->getTimeSent() < transB->getTimeSent());
}

#endif
