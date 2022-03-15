/**********************************************************
 * 1st Programming Exercise (part 1 of 2)
 * Course: Algorithms and Complexity
 * Winter - 2021
 * 
 * We construct a graph from the input G = (M \cup U, E). We
 * separate the vertex set V in two disjoint set of nodes,
 * the "morty" nodes and the "universe" nodes.
 * 
 * Also, the edges in this graph are also separated in two sets. The
 * one set is this of the portals. For each portal, connecting
 * two universes u1, u2 of width w, we create the (non directed)
 * edge {u1, u2} of weight w.
 * 
 * In addition to these edges, we add an edge {m1, u2} for
 * the initial positions of the mortys, i.e. if, at the
 * beginning morty m1 is in universe u2, we add the edge
 * {m1, u2} of infinite weight.
 * 
 * Observe that all the morties can go to their respective
 * universe, if they belong to the same connected component.
 * Therefore, we will imitate Kruskal's algorithm selecting
 * in each iteration the "most expensive" edge. We stop, when
 * for each nodes m_i, u_i, there is a m_i, u_i--path connecting
 * the two nodes, in the graph *inducted* by the selected edges.
 * 
 * 
 * Author: Merkouris Papamichail
 * email: m.papamichail@di.uoa.gr
 * Institute: PGSP Algorithms, Login and Discrete Mathematics
 * NKUA, NTUA
 * 
 **********************************************************/ 


#include <iostream>		// std::out
#include <string>		// string

#include <algorithm>	// heap operations

#include <vector>		// vector container
#include <list>
#include <set>

#include <limits>		// infinity

#include <cassert>		// assert


using namespace std;

const int Inf = std::numeric_limits<int>::max();

/***********
 * Classes *
 ***********/


class Vertex{
// Data Members
	int 	v;
	char	type;	// (m)orty or (u)niverse
public:
	// Constructor
	Vertex(int vv, char ttype){
		assert(vv >= 1 && (ttype == 'm' || ttype == 'u'));
		
		v		= vv;
		type	= ttype;
	}
	
	// Accessors
	int  get_v()	const {return v;}
	char get_type()	const {return type;}
	
	// print
	void print(){
		cout << type << v << endl;
	}
};


// functions
struct vertex_comp{
	bool operator()(const Vertex* v1, const Vertex* v2) const{
		return v1->get_type() < v2->get_type() ||
				(v1->get_type() == v2->get_type() && v1->get_v() < v2->get_v());
	}	
};


class Edge{
// Data Members
	Vertex* v1;
	Vertex* v2;
	int w;

// Methods
public:
	// Constructors	
	// useful constructor
	Edge(Vertex* vv1, Vertex* vv2, int ww){
		assert(vv1 != NULL && vv2 != NULL && ww >= 0);
		
		v1 	= vv1;
		v2 	= vv2;
		w	= ww;
	}
	
	// Accessors
	Vertex* get_v1() const { return v1; }
	Vertex* get_v2() const { return v2; }
	int		get_w()	 const { return w;  }
};


// functions
struct edge_comp{
	bool operator()(const Edge* e1, const Edge* e2) const{
		return e1->get_w() < e2->get_w();
	}
};


class Graph{
private:
	// The graph
	set<Vertex*, vertex_comp>		V;	// vertices
	multiset<Edge*, edge_comp>		E;	// edges, this will be a heap
	
	// edge iterator
	multiset<Edge*, edge_comp>::reverse_iterator edge_it;
	
	//states
	int num_vertices;
	int num_edges;
	int num_satisfied_constraints;
	
	int narrowest_portal;
	
	
	// The connected components we created
	list<set<Vertex*>*> connected_components;

	// State
	bool initialization_completed;
	
// private methods

	// Initialization
	void add_vertex(int v, char type){
		Vertex* vertex = new Vertex(v, type);
		V.insert(vertex);
	}

	void add_vertices(int num_morties, int num_universes){
		assert(num_morties > 0 && num_universes > 0);
		
		// add morties
		for(int i = 1; i <= num_morties; ++i){
			add_vertex(i, 'm');
		}
		
		// add universes
		for(int i = 1; i <= num_universes; ++i){
			add_vertex(i, 'u');
		}
	}
	
