#ifndef CS207_GRAPH_HPP
#define CS207_GRAPH_HPP

/** @file Graph.hpp
 * @brief An undirected graph type
 */

#include "CS207/Util.hpp"
#include <algorithm>
#include <vector>
#include <cassert>
using namespace std;

/** @class 	Graph
 * @brief 	A template for 3D undirected graphs
 * @tparam  V	The value type for a node 
 *
 * A Graph is a set of Nodes and Edges s.t. G = <N,E> for N = <n_0,n_1,...n_m-1> and E = <e_0, e_1, .. e_d-1> where @a m == the number of valid nodes and @a d == the number of valid edges
 * A Node is a proxy for the abstract representation of the data passed into to it
 * An Edge connects two nodes s.t. <N_i,N_j> == <N_j,N_i> for all i,j in the set of valid Nodes for this Graph subject to !(i==j)
 * Users can add and retrieve nodes and edges. There is at most one edge between any pair of distinct nodes. 
 * V describes a user-defined abstract representation of a node (i.e. Mass, Temperature, Weight).
 */
template <typename V, typename E>
class Graph {
 private:
  struct node_set_t;
  struct edge_set_t;

 public:
  // PUBLIC TYPE DEFINITIONS
  /** Template type of a Node **/
  typedef V node_value_type;  

  /** Template type of a Node **/
  typedef E edge_value_type;  

  /** Type of this graph. */
  typedef Graph graph_type;

  /** Predeclaration of Node type. */
  class Node;
  /** Synonym for Node (following STL conventions). */
  typedef Node node_type;

  /** Predeclaration of Edge type. */
  class Edge;
  /** Synonym for Edge (following STL conventions). */
  typedef Edge edge_type;

  /** Type of indexes and sizes. Return type of Node::index() and
      Graph::num_nodes(), argument type of Graph::node. */
  typedef unsigned size_type;

  /** Type of node iterators, which iterate over all graph nodes. */
  class node_iterator;

  /** Type of edge iterators, which iterate over all graph edges. */
  class edge_iterator;

  /** Type of incident iterators, which iterate incident edges to a node. */
  class incident_iterator;

  // CONSTRUCTOR AND DESTRUCTOR

  /** Initializes an empty graph. */
  Graph(){
  }

  /** Default destructor */
  ~Graph() = default;

  /** Destroys the ability for users to assign graphs by reference */
  Graph& operator=(const Graph&) = delete;

  /** Abstract equality comparison function
  * @brief 	tests the abstract equality of two graphs
  * @return	true iff @a n is a proper subset of this graph && this graph is a proper set of @a n. 
  * @post	for all i,j in [0,size()), there exists a 1-to-1 bijective function s.t. (Node(i)==n.Node(j)) or n is a proper subset of the nodes in this graph and this graph is a proper subset of the nodes in n
  * Complexity: O(N^2 + E^2), where N==size() and E==num_edges()
  */
  bool operator==(const Graph& n) const{
	if (size()==n.size() && num_edges()==n.num_edges()){
		// Ensure all nodes of this graph are contained in n
		for (size_type i=0; i < size(); ++i){
			for (size_type j=0; j < n.size(); j++){
				if (n.Node(j)==this->Node(i))
					break;
				if (j == n.size()-1)
					return false;
			}
		}
		// Ensure all edges of this graph are contained in n
		for (size_type i=0; i < num_edges(); ++i){
			for (size_type j=0; j < n.num_edges(); j++){
				if (n.Edge(j)==this->Edge(i))
					break;
				if (j == n.num_edges()-1)
					return false;
			}
		}
		return true;
	}
	return false;
  }

  /** Representative equality comparison function
  * @brief  		tests if graph n is a reference for this graph
  * @param[in]  n 	pointer to a Graph		
  * @return 		returns true iff @a this==n
  * Complexity: O(1) 
  */
  bool operator==(const Graph* n) const{
	return this==n;
  }

  // NODES
  /** @class Graph::Node
   * @brief Class representing the graph's nodes.
   * @pre	When accessing the value() method, this->set_->node_lookup[set_->nodes_[uid_].index_] == uid_. Does not test the pointer to the graph for memory equality with this graph. 
   *   
   * Node objects are proxies to a specific position, index, and value.
   */
  class Node: private totally_ordered<Node> {
   public:
    /** Constructs an invalid node */
    Node(){
    }

