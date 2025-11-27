package main

import (
	"crypto/sha256"
	"fmt"
)

type MerkleNode struct {
	Left  *MerkleNode
	Right *MerkleNode
	Data  []byte
}

func NewMerkleNode(left, right *MerkleNode, data []byte) *MerkleNode {
	node := &MerkleNode{}

	if left == nil && right == nil {
		hash := sha256.Sum256(data)
		node.Data = hash[:]
	} else {
		prevHashes := append(left.Data, right.Data...)
		hash := sha256.Sum256(prevHashes)
		node.Data = hash[:]
		node.Left = left
		node.Right = right
	}

	return node
}

func NewMerkleTree(data [][]byte) *MerkleNode {
	var nodes []*MerkleNode

	for _, d := range data {
		node := NewMerkleNode(nil, nil, d)
		nodes = append(nodes, node)
	}

	for len(nodes) > 1 {
		var newLevel []*MerkleNode

		for i := 0; i < len(nodes); i += 2 {
			node1 := nodes[i]
			node2 := nodes[i] // replicate the last node(bitcoin)
			if i+1 < len(nodes) {
				node2 = nodes[i+1]
			}

			newNode := NewMerkleNode(node1, node2, nil)
			newLevel = append(newLevel, newNode)
		}

		nodes = newLevel
	}

	return nodes[0]
}

func main() {
	data := [][]byte{
		[]byte("Transaction 1"),
		[]byte("Transaction 2"),
		[]byte("Transaction 3"),
		[]byte("Transaction 4"),
	}

	root := NewMerkleTree(data)

	fmt.Printf("Merkle Root Hash: %x\n", root.Data)
}
