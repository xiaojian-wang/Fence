import sys
import textwrap
import argparse
import networkx as nx
from config import *
import re
import os
import math
import random
import numpy as np
from matplotlib import pyplot

# eg. 0r: the router number 0
def parse_node_name(node_name, max_router, max_host):
    try:
        val = int(node_name[:-1]) # omit the last character r
        if(node_name[-1] == 'r'):
            if(val > max_router):
                max_router = val # x-th router
            return ("router[" + str(val) + "]", max_router, max_host)
        if(node_name[-1] == 'e'):
            if(val > max_host):
                max_host = val
            return ("host[" + str(val) + "]", max_router, max_host)
        return -1
    except:
        return -1



# take the topology file in a specific format and write it to a ned file
def write_ned_file(number_of_nodes, topo_filename, output_filename, network_name, routing_alg):
    # topo_filename must be a text file where each line contains the ids of two neighbouring nodes that 
    # have a payment channel between them, relative delays in each direction,  initial balance on each 
    # end (see sample-topology.txt)
    # each line is of form:
    # [node1] [node2] [1->2 delay] [2->1 delay] [balance @ 1] [balance @ 2]
    # Note: the ned file seems not include the balance infomation? yes
    topo_file = open(topo_filename).readlines() 
    outfile = open(output_filename, "w")

    # metadata used for forwarding table
    neighbor_interfaces = dict()
    node_interface_count = dict()
    node_used_interface = dict()
    linklist = list()
    max_val = -1 #used to find number of nodes, assume nodes start at 0 and number consecutively Note: which is not true in our cases
    max_router = -1
    max_host = -1
    line_num = 0
    for line in topo_file:
        line_num += 1

        # landmark line Note: the first line is the landmark line, and skip this line
        if line_num == 1: 
            continue

        if line == "\n":
            continue

        n1 = parse_node_name(line.split()[0], max_router, max_host)  # Note: 0r 1r 120 120 50 51
        if(n1 == -1):
            print("Bad line " + line)
            continue
        max_router = n1[1]
        max_host = n1[2]

        n2 = parse_node_name(line.split()[1], max_router, max_host)
        if(n2 == -1):
            print("Bad line " + line)
            continue
        max_router = n2[1]
        max_host = n2[2]

        n3 = float(line.split()[2]) # delay going from n1 to n2
        n4 = float(line.split()[3]) # delay going from n2 to n1

        linklist.append((n1[0], n2[0], n3, n4)) # router[1] host[1]

    max_router = max_router + 1 #Note: the returned value already change
    max_host = max_host + 1

    # generic routerNode and hostNode definition that every network will have
    print(routing_alg)
    if (routing_alg == 'shortestPath'):
        host_node_type = 'hostNodeBase'
        router_node_type = 'routerNodeBase'
    else:
        if routing_alg == 'DCTCPBal' or routing_alg == 'DCTCPQ' or routing_alg == 'TCP' or routing_alg == 'TCPCubic':
            host_node_type = 'hostNodeDCTCP'
        else: 

            if routing_alg == 'DCTCPRate':
                host_node_type = 'hostNodePropFairPriceScheme'
            elif routing_alg == 'OnlineFee':
                host_node_type = 'hostOnlineFeeBase'
            else:
                host_node_type = 'hostNode' + routing_alg[0].upper() + routing_alg[1:]
            
            if routing_alg == 'landmarkRouting':
                router_node_type = 'routerNodeWaterfilling'
            else:
                if routing_alg == 'DCTCPRate' or routing_alg == 'DCTCPQ' or routing_alg == 'TCP' or routing_alg == 'TCPCubic':
                    router_node_type = 'routerNodeDCTCP'
                elif routing_alg == 'OnlineFee':
                    router_node_type = 'routerOnlineFeeBase' # Note: OnlineScheme:
                else:
                    router_node_type = 'routerNode' + routing_alg[0].upper() + routing_alg[1:]
    
    print(router_node_type)

    outfile.write("import " + router_node_type + ";\n")
    outfile.write("import " + host_node_type + ";\n\n")

    outfile.write("network " + network_name + "_" + routing_alg + "\n")  # two_node_OnlineFee
    outfile.write("{\n")


    # This script (meant for a simpler datacenter topology) just assigns the same link delay to all links.
    # You need to change this such that the parameter values are instead assigned on a per node basis and 
    # are read from an additional 'delay' column and 'channel balance' columns in the text file.
    # FIXME: do we need to do sth here? the channel balance? where is the read place? 
    outfile.write('\tparameters:\n\t\tdouble linkDelay @unit("s") = default(100us);\n')
    outfile.write('\t\tdouble linkDataRate @unit("Gbps") = default(1Gbps);\n')  # Note: these two line seems useless???
    outfile.write('\tsubmodules:\n')
    #########################
    outfile.write('\t\thost['+str(max_host)+']: ' + host_node_type + ' {} \n')
    outfile.write('\t\trouter['+str(max_router)+']: ' + router_node_type + ' {} \n')
    # outfile.write('\t\thost['+str(number_of_nodes)+']: ' + host_node_type + ' {} \n')  # OnlineScheme: since we delete some channel that less than 40%
    # outfile.write('\t\trouter['+str(number_of_nodes)+']: ' + router_node_type + ' {} \n')
    #########################

    outfile.write('\tconnections: \n')

    for link in linklist:
        a = link[0]
        b = link[1]
        abDelay = link[2]
        baDelay = link[3]

        outfile.write('\t\t' + a + '.out++ --> {delay = ' + str(abDelay) +'ms; }')
        outfile.write(' --> ' + b + '.in++;  \n')
        outfile.write('\t\t' + a + '.in++ <-- {delay = ' + str(baDelay) +'ms; }')
        outfile.write(' <-- ' + b + '.out++;  \n')
    outfile.write('}\n')