    /** Returns this node's position. 
    * @pre	has_node(*this)==True
    * @return 	The position of this node
    * @post	has_node(*this) && node(i).position()
    */
    Point& position() const {
	return fetch().position_;
    }

    /** Returns this node's position. 
    * @pre	has_node(*this)==True
    * @return 	The position of this node
    * @post	has_node(*this) && node(i).position()
    */
    Point& position() {
	return fetch().position_;
    }

    /** Returns this node's position. 
    * @pre	has_node(*this)==True
    * @post	has_node(*this) && node(i).position() && node.position==p
    */
    void set_position(const Point& p) const {
	fetch().position_ = p;
    }

    /** Returns this node's index 
    * @pre	has_node(*this)==True
    * @return 	result in [0,graph.size())
    * @post	( (has_node(*this) && node(i).index())) )
    */
    size_type& index() const {
	return fetch().index_;
    }

    Graph* graph() const{
	return set_;
    }
    /** Returns a reference to this node's value 
    * @pre	has_node(*this)==True
    * @return 	value()==nodes_[uid_].value_
    * @post	( has_node(*this) && node(i).index()  )
    */
    node_value_type& value(){
	return fetch().value_;
    }

    /** Returns a read-only reference to this node's value 
    * @pre	has_node(*this)==True
    * @return 	value()==nodes_[uid_].value_ 
    * @post	( (has_node(*this) && node(i).index())) )
    */
    node_value_type& value() const{
	return fetch().value_;
    }

    /** Sets a node's value   
     * @pre	has_node(*this)==True
     * @post	has_node(*this) && node(i).index() && node.value()=value
     */
    void set_value(node_value_type& value) const{
	fetch().value_ = value;
    }


    /** Determines the equality of a node 
    * @pre	has_node(*this)==True
    * @return 	returns true iff this is a valid node && this.position()==n.position() && this.value()==n.value(). 
    * @post	has_node(*this) && position()==n.position() && value()==n.value()
    */
    bool operator==(const Node& n) const{
	return uid_ == n.uid_ && set_ == n.set_;
    }

    /** Determines the equality of a node to another node
    */
    bool operator==(const Node* n) const{
	return uid_ == n->uid_ && set_ == n->set_;
    }

    /** Abstractly compares two nodes for ordering. 
     *  Currently implemented as a queue (first in, first out), whereby nodes are ordered based on the order they were placed in the graph.
     *  @return 	returns false if comparing nodes of two different graphs 
    */
    bool operator<(const Node& n) const{
	return (uid_ < n.uid_) && (n.set_ == set_);
    }

    /** Returns the number of edges incident to a node
    * @pre	has_node(*this)==True
    * @return 	result in [0,graph.num_edges())
    * @return 	degree()==the number of valid nodes that are connected to this node by valid edges
    */
    size_type degree() const {
	return set_->edges_[uid_].size(); 
    }

    /** Returns an iterator to a node's edges 
     *  @pre		has_node(*this)==True
     *  @return  	set_->incident_begin(this) 
     *  @post		iterator pointing to the first incident edge of this node 
     */
    incident_iterator edge_begin() const{
	return set_->incident_begin(this);
    }

    /** Returns an invalid iterator 
     *  @post  		an iterator past-the-last of the incident edges
     */
    incident_iterator edge_end() const {
	return set_->incident_end(this);
    }
   
   private:
    friend class Graph;
    Graph* set_;
    size_type uid_;
    
    /** Returns a Node, whose validity is not guaranteed */ 
    Node(const Graph* set, size_type uid)
        : set_(const_cast<Graph*>(set)),uid_(uid) {
    }

    /** Returns a reference of the node_set_type for the index of a node
    * @pre 	valid()==True 
    * @return 	set_->nodes_[uid_]
    * @post	the node_set_t that corresponds to uid_ is returned   
    */
    node_set_t& fetch() const{
	return set_->nodes_[uid_];
    }

    /** Test the validity of this node
    * @return	set_->node_lookup[set_->nodes_[uid_].index_] == uid_   
    * @post	has_node(*this)==True
    */
    bool valid() const{
	return (uid_ < set_->nodes_.size() && set_->nodes_[uid_].index_ < set_->node_lookup.size() && set_->node(set_->nodes_[uid_].index_).uid_ == uid_);
    }

  };