	// algorithm
	void initialize_conected_components(){
		for(set<Vertex*, vertex_comp>::iterator it = V.begin(); it != V.end(); ++it){
			set<Vertex*>* cc = new set<Vertex*>;
			cc->insert(*it);
			connected_components.push_back(cc);
		}
	}
	
	// union - find -..ish
	list<set<Vertex*>*>::iterator find_connected_component(Vertex* v){
				
		list<set<Vertex*>*>::iterator cc_iter = connected_components.begin();
		for(; cc_iter != connected_components.end(); ++cc_iter){
			set<Vertex*>::iterator v_it = (*cc_iter)->find(v);
			
			if(v_it != (*cc_iter)->end()){
				break;
			}
		}
		
		
		// this find operation *must* always succeed
		assert(cc_iter != connected_components.end());
		
		return cc_iter;
	}
	
	
	void check_constraints(list<set<Vertex*>*>::iterator cc1, list<set<Vertex*>*>::iterator cc2){
		for(set<Vertex*>::iterator it = (*cc1)->begin(); it != (*cc1)->end(); ++it){
			int vv		= (*it)->get_v();
			
			// reverse the type
			char type;
			if((*it)->get_type() == 'm')	type = 'u';
			else							type = 'm';
			
			// get the pointer to this node
			Vertex v(vv, type);  set<Vertex*, vertex_comp>::iterator v_iter = V.find(&v);
			
			// search if this node is a member of cc2
			if((*cc2)->find(*v_iter) != (*cc2)->end()){
				// the node belongs to cc2
				// one constraint has been satisfied
				--num_satisfied_constraints;
			}
		}
	}
	
	
	void unify_connected_component(list<set<Vertex*>*>::iterator cc1, list<set<Vertex*>*>::iterator cc2){
		assert(cc1 != cc2);
		
		// merge the two cc
		(*cc1)->insert( (*cc2)->begin(), (*cc2)->end() );
		
		// erase cc no. 2
		(*cc2)->clear();
		delete (*cc2);
		connected_components.erase(cc2);
	}
	
public:
	// Constructor - Destructor
	Graph(int num_universes, int num_portals){
		assert(num_universes > 0 && num_portals > 0);
		
		num_vertices				= 2 * num_universes;
		num_edges					= num_portals;
		num_satisfied_constraints	= num_universes;	// This variable denotes the number
														// of UNSATISFIED constraints!!
		narrowest_portal = Inf;							// Although, it was too troublesome
														// to correct this typo.
		add_vertices(num_universes, num_universes);
		
		initialize_conected_components();
		
		initialization_completed = false;
	}
	~Graph(){
		
		// delete vertices
		for(set<Vertex*, vertex_comp>::iterator it = V.begin(); it != V.end(); ++it){
			delete *it;
		}
		V.clear();
		
		// delete edges
		for(multiset<Edge*, edge_comp>::iterator it = E.begin(); it != E.end(); ++it){
			delete *it;
		}
		E.clear();
		
		for(list<set<Vertex*>*>::iterator it = connected_components.begin(); it != connected_components.end(); ++it){
			(*it)->clear();
			delete *it;
		}
		connected_components.clear();
	}
	
	// Accessors
	int get_narrowest_portal() const{
		return narrowest_portal;
	}
	
	int get_num_unsatisfied_constraints() const{
		return num_satisfied_constraints;
	}
	
	bool constraints_satisfied() const{
		return num_satisfied_constraints == 0;
	}
	
	// Mutators
	void add_edge(int vv1, char type1, int vv2, char type2, int w){
		assert(initialization_completed == false);
		
		assert(	w > 0							&&
				vv1 > 0 && vv2 > 0 				&&
				(type1 == 'm' || type1 == 'u') 	&&
				(type2 == 'm' || type2 == 'u')
		);
		
		Vertex v1(vv1, type1);
		Vertex v2(vv2, type2);
		
		set<Vertex*, vertex_comp>::iterator it1 = V.find(&v1);
		set<Vertex*, vertex_comp>::iterator it2 = V.find(&v2);
		
		Edge* e = new Edge(*it1, *it2, w);
		
		E.insert(e);
	}
	
	// Algorithm
	void set_initialization_completed(){
		initialization_completed = true;
		
		edge_it = E.rbegin();
	}
	