# generate either a small world or scale free graph
def generate_graph(size, graph_type):
    if graph_type == 'random':
        G = nx.dense_gnm_random_graph(size, size * 5,seed=SEED)
    elif graph_type == 'small_world':
        G = nx.watts_strogatz_graph(size, 8, 0.25, seed=SEED)
    elif graph_type == 'small_world_sparse':
        G = nx.watts_strogatz_graph(size, size/8, 0.25, seed=SEED)
    elif graph_type == 'scale_free':
        # regular expts
        G = nx.barabasi_albert_graph(size, 8, seed=SEED) 

        # implementation, celer expts - 10 node graph
        # G = nx.barabasi_albert_graph(size, 5, seed=12)
    elif graph_type == 'scale_free_sparse':
        G = nx.barabasi_albert_graph(size, size/8, seed=SEED)
    elif graph_type == 'tree':
        G = nx.random_tree(size, seed=SEED)

    # remove self loops and parallel edges
    # G.remove_edges_from(G.selfloop_edges())
    G.remove_edges_from(nx.selfloop_edges(G))
    G = nx.Graph(G)

    print('Generated a ', graph_type, ' graph')
    print('number of nodes: ', G.number_of_nodes())
    print('Number of Edges: ', G.number_of_edges())
    print('Number of connected components: ', nx.number_connected_components(G))
    return G




def capacity_percentile(capacity):
    return np.percentile(capacity, (25, 30, 35, 45, 50, 75, 80, 90, 95, 98, 99, 99.5, 99.6191, 99.7, 99.9, 100), interpolation='nearest')


def drawBox(capacity):
    pyplot.boxplot([capacity], labels=['capacity'])
    pyplot.title('capacity distribution')
    pyplot.show()


def drawHist(capacity):
    pyplot.hist(capacity, 20)
    pyplot.xlabel('capacity')
    pyplot.ylabel('Frequency')
    pyplot.title('capacity distribution')
    pyplot.show()