  /** Returns the number of VALID nodes in the graph.
   * @post	[0,n] where n == maximum value of size_type
   * Complexity: O(1).
   */
  size_type size() const {
    return node_lookup.size();
  }

  /** Synonym for size(). */
  size_type num_nodes() const {
    return size();
  }

  /** Adds a node to the graph, returning the added node.
   * @param[in] position 	the new node's position
   * @param[in] value 		optional param that sets the value of the node
   * @return			returns the Node that was added
   * @post 			new size() == old size() + 1
   * @post 			new num_edges() == old num_edges()
   * @post 			edges_.size() == nodes_.size()
   * @post			has_node(node(size()-1))==True
   * @post			node(size()-1).value()==value
   * @post			node(size()-1).position()==position
   * @post			node(size()-1).index()== old size()
   * Complexity: O(1) amortized operations.
   */
  Node add_node(const Point& position, const node_value_type& value = node_value_type()){
    node_set_t new_node {position,value,size()};
    size_type uid = 0;
    if (size()!=0)
	uid =  node(size()-1).uid_ + 1;
    nodes_.push_back(new_node);
    vector<edge_info_type> dummy;
    edges_.push_back(dummy);
    assert(edges_.size()==nodes_.size());
    node_lookup.push_back(uid);
    return Node(this,uid);
  }

  /** Guarantees the validity of a node and the inclusion of this node in this graph
   * @param[in] n	node to test for	
   * @return 		returns true iff a node is valid
   * @post		in this graph there exists an i in [0,graph.size()) s.t. node_lookup[i]==n.uid_
   * Complexity: O(1)
   */
  bool has_node(const Node& n) const {
    	return (n.set_==this) && n.valid();
  }

  /** Return the node with index @a i.
   * @pre 	@a i = [0, size() )
   * @return	returns a valid node of this graph at index @a i 
   * @post	has_node(node(@a i))==True
   * Complexity: O(1).
   */
  Node node(size_type i) const {
    assert (i<size());
    return Node(this,node_lookup[i]);
  }

  /** Remove a node from the graph.
   * @param[in] n Node to be removed
   * @post 	new size() == old size() - 1
   * @post 	for all @a i in [n.index(), size()), new Node(i).index() == old Node(i).index()-1. 
   * @post	for all @a i in [0,num_edges()), ((edge(i).node1().uid_!=n.uid_) && (edge(i).node2().uid_!=n.uid_))
   * @post	num_edges() is reduced by the exact amount of edges that n was incident to
   * Invalidates outstanding iterators. @a n becomes invalid, as do any
   * other Node objects equal to @a n. All other Node objects remain valid.
   *
   * Complexity: Polynomial in size().
   */
  void remove_node(const Node& n) {
	if (has_node(n)){
		size_type idx = n.index();
		size_type uid = n.uid_;
		for (size_type i=idx+1; i < size(); ++i)
			--nodes_[node(i).uid_].index_;
		for (size_type k = 0; k < num_edges(); ++k){
			if (edge(k).node1()==Node(this,uid) || edge(k).node2()==Node(this,uid)){ 
				remove_edge(edge(k));  
				--k;
			}
		}
		node_lookup.erase(node_lookup.begin()+idx);
	}	
  }

  /** Removes all nodes and edges from this graph.
   * @post 	num_nodes() == 0 && num_edges() == 0
   * @post 	all outstanding Node and Edge objects for this graph are invalidated
   */
  void clear() {
    node_lookup.clear();
    edge_lookup.clear();
    nodes_.clear();
    edges_.clear();
  }


  // EDGES
  /** @class Graph::Edge
   * @pre	has_edge(*this)==True
   * @brief 	Class representing the graph's undirected edges.
   *
   * Edges are undirected.
   */
  class Edge: private totally_ordered<Edge>{
   private:
    friend class Graph;
    Graph* set_;
    size_type n1_; //node1() uid
    size_type n2_; //node2() uid
    size_type euid_; // idx of edge in edges_ set. provides O(1) access to this edge's edge_info_type at edges_[n1_][euid_]

