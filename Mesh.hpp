#pragma once

/** @file Mesh.hpp
 * @brief An undirected subset of the graph class that stores triangles
 */

#include "CS207/Util.hpp"
#include <algorithm>
#include <vector>
#include <cassert>
#include <Graph.hpp>
using namespace std;


/** @class 	Mesh
 * @brief 	A mesh triangular graph class
 * @tparam  T	The value type for a triangle
 * @tparam  E	The value type for an edge
 * @tparam  N	The value type for a node
 */
template <typename N, typename E, typename T>
class Mesh {
 private:
    struct tri_info_type;
    typedef Graph<N,E> GraphType;
 public:
  
  /** Template type of a Vertex of the Triangle **/
  typedef N node_value_type;
  
  /** Template type of a Edge of the Triangle . */
  typedef E edge_value_type;

  /** Template type of a Triangle **/
  typedef T tri_value_type;

  /** Predeclaration of Node type. */
  class Node;

  /** Predeclaration of Edge type. */
  class Edge;

  /** Predeclaration of Triangle type. */
  class Triangle;

  /** Graph typedefs **/
  typedef typename Graph<N,E>::Node node_type;
  typedef typename Graph<N,E>::Edge edge_type;
  typedef typename Graph<N,E>::Node Node;
  typedef typename Graph<N,E>::Edge Edge;
  typedef typename Graph<N,E>::node_iterator node_iterator;
  typedef typename Graph<N,E>::edge_iterator edge_iterator;

  /** Synonym for Mesh (following STL conventions). */
  typedef Mesh mesh_type;

  /** Synonym for Triangle (following STL conventions). */
  typedef Triangle tri_type;

  /** Type of indexes and sizes. */
  typedef unsigned size_type;

  /** Type used to return area of triangles. */
  typedef double value_type;

  /** Forward iterators, which iterates over all triangles. */
  class tri_iterator;

  /** Forward iterators, which iterates over all triangles with the same vertex. */
  class vertex_iterator;

  /** Forward iterators, which iterates over all triangles with the same edge. */
  class tri_edge_iterator;


  /** Initializes an empty mesh. */
  Mesh(){
  }

  /** Default destructor */
  ~Mesh() = default;

  /** Destroys the ability for users to assign mesh by reference */
  Mesh& operator=(const Mesh&) = delete;

  /** Equality comparison function */
  bool operator==(const Mesh& n) const{
	return n.graph_==this->graph_;
  }

  /** Returns the number of nodes in the graph. */
  size_type num_nodes() {
	return this->graph_.size();
  }

  /** Adds a node to this graph.  O(1) amortized operations */
  Node add_node(const Point& position){
  	return this->graph_.add_node(position);
  }

  /** Determines if a node is a valid node of this graph. */
  bool has_node(const Node& n) const {
	return this->graph_.has_node(n);
  }

  /** Returns the node at index i in the mesh. */
  Node node(size_type i) {
	return this->graph_.node(i);
  }  

  /** Returns the edge at index i in the mesh. */
  Edge edge(size_type i) {
	return this->graph_.edge(i);
  }

  /** For a given edge index, returns true if it is shared */
  bool has_neighbor(size_type i){
	assert(i<edge_lookup_.size());
  	return (edge_lookup_[i].size() >=2);
  }

  /** Returns an iterator to this triangle */
  node_iterator node_begin() {
        return this->graph_.node_begin();
  }

  /** Returns an invalid node iterator  */
  node_iterator node_end() {
	return this->graph_.node_end();
  }

  /** Returns an iterator to an edge */
  edge_iterator edge_begin(){
	return this->graph_.edge_begin();
  }

  /** Returns an invalid edge_iterator  */
  edge_iterator edge_end(){
	return this->graph_.edge_end();
  }

  /** Returns an iterator to the triangle class.  */
  tri_iterator tri_begin() {
	return tri_iterator(this, 0);
  }

  /** Returns an invalid iterator to the triangle class  */
  tri_iterator tri_end() {
	return tri_iterator(this, size());
  }

  /** Returns an iterator to the triangle class.  */
  vertex_iterator vertex_begin(size_type v) {
	assert(v<num_nodes());
	return vertex_iterator(this, v,0);
  }

  /** Returns an invalid iterator to the triangle class  */
  vertex_iterator vertex_end(size_type v) {
	assert(v<num_nodes());
	return vertex_iterator(this, v, node_lookup_[v].size());
  }