# Note: read the original json file? and out put the txt topo file
# print the output in the desired format for write_ned_file to read
# generate extra end host nodes if need be
# make the first line list of landmarks for this topology
# Note: the delay_per_channel is a default delay
# Note: balance_per_channel is the default capacity of each channel ? is this should not read from the file? yes, if use lnd_july15_2019_reducedsize
# balance_per_channel seems like a factor? 
def print_topology_in_format(G, balance_per_channel, delay_per_channel, output_filename, separate_end_hosts,\
        randomize_init_bal=False, random_channel_capacity=False, lnd_capacity=False, is_lnd=False, rebalancing_enabled=False):
    f1 = open(output_filename, "w+") # Note: write into clightning_oct5_2020_delay_uniform.txt'
    end_host_delay = delay_per_channel # Note: delay default 30

    offset = G.number_of_nodes()
    if (separate_end_hosts == False):
        offset = 0

    nodes_sorted_by_degree = sorted(G.degree, key=lambda x: x[1], reverse=True) # Note: nodes sorted by degree

    # generate landmarks based on degree Note: generate the landmark code 00
    i = 0
    landmarks, current_list = [], []
    max_degree = -1
    while len(landmarks) < NUM_LANDMARKS and i < len(nodes_sorted_by_degree):
        num_remaining = NUM_LANDMARKS - len(landmarks)
        if nodes_sorted_by_degree[i][1] == max_degree:
            current_list.append(nodes_sorted_by_degree[i][0])
        else:
            spaced_indices = np.round(np.linspace(0, len(current_list)-1, \
                    min(num_remaining, len(current_list)))).astype(int)
            if max_degree != -1:
                landmarks.extend([current_list[x] for x in spaced_indices])
            current_list = [nodes_sorted_by_degree[i][0]]
            max_degree = nodes_sorted_by_degree[i][1]
        i += 1
    if len(landmarks) < NUM_LANDMARKS:
        spaced_indices = np.round(np.linspace(0, len(current_list)-1, \
                    min(num_remaining, len(current_list)))).astype(int)
        landmarks.extend([current_list[x] for x in spaced_indices])

     
    # make the first line the landmarks and make them all router nodes
    for l in landmarks[:NUM_LANDMARKS]:
        f1.write(str(l) + "r ")
    f1.write("\n")

    total_budget = balance_per_channel * len(G.edges())
    weights = {e: min(G.degree(e[0]), G.degree(e[1])) for e in G.edges()}
    sum_weights = sum(weights.values())
    capacity_dict = dict()

    # get lnd capacity data Note: only used by lnd_capacity = true, randomly seleted 
    lnd_capacities_graph = nx.read_edgelist(LND_FILE_PATH + 'lnd_july15_2019_reducedsize' + '.edgelist')
    lnd_capacities = list(nx.get_edge_attributes(lnd_capacities_graph, 'capacity').values()) 

    # write rest of topology Note: they even have the real rtt data???
    real_rtts = np.loadtxt(LND_FILE_PATH + "ping_times_data")

    # Note: first get the percentile of capacity, and delete the edge without enough capacity
    capacity_temp_list = []
    for e in G.edges():
        capacity_temp_list.append(round(G[e[0]][e[1]]['capacity']))

    # PERCENTILE:
    percentile_list = capacity_percentile(capacity_temp_list)
    print('percentile: 25%-{}, 30%-{}, 35%-{}, 40%-{}, 50%-{}, 75%-{}, 80%-{}, 90%-{}, 95%-{}, 98%-{}, 99%-{}, 99.5%-{},99.6%-{}, 99.7%-{},99.9%-{},100%-{}'.format(percentile_list[0],percentile_list[1],percentile_list[2],percentile_list[3],percentile_list[4],percentile_list[5],percentile_list[6],percentile_list[7],percentile_list[8],percentile_list[9],percentile_list[10],percentile_list[11],percentile_list[12],percentile_list[13],percentile_list[14],percentile_list[15]))

    for e in G.edges():
        if round(G[e[0]][e[1]]['capacity']) < percentile_list[12]: 
            G.remove_edge(e[0], e[1]) # OnlineScheme: else delete this edge

    G.remove_nodes_from(list(nx.isolates(G)))
    print("IS THIS GRAPH CONNECTED?  {}".format(nx.is_connected(G)))
    print("CONNECTED COMPONETS ARE {}".format(nx.number_connected_components(G)))
    # graphs=list(nx.connected_component_subgraphs(G)) 
    graphs = list(G.subgraph(c) for c in nx.connected_components(G))
    component_size = [len(one) for one in graphs]
    
    for x in range(len(component_size)):
        print(component_size[x]),
    largest_componet_index = component_size.index(max(component_size))
    print("largest_componet_index is {}".format(largest_componet_index))
    G = list(graphs)[largest_componet_index] # Note: we only need the largest connected component 



    rename_dict = {v: int(str(v)) for v in G.nodes()} # Note: to rename the node, key is original_id, value is int??  old labels as keys and new labels as values
    G = nx.relabel_nodes(G, rename_dict) # now the graph is node in int

    # OnlineScheme: FIXME: Note: after delete these nodes and edges, all the nodes need to be relabled
    G = nx.convert_node_labels_to_integers(G, first_label=0, ordering='default', label_attribute=None)
    
    count = 0
    for e in G.edges():
        # if round(G[e[0]][e[1]]['capacity']) >= percentile_list[3]: # Note: delete < 47 [0]
        count = count + 1
        f1.write(str(e[0]) + "r " + str(e[1]) +  "r ") # Note: only router node 
        
        if not random_channel_capacity and is_lnd and "uniform" not in output_filename: # Note: if you want uniform delay, need to insert uniform in output_filename
            delay_per_channel = np.random.choice(real_rtts) / 2.0  # Note: randomly generate the delay from the RTT dataset
            f1.write(str(delay_per_channel) + " " + str(delay_per_channel) + " ")  # equal delay
        else:
            f1.write(str(delay_per_channel) + " " + str(delay_per_channel) + " ") # Note: generate uniform delays
        
        # Note: balance details
        if random_channel_capacity: # Note: random_channel_capacity
            balance_for_this_channel = -1
            while balance_for_this_channel < 2:
                balance_for_this_channel = round(np.random.normal(balance_per_channel, \
                        0.75 * balance_per_channel))
        
        elif lnd_capacity:  # Note: if the lnd capacity is use, this is not directly corresponding to the topology, it is random selected?
            balance_for_this_channel = -1
            while balance_for_this_channel < 40: # make sure the balance is greater than 40 
                balance_for_this_channel = round(np.random.choice(lnd_capacities) * \
                    (balance_per_channel / np.mean(lnd_capacities))) # Question: why need normalize?
        
        # elif is_lnd and "uniform" not in output_filename:
        elif is_lnd and "uniform" in output_filename: # Note: we want to generate uniform delay but real balance distribution
            if "lessScale" in output_filename: # Note: we can control the output file name to do sth 
                balance_for_this_channel = int(round(G[e[0]][e[1]]['capacity'] *10 * balance_per_channel))
            else:
                balance_for_this_channel = int(round(G[e[0]][e[1]]['capacity'] * balance_per_channel)) # Note: not scale OnlineScheme: here is supposed to be used part
                # Question: where you change the G capacity attribution??? 226 no, from the input, but why * balance_per_channel?
        
        else:
            balance_for_this_channel = balance_per_channel

        capacity_dict[e] = balance_for_this_channel # Note: record the e,capacity

        # Note: two end capacity assign, the two end balance is randomly assign
        if randomize_init_bal:
            one_end_bal = random.randint(1, balance_for_this_channel) # a <= N <= b
            other_end_bal = balance_for_this_channel - one_end_bal
            f1.write(str(one_end_bal) + " " + str(other_end_bal) + "\n")
        else:
            f1.write(str(round(balance_for_this_channel/2)) + " " + \
                    str(round(balance_for_this_channel/2)) + "\n") # seperate /2 uniform use
        
        # else:
        #     G.remove_edge(e[0], e[1]) # OnlineScheme: else delete this edge

    # Question: why we need extra end host? Note: if we do not have the end host, no one will intial the tx since all the nodes are routers....
    # generate extra end host nodes
    # OnlineScheme: need to change the graph so that generate corresponding host node, since after delete some channel < 47, some nodes may not exist...
    # G.remove_nodes_from(list(nx.isolates(G)))
    nodes_num = 0
    if separate_end_hosts : 
        for n in G.nodes():
            nodes_num = nodes_num + 1
            # print("why 5882? n = ", n)
            # print("why nodes_number = ", nodes_num)
            f1.write(str(n) + "e " + str(n) + "r ")
            f1.write(str(end_host_delay) + " " + str(end_host_delay) + " ")
            if rebalancing_enabled:
                f1.write(str(REASONABLE_BALANCE) + " " + str(REASONABLE_ROUTER_BALANCE) + "\n")
            else:
                f1.write(str(LARGE_BALANCE/2) + " " + str(LARGE_BALANCE/2) + "\n")  # Note: LARGE_BALANCE = 1000000000

        if args.graph_type == "parallel_graph": # Note: not used
            for (e,r) in zip([1,3], [0, 2]):
                f1.write(str(e) + "e " + str(r) + "r ")
                f1.write(str(end_host_delay) + " " + str(end_host_delay) + " ")
                f1.write(str(LARGE_BALANCE/2) + " " + str(LARGE_BALANCE/2) + "\n")
    f1.close()

    nx.set_edge_attributes(G, capacity_dict, 'capacity') # Question: why need to set edge attribute? no return G ...
    # OnlineScheme: here we can output the min ce
    mince = min(capacity_dict.values())
    print("*****************************************mince is {}".format(mince)) 
    print('min_capacity:{}'.format(min(capacity_dict.values())))
    print('max_capacity:{}'.format(max(capacity_dict.values())))
    print('mean_capacity:{}'.format(np.mean(np.array(list(capacity_dict.values())))))
    print('median_capacity:{}'.format(np.array(np.median(list(capacity_dict.values())))))
    drawBox(list(capacity_dict.values()))
    drawHist(list(capacity_dict.values()))
    print("*****************************************total edge is {}".format(count))
    print("*****************************************total nodes is {}".format(nodes_num))
    print("THIS GRAPH IS CONNECTED? {}".format(nx.is_connected(G)))
    print("total edge number_of_edges: {}".format(G.number_of_edges()))
    print("number_of_nodes: {}".format(G.number_of_nodes()))
    return G.number_of_nodes()