    /** Returns an Edge for this graph */
    Edge(const Graph* set, size_type n1, size_type n2, size_type uid)
        : set_(const_cast<Graph*>(set)),  n1_(std::min(n1,n2)), n2_(std::max(n1,n2)), euid_(uid){
    }

    /** Guarantees the current validity of the Edge
     * @return 	returns true if there exist an i in [0,num_edges()) s.t. this==edge(i)
     * @post	has_edge(*this)
     */
    bool valid() const{
	return ((set_->nodes_.size()>n1_) && (set_->edges_[n1_].size()>euid_) && (set_->edges_[n1_][euid_].ouid_==n2_) && (set_->edge_lookup[set_->edges_[n1_][euid_].idx_].n1_==n1_) && (set_->edge_lookup[set_->edges_[n1_][euid_].idx_].euid_==euid_));
    }

   public:
    /** Constructs an invalid Edge. */
    Edge() {
    }

    /** Returns a read-only version of the first Node of an Edge */
    Node node1() const {
	return Node(set_,n1_);
    }

    /** Returns a read-only version of the second Node of an Edge  */
    Node node2() const {
	return Node(set_,n2_);
    }

     /** Returns a reference to this edge's value */
    edge_value_type& value(){
	return set_->edges_[n1_][euid_].value_;
    }

    void set_value(const edge_value_type& value){
	set_->edges_[n1_][euid_].value_ = value;
    }

    /** Returns a reference to this edge's value */
    const edge_value_type& value() const{
	return set_->edges_[n1_][euid_].value_;
    }

    double length() const{
	return norm(node1().position() - node2().position());
    }

    /** Returns a const index of this edge for an undirected graph
    */
    size_type index() const{
	return set_->edges_[n1_][euid_].idx_;
    }

    /** Returns the equality of an edge. 
     * @return 	returns true this edge connects equal nodes
     */

    bool operator==(const Edge& a) const{
	return a.set_==set_ && a.n1_==n1_ && a.n2_==n2_ && a.euid_ == euid_;
    }
    
    /** For ordering purposes
    * @return 	Returns true based on some value of an Edge
    * @post	(!((this<a) && (a<this))   &&   !(this==a))    ||     (((this<a) && (a<this))   &&   (this==a))
    * Complexity: O(1)
    */
    bool operator<(const Edge& a) const{
	return (n1_<a.n1_) || (n1_==a.n1_ && n2_<a.n2_);
    }

  };

  /** Returns the total number of VALID edges in the graph.
   * @return	Returns the number of valid edges in the graph
   * @post	[0,n] whre n == maximum value of size_type
   * Complexity: No more than O(num_nodes() + num_edges()), currently O(1)
   */
  size_type num_edges() const {
    return edge_lookup.size();
  }

  /** Returns a VALID edge at the index of i.
   * @pre 	0 <= @a i < num_edges()
   * @return 	returns the valid edge that correspons to index i
   * @post	has_edge(result)==True
   * Complexity: No more than O(num_nodes() + num_edges()), currently O(1)
   */
  Edge edge(size_type i) const {
    assert(i<num_edges());
    size_type n1_uid = edge_lookup[i].n1_;
    size_type n2_uid = edges_[n1_uid][edge_lookup[i].euid_].ouid_;
    return Edge(this,n1_uid,n2_uid,edge_lookup[i].euid_);
  }

  /** Determines if an edge is valid - does not check self-edges. That is done with the add_edge function
   * @param	a,b	Nodes to check for
   * @return		for @a i in [0,num_edges()) there exists an @a i s.t.
   *			( a.uid_ == edge_lookup[i].n1_ && b.uid_ == edge_lookup[i].n2_ ) ||  
   *			( a.uid_ == edge_lookup[i].n2_ && b.uid_ == edge_lookup[i].n1_ ) 
   * @post		Edge(this,@a a, @a b).valid()==True && has_node(a) && has_node(b)
   * Complexity: No more than O(num_nodes() + num_edges()), currently O(degree())
   */
  bool has_edge(const Node& a, const Node& b) const {
    for (size_type i=0; i < edges_[std::min(a,b).uid_].size(); ++i)
	if (Edge(this,a.uid_,b.uid_,i).valid())
		return a.set_==this && b.set_==this;
    return false;
  }

