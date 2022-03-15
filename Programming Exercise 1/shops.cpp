/**********************************************************
 * 1st Programming Exercise (part 2 of 2)
 * Course: Algorithms and Complexity
 * Fall - 2021
 * 
 * We reduce the given problem to the problem of find
 * two numbers that sum up to K, given an array of numbers A.
 * 
 * In order to do that we calculate all the "sums" of every
 * interval and store them to a map structure <Value, Interval>
 * H.
 * 
 * Then for each interval Int1 \in H we look for another
 * interval Int2 \in H, where val(Int1) + val(Int2) = K.
 * These lookups need only O(log(N^2)) = O(2log(N)) = O(log(N))
 * time, resulting to O(N^2 * log(N)) total time.
 * 
 * Author: Merkouris Papamichail
 * email: m.papamichail@di.uoa.gr
 * Institute: PGSP Algorithms, Login and Discrete Mathematics
 * NKUA, NTUA
 * 
 **********************************************************/ 
#include <iostream>

// containers
#include <map>
#include <vector>

// c essentials
#include <cassert>

// misc
#include <limits>		// infinity



using namespace std;

const int Inf = std::numeric_limits<int>::max();


struct Interval{
	int begin;
	int end;
	int distance;
	int value;

// Constructor - Destructor	
	Interval(int b, int e, int d, int v){
		begin 		= b;
		end			= e;
		distance	= d;
		value		= v;
	}
	
// print
	void print(){
		cout << "(" 
		<< begin 	<< ", "
		<< end 		<< ", "
		<< distance	<< ", "
		<< value
		<< ")";
	}
};


bool compatible_intervals(Interval& Int1, Interval& Int2){
	return Int1.end < Int2.begin ||
			Int2.end < Int1.begin;
}


typedef multimap<int, Interval> HashMap;

class ShopCalculator{
private:
// Data Members
	HashMap 	H;
	int 		K;
	vector<int> values;


// Private Methods
	void compute_interval_sums(){
		
		// add to the hashtable a dummy interval
		// to include the case of a single interval
		H.insert( make_pair( 0, Interval(-2, -1, 0, 0) ) );
		
		for(int i = 0; i <= values.size() - 1; i++){
			for(int j = i; j <= values.size() - 1; j++){
				
				int s = 0;
				for(int l = i; l <= j; l++){
					s += values[l];
				}
				
				H.insert(  make_pair( s, Interval(i, j, j - i + 1, s) )  );
			}
		}
	}
public:
	// Constructor / Destructor
	ShopCalculator(vector<int> vvalues, int KK){
		values 	= vvalues;
		K		= KK;
		
		// fill the HashMap H
		compute_interval_sums();
	}
	
	// Algorithm
	int find_min_interval(){
		
		int min_dist = Inf;
		for(HashMap::iterator it = H.begin(); it != H.end(); ++it){
			
			int L = K - it->first;
			HashMap::iterator pos = H.find(L);
			if(pos == H.end()) continue;			// pair not found
													// continue to the next interval
			
			// else a pair has been found
			// look for a compatible pair
			while(
					! compatible_intervals(it->second, pos->second) &&
					pos->first == L
				 ){
				++pos;
			}
			// check if pos is still feasible
			if(pos->first != L) continue;
			
			// is this pair better ?
			if(it->second.distance + pos->second.distance < min_dist){
				min_dist = it->second.distance + pos->second.distance;
			}
		}
		
		if(min_dist < Inf)	return min_dist;
		else				return -1;
	}
	
	// Print
	void print(){
		// print K
		cout << "K = " << K << endl;
		
		// print values
		cout << "values = [";
		for(int i = 0; i <= values.size() - 1; ++i){
			cout << values[i] << ", ";
		}
		cout << "]" << endl;
		
		// print hash table
		cout << "H = {" << endl;
		for(HashMap::iterator it = H.begin(); it != H.end(); ++it){
			cout << "\t<" << it->first << ", ";
			it->second.print();
			cout << ">" << endl;
		}
		cout << "}" << endl;
	}
};




/***********************
 * Auxiliary Functions *
 ***********************/

vector<int> read_numbers(int N){
	
	vector<int> nums;
	
	for(int i = 1; i <= N; ++i){
		int n;	cin >> n;
		nums.push_back(n);
	}
	
	return nums;
}


/********
 * Main *
 ********/

int main(void){
	
	int N;	cin >> N;
	int K;	cin >> K;
	
	ShopCalculator S(read_numbers(N), K);
	
	cout << S.find_min_interval() << endl;
}