# parse arguments
parser = argparse.ArgumentParser(description="Create arbitrary topologies to run the omnet simulator on")
parser.add_argument('--num-nodes', type=int, dest='num_nodes', help='number of nodes in the graph', default=20)
parser.add_argument('--delay-per-channel', type=int, dest='delay_per_channel', \
        help='delay between nodes (ms)', default=30) # Note: default delay is 30
parser.add_argument('graph_type', choices=['small_world', 'scale_free', 'hotnets_topo', 'simple_line', 'toy_dctcp', \
        'simple_deadlock', 'simple_topologies', 'parallel_graph', 'dag_example', 'lnd_dec4_2018','lnd_dec4_2018lessScale', \
        'lnd_dec4_2018_randomCap', 'lnd_dec4_2018_modified', 'lnd_uniform', 'tree', 'random', \
        'lnd_july15_2019', 'lnd_just_for_test', 'lnd_whatevername'], \
        help='type of graph (Small world or scale free or custom topology list)', default='small_world') # Note: the graph type, this argument can be the real topology file
parser.add_argument('--balance-per-channel', type=int, dest='balance_per_channel', default=100) # Note: default capacity
parser.add_argument('--topo-filename', dest='topo_filename', type=str, \
        help='name of intermediate output file', default="topo.txt")