  /** Add an edge to the graph, or return the current edge if it already exists.
   * @param[in] a  Node to connect to Node b
   * @param[in] b  Node to connect to Node a
   * @pre 	!(a==b)
   * @return 	An Edge object @a e with e.node1() == min(a,b) and e.node2() == max(a,b)
   * @return	assertion check will not permit a self-edge
   * @post 	has_edge(@a a, @a b) == true
   * @post	!(a==b)
   * @post 	If old has_edge(@a a, @a b)==true, new num_edges() == old num_edges().
   *       	Else, new num_edges() == old num_edges() + 1.
   * @post 	there exists an @a i in [0,num_edges()) s.t. edge_lookup[i] == edge_set_t edge_val{a.uid_,b.uid}) 
   *
   * Can invalidate edge indexes -- in other words, old edge(@a i) might not
   * equal new edge(@a i). Must not invalidate outstanding Edge objects.
   *
   * Complexity: No more than O(num_nodes() + num_edges()), currently O(degree())
   */
  Edge add_edge(const Node& a, const Node& b) {  
    assert(a.uid_!=b.uid_); //no self-nodes 
    
    for (size_type i=0; i < edges_[std::min(a,b).uid_].size(); ++i)
	if (Edge(this,a.uid_,b.uid_,i).valid())
		return Edge(this,a.uid_,b.uid_,i);

    size_type n1_uid = std::min(a.uid_,b.uid_);
    size_type euid = edges_[n1_uid].size(); //euid will be based on idx of min node
    edge_set_t new_edge {n1_uid, euid};
    edge_lookup.push_back(new_edge); //updated index table

    size_type n2_uid = std::max(a.uid_,b.uid_);
    edge_info_type new_edge_info1 {{edge_lookup.size()-1},n2_uid,edge_value_type()};
    edge_info_type new_edge_info2 {{euid},n1_uid,edge_value_type()}; //node2 is a "pointer" back to node1 - in other words, node2 does not store index like node1, but rather euid_ of node1
    edges_[n1_uid].push_back(new_edge_info1);
    edges_[n2_uid].push_back(new_edge_info2);
    
    return Edge(this,n1_uid,n2_uid,euid);
  }

  /** Remove an edge, if any, returning the number of edges removed.
   * @param[in] a,b 	The nodes of an Edge to be removed
   * @return 		1 if old has_edge(@a a, @a b), 0 otherwise
   * @post 		!has_edge(@a a, @a b)
   * @post 		new num_edges() == old num_edges() - result
   * @post		invalidates all edge iterators
   * Can invalidate edge indexes -- in other words, old edge(@a i) might not
   * equal new edge(@a i). Can invalidate all edge and incident iterators.
   * Invalidates any edges equal to Edge(@a a, @a b). Must not invalidate
   * other outstanding Edge objects.
   *
   * Complexity: No more than O(num_nodes() + num_edges())
   */
  size_type remove_edge(const Node& a, const Node& b) {
    for (size_type i=0; i < edges_[std::min(a,b).uid_].size(); ++i)
	if (Edge(this,a.uid_,b.uid_,i).valid() && a.set_==this && b.set_==this)
		return remove_edge(Edge(this,a.uid_,b.uid_,i));
    return 0;
  }

  /** Remove an edge, if any, returning the number of edges removed.
   * @param[in] e 	The Edge to be removed
   * @return 		1 if old has_edge(@a a, @a b), 0 otherwise
   * @post 		new num_edges() == old num_edges() - result
   * @post		has_edge(@a e)==False
   * @post		invalidates all edge iterators
   * This is a synonym for remove_edge(@a e.node1(), @a e.node2()), but its
   * implementation can assume that @a e is definitely an edge of the graph.
   * This might allow a faster implementation.
   *
   * Can invalidate edge indexes -- in other words, old edge(@a i) might not
   * equal new edge(@a i). Can invalidate all edge and incident iterators.
   * Invalidates any edges equal to Edge(@a a, @a b). Must not invalidate
   * other outstanding Edge objects.
   *
   * Complexity: No more than O(num_nodes() + num_edges()), currently O(num_edges())
   */
  size_type remove_edge(const Edge& e) {
	size_type idx = e.index();
	edge_lookup.erase(edge_lookup.begin()+idx);
	for (size_type i=idx; i < edge_lookup.size(); ++i)
		--edge(i).index(); //updates n1 - does not need to update n2
	return 1;
  }