  /** Returns an iterator to the triangle class.  */
  tri_edge_iterator tri_edge_begin(size_type e1) {
	assert(e1<num_edges());
	return tri_edge_iterator(this, e1, 0);
  }

  /** Returns an invalid iterator to the triangle class  */
  tri_edge_iterator tri_edge_end(size_type e1) {
	assert(e1<num_edges());
	return tri_edge_iterator(this, e1,edge_lookup_[e1].size());
  }

  class Triangle: private totally_ordered<Triangle> {
   private:
        friend class Mesh;
	Mesh* set_;
	size_type idx_;  

	/** Returns a reference to the Triangle */
	tri_info_type& fetch() const{
	   return set_->triangles_[idx_];
	}

	Triangle(const Mesh* set, size_type idx)
        : set_(const_cast<Mesh*>(set)),  idx_(idx){
	}

	/** Helper function */
	bool notEqual(const Node n1, const Node n2){
		if (!(n1==node1() or n2==node1()))
			return node1();
		if (!(n1==node2() or n2==node2()))
			return node2();
		return node3();
	}
	
   public:
    /** Constructs an invalid triangle */
    Triangle(){
    }

    /** Returns this triangle's index  */
    const size_type& index() const{
	return idx_;
    }

    /** Returns this triangle's position. */
    const Point& position() const {
	return (node1().position()+node2().position()+node3.position())/3;
    }
	
    /** Returns this triangle's area.  */
    value_type area() const{
	Point A = node1().position();
	Point B = node2().position();
	Point C = node3().position();
	return abs( (A.x *(B.y-C.y) + B.x*(C.y - A.y)+ C.x*(A.y-B.y))/2.0);		
    }

    /** Returns a reference to this triangle's value */
    tri_value_type& value(){
	return fetch().value_;
    }

    /** Returns a read-only reference to this triangle's value */
    const tri_value_type& value() const{
	return fetch().value_;
    }

    /** Calculates the normal vector between two adjacent triangles. */
    Point norm_vector(const Triangle& t1){	
	if (!t1.has_node(node1()))
	{
		auto dx =node2().position().x - node3().position().x;
		auto dy =node2().position().y - node3().position().y;
		Point Normal = Point(-dy, dx, 0);
		if (norm(Normal, node1().position()) < norm(Normal, (node2().position() + node3.position())/2.0 )) {
			Normal *= -1.0;
		}
		return Normal;
	}
	else if (!t1.has_node(node2()))
	{
		auto dx =node1().position().x - node3().position().x;
		auto dy =node1().position().y - node3().position().y;
		Point Normal = Point(-dy, dx, 0);
		if (norm(Normal, node2().position()) < norm(Normal, (node1().position() + node3.position())/2.0 )) {
			Normal *= -1.0;
		}
		return Normal;
	}
	else{
		auto dx =node1().position().x - node2().position().x;
		auto dy =node1().position().y - node2().position().y;
		Point Normal = Point(-dy, dx, 0);
		if (norm(Normal, node3().position()) < norm(Normal, (node1().position() + node2.position())/2.0 )) {
			Normal *= -1.0;
		}
		return Normal;
	}
	
    }

	/** Returns the normal vector of an Edge */
	Point norm_vector(const Edge& e){	  
	  if (e.node1() != node1() && e.node2() != node1())
	  {
		auto dx =node2().position().x - node3().position().x;
		auto dy =node2().position().y - node3().position().y;
		Point Normal = Point(-dy, dx, 0);
	
		Point outside = node1().position() - node2().position();
	
		if(dot(outside, Normal) > 0)
			Normal *= -1.0;
		return Normal;
	  }
	  else if (e.node1() != node2() && e.node2() != node2())
	  {
		auto dx =node1().position().x - node3().position().x;
		auto dy =node1().position().y - node3().position().y;
		Point Normal = Point(-dy, dx, 0);
		Point outside = node2().position() - node1().position();
	
		if(dot(outside, Normal) > 0)
			Normal *= -1.0;
		
		return Normal;
	  }
	  else {
		auto dx =node1().position().x - node2().position().x;
		auto dy =node1().position().y - node2().position().y;
		Point Normal = Point(-dy, dx, 0);
	
		Point outside = node3().position() - node2().position();
	
		if(dot(outside, Normal) > 0)
			Normal *= -1.0;
		return Normal;
	  }
	 
	}

