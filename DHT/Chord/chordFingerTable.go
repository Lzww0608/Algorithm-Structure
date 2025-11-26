package main

import (
	"fmt"
	"math/big"
	"math/rand"
	"time"
)

var (
	MBit     = 160
	RingSize = new(big.Int).Lsh(big.NewInt(1), uint(MBit))
)

func init() {
	rand.Seed(time.Now().UnixNano())
}

// FingerEntry models a single entry inside a finger table.
type FingerEntry struct {
	Start       *big.Int
	IntervalEnd *big.Int
	Node        *Node
}

// Node models a Chord node with basic maintenance routines.
type Node struct {
	ID          *big.Int
	Fingers     []*FingerEntry
	Predecessor *Node
}

// NewNode constructs a node with all finger entries initialized to itself.
func NewNode(id int64) *Node {
	nodeID := big.NewInt(id)
	n := &Node{
		ID:      new(big.Int).Set(nodeID),
		Fingers: make([]*FingerEntry, MBit),
	}

	for i := 0; i < MBit; i++ {
		start := modAdd(n.ID, pow2(i))
		end := modAdd(n.ID, pow2(i+1))
		n.Fingers[i] = &FingerEntry{
			Start:       start,
			IntervalEnd: end,
			Node:        n,
		}
	}

	return n
}

// Successor returns the direct successor of the current node.
func (n *Node) Successor() *Node {
	if n == nil || len(n.Fingers) == 0 || n.Fingers[0] == nil || n.Fingers[0].Node == nil {
		return n
	}
	return n.Fingers[0].Node
}

// SetSuccessor updates the first finger entry to point to the provided node.
func (n *Node) SetSuccessor(node *Node) {
	if len(n.Fingers) == 0 {
		return
	}
	if n.Fingers[0] == nil {
		n.Fingers[0] = &FingerEntry{
			Start:       modAdd(n.ID, pow2(0)),
			IntervalEnd: modAdd(n.ID, pow2(1)),
		}
	}
	n.Fingers[0].Node = node
}

// FindSuccessor locates the successor for a given identifier.
func (n *Node) FindSuccessor(id *big.Int) *Node {
	succ := n.Successor()
	if succ == nil {
		return n
	}

	if n.isInRange(id, n.ID, succ.ID, true) || succ == n {
		return succ
	}

	nPrime := n.closestPrecedingNode(id)
	if nPrime == nil {
		return n
	}
	if nPrime == n {
		return succ
	}
	return nPrime.FindSuccessor(id)
}

// closestPrecedingNode scans the finger table to find the closest node preceding id.
func (n *Node) closestPrecedingNode(id *big.Int) *Node {
	for i := MBit - 1; i >= 0; i-- {
		if n.Fingers[i] == nil || n.Fingers[i].Node == nil {
			continue
		}
		fNode := n.Fingers[i].Node
		if fNode != nil && n.isInRange(fNode.ID, n.ID, id, false) {
			return fNode
		}
	}
	return n
}

// InitFingerTable initializes the finger table entries when joining via node nPrime.
func (n *Node) InitFingerTable(nPrime *Node) {
	n.Fingers[0].Node = nPrime.FindSuccessor(n.Fingers[0].Start)
	if succ := n.Successor(); succ != nil {
		n.Predecessor = succ.Predecessor
	}

	for i := 0; i < MBit-1; i++ {
		nextStart := n.Fingers[i+1].Start
		currNode := n.Fingers[i].Node
		if currNode != nil && n.isInRange(nextStart, n.ID, currNode.ID, false) {
			n.Fingers[i+1].Node = currNode
		} else {
			n.Fingers[i+1].Node = nPrime.FindSuccessor(nextStart)
		}
	}
}

// FixFingers refreshes a random finger entry to maintain routing accuracy.
func (n *Node) FixFingers() {
	if MBit == 0 {
		return
	}
	i := rand.Intn(MBit)
	n.Fingers[i].Node = n.FindSuccessor(n.Fingers[i].Start)
	fmt.Printf("Node %s: Fixed finger %d, start %s, pointing to %s\n",
		n.ID.String(), i, n.Fingers[i].Start.String(), n.Fingers[i].Node.ID.String())
}

// Stabilize performs the stabilize routine for Chord.
func (n *Node) Stabilize() {
	succ := n.Successor()
	if succ == nil {
		return
	}

	x := succ.Predecessor
	if x != nil {
		switch {
		case n.ID.Cmp(succ.ID) == 0:
			n.SetSuccessor(x)
		case n.isInRange(x.ID, n.ID, succ.ID, false):
			n.SetSuccessor(x)
		}
	}
	n.Successor().Notify(n)
}

// Notify accepts a node that claims to be the predecessor.
func (n *Node) Notify(nPrime *Node) {
	if n.Predecessor == nil || n.isInRange(nPrime.ID, n.Predecessor.ID, n.ID, false) {
		n.Predecessor = nPrime
	}
}

// CheckPredecessor verifies if the predecessor is still alive.
func (n *Node) CheckPredecessor() {
	if n.Predecessor != nil {
		// In a real system, ping the predecessor here.
	}
}

// isInRange tests whether id is within the circular interval (start, end).
func (n *Node) isInRange(id, start, end *big.Int, rightInclusive bool) bool {
	if start.Cmp(end) == 0 {
		return true
	}

	if start.Cmp(end) < 0 {
		if rightInclusive {
			return start.Cmp(id) < 0 && id.Cmp(end) <= 0
		}
		return start.Cmp(id) < 0 && id.Cmp(end) < 0
	}

	if rightInclusive {
		return id.Cmp(start) > 0 || id.Cmp(end) <= 0
	}
	return id.Cmp(start) > 0 || id.Cmp(end) < 0
}

func pow2(exp int) *big.Int {
	return new(big.Int).Lsh(big.NewInt(1), uint(exp))
}

func modAdd(base, delta *big.Int) *big.Int {
	result := new(big.Int).Add(base, delta)
	return result.Mod(result, RingSize)
}

func recalcRingSize() {
	RingSize = new(big.Int).Lsh(big.NewInt(1), uint(MBit))
}

func main() {
	// Simulate a small ring with m = 3 (0-7).
	MBit = 3
	recalcRingSize()

	n0 := NewNode(0)
	n0.SetSuccessor(n0)

	n1 := NewNode(1)
	fmt.Println("--- Node 1 Joining via Node 0 ---")
	n1.InitFingerTable(n0)

	fmt.Printf("Node 1 finger[0] points to: %s\n", n1.Fingers[0].Node.ID.String())

	fmt.Println("--- Running Fix Fingers on Node 1 ---")
	n1.FixFingers()
}
