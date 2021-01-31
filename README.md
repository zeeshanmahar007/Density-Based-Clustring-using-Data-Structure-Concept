# Density Based Clustring

Density and cluster property based data clustering program implemented using graph data structure.

The following data structures were used: 
- Graph (Undirected)(Adjacency list)
- AVL Tree
- Heap

## Algorithm

- Start with a single node as a cluster 
- Grow cluster by adding nodes from neighbors one by one  
 
 
 

## DataSet

<b> FILE FORMAT: </b>

[NODE1] [NEIGHBOUR NODE] [EDGE WEIGHT]

<b> Sample Input: </b>

Enter the filename to read the file : Sample.txt

Enter threshold value for density : 1

Enter threshold value for CP : 1




### Input Data:

TFC3	MYO4	0.014

TFC3	SHP1	0.117

TFC3	TFC1	0.989

MYO4	SHP1	0.3

GLK1	CLB2	0.015

GLK1	ASN1	0.016

YCL047C	YBL071C	0.02

YCL047C	YOR291W	0.02

RVS161	YAR010C	0.172

RVS161	FHL1	0.01




### output Cluster:

Cluster # 1

SHP1	MYO4	TFC3	

Cluster # 2

RVS161	YAR010C	

Cluster # 3

YCL047C	YBL071C	

Cluster # 4

GLK1	CLB2	

Cluster # 5

YOR291W	

Cluster # 6

TFC1	

Cluster # 7

ASN1	

Cluster # 8

FHL1	