  // ITERATORS

  /** @class Graph::node_iterator
   * @brief Iterator class for nodes. A forward iterator. */
  class node_iterator: private totally_ordered<node_iterator> {
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Node value_type;
    /** Type of pointers to elements. */
    typedef Node* pointer;
    /** Type of references to elements. */
    typedef Node& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    /** Constructs an invalid node_iterator. */
    node_iterator() {
    }

    /** Dereferences the iterator to return the Node at that position
     * @return 		the Node(set_,i) s.t. i==set_->node_lookup[idx_]
     * @post		(for idx_ in [0,graph.size()), has_node(*this)==True)  ||  has_node(*this)==False)
     */
    Node operator*() const{
	return set_->node(idx_);
    }

    /** Increments the iterator
     * @post	new idx_ == old idx_ + 1
     * @return 	a reference to this node_iterator
     * @post	new idx_ == old idx_ + 1
     */
    node_iterator& operator++(){
	++idx_;
	return *this;
    }

    /** Test the equality of an iterator based on current position
     *  Do not need to test equality based on end position
     * @return 	idx_==a.idx_ && set_==a.set_
     * @post	(set_==a.set_) && (idx_==a.idx) 
     */
    bool operator==(const node_iterator& a) const{
	return (idx_==a.idx_ && set_==a.set_);
    }
   
   private:
    friend class Graph;
    friend class edge_iterator;
    Graph* set_;
    size_type idx_;

    node_iterator(const Graph* set, size_type idx)
        : set_(const_cast<Graph*>(set)),idx_(idx) {
    }
  };

  /** Returns an iterator that iterates over all nodes in the graph
   * @pre	Valid node
   * @return 	node_iterator(this, 0)
   * @post	(has_node(*result) && graph.size()>0) || (!has_node(*result))
   */
  node_iterator node_begin() const{
	return node_iterator(this, 0);
  }

  /** Returns an iterator that iterates over all nodes in the graph
   * @return 	node_iterator(this, size())
   * @post	!has_node(*result)
   */
  node_iterator node_end() const{
	return node_iterator(this,size());
  }


  /** @class Graph::edge_iterator
   * @brief Iterator class for edges. A forward iterator. */
  class edge_iterator: private totally_ordered<edge_iterator> {
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Edge value_type;
    /** Type of pointers to elements. */
    typedef Edge* pointer;
    /** Type of references to elements. */
    typedef Edge& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    /** Construct an invalid edge_iterator. */
    edge_iterator() {
    }

    /** Dereferences an iterator
    * @return 	edge(idx_) at @a idx @a idx_ 
    */
    Edge operator*() const{
	return set_->edge(idx_);
    }
    
    /** Increments the iterator
    * @post	new idx_ == old idx_ + 1 
    */
    edge_iterator& operator++(){
	++idx_;
	return *this;
    }

    /** Test the equality of two iterators
    * @return 	set_==a.set_ && idx_ == a.idx_
    */
    bool operator==(const edge_iterator& a) const{
	return (set_==a.set_ && idx_ == a.idx_);
    }

   private:
    friend class Graph;
    Graph* set_;
    size_type idx_;
    edge_iterator(const Graph* set, size_type idx)
        : set_(const_cast<Graph*>(set)),idx_(idx) {
    }
  };

  /** Returns an edge_iterator to edge(0)
    * @return 	edge_iterator(this,0)
    * @post	(graph.num_edges()>0 && has_edge(*edge_begin())) || has_edge(*edge_begin())==False
    */  
  edge_iterator edge_begin() const{
	return edge_iterator(this,0);
  }

  /** Returns an edge_iterator to the last edge
    * @return 	edge_iterator(this,num_edges())
    * @post	has_edge(*edge_end())==false
    */ 
  edge_iterator edge_end() const{
	return edge_iterator(this,num_edges());
  }