parser.add_argument('--network-name', type=str, dest='network_name', \
        help='name of the output ned filename', default='simpleNet') # Note: network-name is the output file name ... no .... only the topo name ...
parser.add_argument('--separate-end-hosts', action='store_true', \
        help='do you need separate end hosts that only send transactions') # Note: seperate for generate host only send tx? 
parser.add_argument('--randomize-start-bal', type=str, dest='randomize_start_bal', \
        help='Do not start from perfect balance, but rather randomize it', default='False') # Note: start from perfect balance
parser.add_argument('--random-channel-capacity', type=str, dest='random_channel_capacity', \
        help='Give channels a random balance between bal/2 and bal', default='False')
parser.add_argument('--lnd-channel-capacity', type=str, dest='lnd_capacity', \
        help='Give channels a random balance sampled from lnd', default='False') # Note: random sample from dataset , not real distribution
parser.add_argument('--rebalancing-enabled', type=str, dest="rebalancing_enabled",\
        help="should the end host router channel be reasonably sized", default="false") # Question: rebalancing? 
# routing_alg_list = ['shortestPath', 'priceScheme', 'waterfilling', 'landmarkRouting', 'lndBaseline', \
#         'DCTCP', 'DCTCPBal', 'DCTCPRate', 'DCTCPQ', 'TCP', 'TCPCubic', 'celer', 'OnlineFee']
# OnlineScheme: only consider OnlineFee
routing_alg_list = ['OnlineFee']

