package main

import (
	"bytes"
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"math/big"
	"sort"
)

const (
	IDLength = 20 // SHA-1 hash length 160 bits = 20 bytes
	K        = 4  // size of k-bucket in kademlia(20 standard nodes, 4 is a simple number)
)

type NodeID [IDLength]byte

type Node struct {
	ID   NodeID
	Addr string // IP address
}

func (id NodeID) Xor(other NodeID) NodeID {
	var res NodeID
	for i := range IDLength {
		res[i] = id[i] ^ other[i]
	}

	return res
}

// PrefixLen returns the length of the common prefix of two NodeIDs
// For example, if id = 1101001 and other = 1101010, then PrefixLen(id, other) = 4
// This decides the K-Bucket index for the node
// The longer the prefix, the closer the nodes are, and bigger the index is
func (id NodeID) PrefixLen(other NodeID) int {
	for i := range IDLength {
		xor := id[i] ^ other[i]
		if xor == 0 {
			continue
		}

		// find the first non-zero bit
		for j := range 8 {
			if (xor>>7)&1 == 1 {
				return i*8 + j
			}

			xor <<= 1
		}
	}

	return IDLength * 8
}

func (id NodeID) Compare(other NodeID) int {
	return bytes.Compare(id[:], other[:])
}

func (id NodeID) String() string {
	return hex.EncodeToString(id[:])
}

type KBucket struct {
	nodes []Node
}

// core logic of k-bucket: LRU
// 1. if the node is already in the bucket, remove to the front of the list
// 2. if node is not in the bucket, and the bucket is not full, add to the front of the list
// 3. if the bucket is full, ISO Ping the last node in the list to check if it is alive.
// To simplify, we just reject.
func (kb *KBucket) Update(n Node) {
	// 1. check if the node is already in the bucket
	for i, node := range kb.nodes {
		if node.ID == n.ID {
			// remove to the front of the list
			kb.nodes = append(kb.nodes[:i], kb.nodes[i+1:]...)
			kb.nodes = append(kb.nodes, n)
			return
		}
	}

	// 2. if node is not in the bucket, and the bucket is not full, add to the front of the list
	if len(kb.nodes) < K {
		kb.nodes = append(kb.nodes, n)
	} else {
		// TODO:
	}
}

type RoutingTable struct {
	localNode Node                   // local node information
	buckets   [IDLength * 8]*KBucket // 160 buckets, one for each prefix length
}

func NewRoutingTable(local Node) *RoutingTable {
	rt := &RoutingTable{
		localNode: local,
	}
	for i := range rt.buckets {
		rt.buckets[i] = &KBucket{}
	}

	return rt
}

func (rt *RoutingTable) AddNode(n Node) {
	if n.ID == rt.localNode.ID {
		return
	}

	prefixLen := rt.localNode.ID.PrefixLen(n.ID)

	rt.buckets[prefixLen].Update(n)
}

func (rt *RoutingTable) FindClosestNodes(targetID NodeID, count int) []Node {
	var candidates []Node

	//targetCPL := rt.localNode.ID.PrefixLen(targetID)

	for _, bucket := range rt.buckets {
		candidates = append(candidates, bucket.nodes...)
	}

	sort.Slice(candidates, func(i, j int) bool {
		distI := candidates[i].ID.Xor(targetID)
		distJ := candidates[j].ID.Xor(targetID)
		return distI.Compare(distJ) < 0
	})

	if len(candidates) > count {
		return candidates[:count]
	}
	return candidates
}

func RandomID() NodeID {
	var id NodeID
	rand.Read(id[:])
	return id
}

func (rt *RoutingTable) PrintStats() {
	fmt.Printf("\n--- Routing Table Stats for %s ---\n", rt.localNode.ID.String()[:6])
	for i, b := range rt.buckets {
		if len(b.nodes) > 0 {
			fmt.Printf("Bucket [%3d] (Shared Bits: %3d): %d nodes\n", i, i, len(b.nodes))
		}
	}
	fmt.Println("---------------------------------------")
}

func main() {

	myID := RandomID()
	me := Node{ID: myID, Addr: "127.0.0.1:8000"}
	table := NewRoutingTable(me)

	fmt.Printf("Local Node ID: %s\n", myID.String())

	fmt.Println("\nStep 1: Adding 50 random nodes to routing table...")
	for i := 0; i < 50; i++ {
		n := Node{ID: RandomID(), Addr: fmt.Sprintf("192.168.1.%d", i)}
		table.AddNode(n)
	}

	closeID := myID
	closeID[19] = closeID[19] ^ 1
	table.AddNode(Node{ID: closeID, Addr: "10.0.0.1"})

	table.PrintStats()

	targetID := RandomID()
	fmt.Printf("\nStep 2: Finding closest nodes to Target: %s\n", targetID.String())

	closest := table.FindClosestNodes(targetID, K)

	for i, n := range closest {
		dist := n.ID.Xor(targetID)
		distInt := new(big.Int).SetBytes(dist[:])
		fmt.Printf("#%d Node: %s... | Distance: %e (Prefix: %d)\n",
			i+1, n.ID.String()[:8], float64(distInt.Int64()), n.ID.PrefixLen(targetID))
	}
}