    /** Returns the adjacent triangles to this node */
    std::vector<Point> adjacent_triangle_vector() const{
	std::vector<Point> tri;
	auto adj = set_->adjacent_triangles(*this);
	for (size_type i=0; i<adj.size(); ++i){
		tri.push_back(norm_vector(adj[i]));
	}
	return tri;
    }

    /** Determines if a triangle has a particular node */
    bool has_node(Node& n1){
        return (fetch().n1_==n1.index() or fetch().n2_==n1.index() or fetch().n3_==n1.index()); 
    }

    /** Returns the node of this triangle  */
    Node node1() const {
	return set_->node(fetch().n1_);
    }

    /** Returns the node of this triangle  */
    Node node2() const {
	return set_->node(fetch().n2_);
    }

    /** Returns the node of this triangle  */
    Node node3() const {
	return set_->node(fetch().n3_);
    }

    /** Returns the mesh edge of this triangle  */
    Edge edge1() const {
	return set_->edge(fetch().e1_);
    }

    /** Returns the mesh edge of this triangle  */
    Edge edge2() const {
	return set_->edge(fetch().e2_);
    }

    /** Returns the mesh edge of this triangle  */
    Edge edge3() const {
	return set_->edge(fetch().e3_);
    }

    /** Determines the equality of two triangles  */
    bool operator==(const Triangle& n) const{
	return n.idx_==idx_;
    }

    /** Abstractly compares two triangles for ordering purposes.  */
    bool operator<(const Triangle& n) const{
	return idx_<n.idx_;
    }

  };

    /** Returns a node's value */
    node_value_type& value(Node n, node_value_type value){
	n.value() = value;
	return n.value();
    }

    /** Returns a read-only reference to a nodes value */
    const N& value(const Node& n) const{
	return n.value();
    }
   
    /** Returns a vectors of all adjacent triangles */	
    std::vector<Point> adjacent_triangle_vector(const Triangle& t){
	return t.adj_triangle_vector();
    }

    /** Returns adjacent triangles of a triangle */
    std::vector<Triangle> adjacent_triangles(Triangle t){
	std::vector<Triangle> tri;
	for (auto i = edge_lookup_[t.edge1().index()].begin(); i < edge_lookup_[t.edge1().index()].end(); ++i){
		if ( (*i) != t.idx_)
			tri.push_back(triangle(*i));
	}
	for (auto i = edge_lookup_[t.edge2().index()].begin(); i < edge_lookup_[t.edge2().index()].end(); ++i){
		if ((*i) != t.idx_)
			tri.push_back(triangle(*i));
	}
	for (auto i = edge_lookup_[t.edge3().index()].begin() ; i < edge_lookup_[t.edge3().index()].end(); ++i){
		if ((*i) != t.idx_)
			tri.push_back(triangle(*i));
	}
	return tri;
    }
  
  /** Returns the number of triangles in the mesh.  */
  size_type size() const {
	return this->triangles_.size();
  }

  /** Synonym for size(). */
  size_type num_triangles() const {
	return size();
  }

  /** Number of edges in this graph. */
  size_type num_edges() const {
	return this->graph_.num_edges();
  }

  /** Returns the triangle at index @a i. */
  Triangle triangle(size_type i) const {
	assert(i<size());
	return Triangle(this,i);
  }

  /** Adds a triangle to the mesh. */
  Triangle add_triangle(Node& n1, Node& n2, Node& n3){

	/* Triangle index */
	size_type tri_idx = size();
	
	/* Edges */
	edge_type e1 = this->graph_.add_edge(n1, n2);
	edge_type e2 = this->graph_.add_edge(n1, n3);
	edge_type e3 = this->graph_.add_edge(n2, n3);
		
	/*Update tri data*/
	tri_info_type tri{this,n1.index(),n2.index(),n3.index(),e1.index(),e2.index(),e3.index(),tri_value_type(),tri_idx};
	this->triangles_.push_back(tri);
	
	//udpdate edge_lookup table	
	edge_lookup_[e1.index()].push_back(tri_idx);
	edge_lookup_[e2.index()].push_back(tri_idx);
	edge_lookup_[e3.index()].push_back(tri_idx);
	
	node_lookup_[n1.index()].push_back(tri_idx);
	node_lookup_[n2.index()].push_back(tri_idx);
	node_lookup_[n3.index()].push_back(tri_idx);
		
	return this->triangle(tri_idx); 
  } 