  /** @class Graph::incident_iterator
   * @brief Iterator class for edges incident to a given node. A forward
   * iterator. 
   * Complexity: O(n.degree())
   */
  class incident_iterator: private totally_ordered<incident_iterator> {
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Edge value_type;
    /** Type of pointers to elements. */
    typedef Edge* pointer;
    /** Type of references to elements. */
    typedef Edge& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;
    /** Begin */
    //typedef set_->iterator_begin(this) begin;
    /** End */
    //typedef set_->iterator_end(this) end;

    /** Construct an invalid incident_iterator. */
    incident_iterator(){
    }

    /** Dereferences the incident iterator and returns the Edge
     *  @return 	an Edge incident to node with the @a uid_== node.uid_
     */ 
    Edge operator*() const{
	return fetch();
    }

    /** Increments the iterator to the next valid value that satisfies the validity predicate of Edge.value()
   * @pre	This is a valid iterator
   * Complexity: O(1) amortized time
   */
    incident_iterator& operator++(){
	++it_;
	return *this;
    }

    /** Test the equality of two incident iterators
     * @return	true if iterating over the same node in the same graph
     * @post	(set_==a.set_ && n_uid_==a.n_uid_ && it_==a.it_)
     */
    bool operator==(const incident_iterator& a) const{
	return (set_==a.set_ && n_uid_==a.n_uid_ && it_==a.it_);
    }

   private:
    friend class Graph;
    Graph* set_;
    size_type n_uid_;
    size_type it_; 

    incident_iterator(const Graph* set, size_type uid, size_type it)
        : set_(const_cast<Graph*>(set)), n_uid_(uid),it_(it){
    }

    /** Returns the Edge that the incident iterator points to 
     *  @post	( (it_!=end_) && has_edge(*it_) ) || has_edge(*it_)==False 
     */
    Edge fetch() const{
	if (set_->edges_[n_uid_][it_].ouid_ > n_uid_){
		size_type euid = set_->edge_lookup[set_->edges_[n_uid_][it_].idx_].euid_;
		return Edge(set_,set_->edges_[n_uid_][it_].ouid_,n_uid_,euid);
	}else{
		size_type euid = set_->edges_[n_uid_][it_].euid_;
		return Edge(set_,set_->edges_[n_uid_][it_].ouid_,n_uid_,euid);
	}
    }

  };

  /** Returns an iterator to the beginning of a specific nodes edges if the node has an edge, or an invalid incident_iterator if it doesn't have any edges
   */
  incident_iterator incident_begin (const Node* n) const{
	return incident_iterator(this,n->uid_,0);
	
  }

  /** Returns an invalid iterator to node @a n
   */
  incident_iterator incident_end(const Node* n) const{
	return incident_iterator(this,n->uid_,n->degree());
  }


  incident_iterator incident_begin (const Node n) const{
	return incident_iterator(this,n.uid_,0);
  }

  /** Returns an invalid iterator to node @a n
   */
  incident_iterator incident_end(const Node n) const{
	return incident_iterator(this,n.uid_,n.degree());
  }

 private:
  struct node_set_t {
	Point position_;
	node_value_type value_;
	size_type index_;
	bool operator==(const node_set_t& a) const{
		return ((position_==a.position_) && (value_==a.value_) && (index_==a.index_));
    	}
	
  };
  struct edge_set_t {
	size_type n1_; //uid of node1, s.t. node1 < node2
	size_type euid_; //idx of edge s.t. in edges_[n1_][euid_] = Edge
   }; 

   struct edge_info_type{
	union{
		size_type idx_; //if node1 -> stores edge idx within edge_lookup
		size_type euid_; //if node2 -> stores euid_ of n1
	};
	size_type ouid_; //other nodes uid_
	edge_value_type value_; //user defined value for this edge
   }; 

  /*indexed by node UID. Stores node data. Includes invalid nodes.**/
  std::vector<node_set_t> nodes_; 

  /**indexed from [0, size()), stores node UID **/
  std::vector<size_type> node_lookup;  

  /*indexed from [0, num_edges()) - stores uid of n1_ and euid_ of that node for lookup in edges_ table.  {n1_,euid_} form a composite key for an edge.  euid_ alone cannot uniquely identify an edge **/
  std::vector<edge_set_t> edge_lookup;

  /**adjacency list that stores the edge_info_type of an edge */
  std::vector<vector<edge_info_type>> edges_; 
};

#endif