	void select_edge(){
		assert(initialization_completed == true);
		assert(edge_it != E.rend());				// ensure there are still edges left
		
		Edge* edge_ptr = *edge_it;
		list<set<Vertex*>*>::iterator cc1 = find_connected_component(edge_ptr->get_v1());
		list<set<Vertex*>*>::iterator cc2 = find_connected_component(edge_ptr->get_v2());
		
		
		// update narrowest portal
		if((*edge_it)->get_w() < narrowest_portal) narrowest_portal = (*edge_it)->get_w();
		
		
		// unify only two previously disjoint connected components
		if(cc1 != cc2){
			// check how many new constraint(s) has(ve) been satisfied
			// at least one new constraint must be satisfied, since we
			// merge two disjoint cc
			check_constraints(cc1, cc2);			// the order of the commands check_constraints and
													// unify_connected_component is of *great* importance!
			// unify the cc							// if the order is reversed, the outcome is undefined
			unify_connected_component(cc1, cc2);
		}
		
		// procced to next edge
		++edge_it;
	}
	
	// print
	void print(){
		
		// Print Vertices
		cout << "V = {";
		for(set<Vertex*, vertex_comp>::iterator it = V.begin(); it != V.end(); ++it){
			cout << (*it)->get_type();
			cout << (*it)->get_v();
			cout << ", ";
		}
		cout << "}" << endl;
		
		
		// Print Edges
		cout << "E = {" << endl;
		for(multiset<Edge*, edge_comp>::iterator it = E.begin(); it != E.end(); ++it){
			Vertex* v1 = (*it)->get_v1();
			Vertex* v2 = (*it)->get_v2();
			int w = (*it)->get_w();
				
			cout << "\t(";
			cout << v1->get_type();
			cout << v1->get_v();
			cout << ", ";
			cout << v2->get_type();
			cout << v2->get_v();
			cout << ", ";
			cout << w << ")," << endl;
		}
		cout << "}" << endl;
		
		
		// Print Connected Components
		cout << "CC = {" << endl;
		for(list<set<Vertex*>*>::iterator it = connected_components.begin(); it != connected_components.end(); ++it){
			set<Vertex*>* cc = *it;
			cout << "\t{";
			for(set<Vertex*>::iterator jt = cc->begin(); jt != cc->end(); ++jt){
				cout << (*jt)->get_type();
				cout << (*jt)->get_v();
				cout << ", ";
			}
			cout << "}" << endl;
		}
		cout << "}" << endl;
		
		// Satisfied Constraints
		cout << "Unsatisfied Constraints: " << num_satisfied_constraints << endl;
		
		// Narrowest Portal
		cout << "Narrowest Portal: " << narrowest_portal << endl;
	}
};



/***********************
 * Auxiliary Functions *
 ***********************/

void read_morty_edges(Graph& G, int num_universes){
	
	assert(num_universes > 0);
	
	for(int i = 1; i <= num_universes; ++i){
		int v1 		= i;
		char type1 	= 'm';
		
		int v2;		cin >> v2;
		char type2 = 'u';
		
		int w = Inf;
		
		G.add_edge(v1, type1, v2, type2, w);		
	}
	
}


void read_universe_edges(Graph& G, int num_portals){
	
	assert(num_portals > 0);
	
	for(int i = 1; i <= num_portals; ++i){
		int v1;		cin >> v1;
		char type1 	= 'u';
		
		int v2;		cin >> v2;
		char type2	= 'u';
		
		int w;		cin >> w;
		
		G.add_edge(v1, type1, v2, type2, w);		
	}
	
}


/********
 * Main *
 ********/

int main(void){
	
	// Input Parameters
	int num_universes	= 0;	cin >> num_universes;
	int num_portals		= 0;	cin >> num_portals;
	
	Graph G(num_universes, num_portals);
	
	
	// Read Morty Edges
	read_morty_edges(G, num_universes);
	
	// Read Universe Edges
	read_universe_edges(G, num_portals);
	
	// Algorithm
	G.set_initialization_completed();
	
	while(! G.constraints_satisfied()){
		G.select_edge();
	}
	
	// Output
	cout << G.get_narrowest_portal() << endl;
}
