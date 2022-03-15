/************************************************
 * Algorithms & Complexity
 * 3rd Programming Exercise, part 1/2
 * ---------------------------------------------
 * Given a set of "distances" D and a set of
 * queries Q, for each q in Q we check weather
 * the Diophantine equation
 * 	X*D = q
 * has positive solutions.
 * We use Dynamic Programming.
 * 
 * Complexity: O(|D| * |Q| * M),
 * 	where M is the max value of Q.
 * 
 * Author: Merkouris Papamichail
 * email: m.papamichail@di.uoa.gr
 * Fall, 2021
 ************************************************/

// Libraries
#include <cassert>
#include <list>
#include <set>
#include <map>
#include <iostream>


// namespace
using namespace std;


// constants
//const int infinity = numeric_limits<int>::max();

const string yes_ans = "YES";
const string no_ans = "NO";

/********************
 * Custom Functions *
 ********************/

// Euclid's Algorithm
int gcd(int a, int b){
	assert(a >= 0 && b >= 0);
	
	// force a >= b
	if(b > a) return gcd(b, a);
	
	// base case
	if(b == 0)	return a;
	else		return gcd(b, a % b);
}


int generalized_gcd(list<int> nums){
	assert(!nums.empty());
	
	// trivial case
	if(nums.size() == 1)	return nums.back();
	
	int a = nums.front();	nums.pop_front();
	return gcd(a, generalized_gcd(nums));
}


bool is_divided(int q, set<int>& divisors){
	assert(q >= 0);
		
	for(set<int>::iterator it = divisors.begin(); it != divisors.end(); ++it){
		if(q % *it == 0) return true;
	}
	
	return false;
}


bool look_up(int q, int min_dist, set<int>& dists, map<int, bool>& computed_vals){
	
	if(q < min_dist){
		computed_vals[q] = false;
		return false;
	}
	
	if(computed_vals.find(q) != computed_vals.end()){
		return computed_vals[q];
	}
	
	bool max_val = false;
	for(set<int>::iterator it = dists.begin(); it != dists.end(); ++it){
		bool val = look_up(q - *it, min_dist, dists, computed_vals);
		max_val = max_val || val;
	}
	
	computed_vals[q] = max_val;
	return max_val;
}



/********
 * Main *
 ********/

int main(void){
	
	// Data
	int num_dists, num_queries; 
	set<int> dists;
	list<int> queries;
	
	
	
	/**************
	 * Read Input *
	 **************/
	
	cin >> num_dists;
	cin >> num_queries;
	
	// Read distances
	for(int i = 1; i <= num_dists; i++){
		int n; cin >> n;
		dists.insert(n);
	}
	
	// Read queries
	for(int i = 1; i <= num_queries; i++){
		int q; cin >> q;
		queries.push_back(q);
	}
	
	
	
	/*************
	 * Algorithm *
	 *************/
	
	// computer the gcd of the distances
	list<int> list_dists;
	for(set<int>::iterator it = dists.begin(); it != dists.end(); ++it){
		list_dists.push_back(*it);
	}
	int gcd = generalized_gcd(list_dists);
	
	
	// initialize DP
	map<int, bool> computed_vals;
	computed_vals[0] = true;
	for(set<int>::iterator it = dists.begin(); it != dists.end(); ++it){
		computed_vals[*it] = true;
	}
	
	
	// min distance
	int min_dist = *(dists.begin());
	
	
	// Check queries
	for(list<int>::iterator it = queries.begin(); it != queries.end(); ++it){
		
		// get the next querie
		int q = *it;
		
		// simple case
		if(is_divided(q, dists)){
			computed_vals[q] = true;
			cout << yes_ans << endl;
			continue;
		}
		
		// the Diophantine equation doesn't have a solution
		if(q % gcd != 0){
			computed_vals[q] = false;
			cout << no_ans << endl;
			continue;
		}
		
		// if the Diophantine equation has a solution
		// we compute a Dynamic Programme to check if there are
		// positive solutions
		if(look_up(q, min_dist, dists, computed_vals) == true)	cout << yes_ans << endl;
		else													cout << no_ans << endl;
	}
	
	return 0;
}