  /** Iterates over all triangles. */
  class tri_iterator: private totally_ordered<tri_iterator> {
   private:
    friend class Mesh;
    Mesh* set_;
    size_type idx_;
    tri_iterator(const Mesh* set, size_type idx)
        : set_(const_cast<Mesh*>(set)),idx_(idx) {
    }
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Triangle value_type;
    /** Type of pointers to elements. */
    typedef Triangle* pointer;
    /** Type of references to elements. */
    typedef Triangle& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    /** Constructs an invalid tri_iterator. */
    tri_iterator() {
    }

    /** Return the Triangle at the position this iterator is dereferenced.  */
    Triangle operator*() const{
	return set_->triangle(idx_);
    }

    /** Increments the iterator.  */
    tri_iterator& operator++(){
	++idx_;
	return *this;
    }

    /** Test the equality of an iterator based on current position.  */
    bool operator==(const tri_iterator& a) const{
	return idx_==a.idx_ && set_==a.set_;
    }

    /** Test the inequality of an iterator based on current position.  */
    bool operator<(const tri_iterator& a) const{
	return idx_<a.idx_ ;
    }
   
  };


  /** Iterates over all triangles incident to a node. */
  class vertex_iterator: private totally_ordered<vertex_iterator> {
   private:
    friend class Mesh;
    Mesh* set_;
    size_type nidx_;
    size_type idx_;
    vertex_iterator(const Mesh* set, size_type n1, size_type idx)
        : set_(const_cast<Mesh*>(set)),nidx_(n1),idx_(idx) {
    }
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Triangle value_type;
    /** Type of pointers to elements. */
    typedef Triangle* pointer;
    /** Type of references to elements. */
    typedef Triangle& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    /** Constructs an invalid tri_iterator. */
    vertex_iterator() {
    }

    /** Return the Triangle at the position this iterator is dereferenced.  */
    Triangle operator*() const{
	return set_->triangle(set_->node_lookup_[nidx_][idx_]);
    }

    /** Increments the iterator.  */
    vertex_iterator& operator++(){
	++idx_;
	return *this;
    }

    /** Test the equality of an iterator based on current position.  */
    bool operator==(const vertex_iterator& a) const{
	return idx_==a.idx_ && set_==a.set_ && nidx_==a.nidx_;
    }

    /** Test the inequality of an iterator based on current position.  */
    bool operator<(const vertex_iterator& a) const{
	return nidx_<a.nidx_;
    }
   
  };

  /** Iterates over all triangles incident to a node. */
  class tri_edge_iterator: private totally_ordered<tri_edge_iterator> {
   private:
    friend class Mesh;
    Mesh* set_;
    size_type eidx_;
    size_type idx_;
    tri_edge_iterator(const Mesh* set, size_type e1, size_type idx)
        : set_(const_cast<Mesh*>(set)),eidx_(e1),idx_(idx) {
    }
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Triangle value_type;
    /** Type of pointers to elements. */
    typedef Triangle* pointer;
    /** Type of references to elements. */
    typedef Triangle& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    /** Constructs an invalid iterator. */
    tri_edge_iterator() {
    }

    /** Return the Triangle at the position this iterator is dereferenced.  */
    Triangle operator*() const{
	return set_->triangle(set_->edge_lookup_[eidx_][idx_]);
    }

    /** Increments the iterator.  */
    tri_edge_iterator& operator++(){
	++idx_;
	return *this;
    }

    /** Test the equality of an iterator based on current position.  */
    bool operator==(const tri_edge_iterator& a) const{
	return idx_==a.idx_ && set_==a.set_ && eidx_==a.eidx_;
    }

    /** Test the equality of an iterator based on current position.  */
    bool operator<(const tri_edge_iterator& a) const{
	return idx_<a.idx_;
    }
   
  };
  
  private:

	/* Stores Graph Information */
	typename GraphType::graph_type graph_;
	
	/** Lookup tables */
  	map<size_type, vector<size_type> > edge_lookup_;
  	map<size_type, vector<size_type> > node_lookup_;

	/*indexed by node UID. Stores triangles data.**/
	std::vector<tri_info_type> triangles_; 

	/** Stores Triangle Information */
	struct tri_info_type {
		Mesh* set_;
		
		size_type n1_;
		size_type n2_;
		size_type n3_;

		size_type e1_;
		size_type e2_;
		size_type e3_;

		tri_value_type value_;
		size_type index_;
	};

};