args = parser.parse_args()
np.random.seed(SEED)
random.seed(SEED)

# generate graph and print topology and ned file
if args.num_nodes <= 5 and args.graph_type == 'simple_topologies':
    if args.num_nodes == 2:
        G = two_node_graph
    elif args.num_nodes == 3:
        G = three_node_graph
    elif args.num_nodes == 4:
        G = four_node_graph
    elif 'line' in args.network_name:
        G = five_line_graph
    else:
        G = five_node_graph
elif args.graph_type in ['small_world', 'scale_free', 'tree', 'random']:
    if "sparse" in args.topo_filename:
        args.graph_type = args.graph_type + "_sparse"
    G = generate_graph(args.num_nodes, args.graph_type)
elif args.graph_type == 'toy_dctcp':
    G = toy_dctcp_graph
elif args.graph_type == 'dag_example':
    print("generating dag example")
    G = dag_example_graph
elif args.graph_type == 'parallel_graph':
    G = parallel_graph
elif args.graph_type == 'hotnets_topo':
    G = hotnets_topo_graph
elif args.graph_type == 'simple_deadlock':
    G = simple_deadlock_graph
    args.separate_end_hosts = False
elif args.graph_type.startswith('lnd_'): # if the start is real topology, change the graph G 
    # G = nx.read_edgelist(LND_FILE_PATH + 'lnd_july15_2019_reducedsize' + '.edgelist') # Note: default use the 2019 dataset and wired ... no matter the input is , always use this dataset...
    G = nx.read_edgelist(LND_FILE_PATH + 'clightning_oct5_2020_capacityInEur' + '.edgelist') # Note: in edgelist, the capacity already eur # Note: Note: Note: 
else:
    G = simple_line_graph
    args.separate_end_hosts = False

args.randomize_start_bal = args.randomize_start_bal == 'true' # if true is 1, else false
args.random_channel_capacity = args.random_channel_capacity == 'true'
args.lnd_capacity = args.lnd_capacity == 'true'

number_of_nodes = print_topology_in_format(G, args.balance_per_channel, args.delay_per_channel, args.topo_filename, \
        args.separate_end_hosts, args.randomize_start_bal, args.random_channel_capacity,\
        args.lnd_capacity, args.graph_type.startswith('lnd_'), args.rebalancing_enabled == "true")
network_base = os.path.basename(args.network_name)

for routing_alg in routing_alg_list:
    write_ned_file(number_of_nodes, args.topo_filename, args.network_name + '_' + routing_alg + '.ned', \
            network_base, routing_alg)



