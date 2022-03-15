/**********************************************************
 * 2nd Programming Exercise (part 2 of 2)
 * Course: Algorithms and Complexity
 * Winter - 2021
 * 
 * A Branch and Bound approach to the Medieval Fair/Bazar
 * Problem
 * 
 * Note: Unfortunately this implementation is slow.
 * 		For the 1st data set example, it computes the
 * 		correct solution (1850316), but in ~36sec.
 * 
 * Author: Merkouris Papamichail
 * email: m.papamichail@di.uoa.gr
 * Institute: IGP Algorithms, Logic and Discrete Mathematics
 * NKUA, NTUA
 **********************************************************/ 

// I/O  
#include <iostream>  
  
// Containers  
#include <set>  
#include <vector>  
#include <queue>  

// Algorithms
#include <algorithm>

// Misc.  
#include <cassert>  
#include <limits>  
  
  
// Namespace  
using namespace std;  
  
  
// Constants  
// the types of equipment  
const int A = 0;  
const int B = 1;  
const int C = 2;  
  
const int inf = std::numeric_limits<int>::max();  
  
  
struct Equipment{  
    int merchant;   // {0, 1, 2}  
    int type;       // {A, B, C}  
};  
  
struct Bundle{  
    int id;  
      
    Equipment eq;  
    int quantity;  
    int price;  
  
// Operations  
    // print  
    void print(){  
        cout << "\tbundle "       << id             << endl;  
        cout << "\t\tmerch: "     << eq.merchant    << endl;  
        cout << "\t\ttype: "      << eq.type        << endl;  
        cout << "\t\tquantity = " << quantity   << endl;  
        cout << "\t\tprice = "    << price      << endl;  
    }  
};

struct bundle_comp{  
    bool operator()(const Bundle& b1, const Bundle& b2) const{  
		//return s1.num_full_sets > s2.num_full_sets;  
		// sort by price
		return b1.price < b2.price;
	}     
} bcomp;
  
/********* 
 * State * 
 *********/  
  
// a state is a 3x3 array  
// each row represents a merchant,  
// each column a piece of equipment  
// state[m][t] = # {of equipment of type t,  
//                  from the merchant m}  
struct State{  
    int state[3][3];    // {0, 1, 2} x {A, B, C}  
    int num_full_sets;  
    int gold_spent;  
      
    //set<int> bundles_bought;  
	vector<bool> bundles_bought; // represent a set as boolean vector
	int num_bundles;
	
// Constructor  
    State(int nnum_bundles){  
        // Initialize State array  
        state[0][A] = 0; state[0][B] = 0; state[0][C] = 0;  
        state[1][A] = 0; state[1][B] = 0; state[1][C] = 0;  
        state[2][A] = 0; state[2][B] = 0; state[2][C] = 0;  
          
        num_full_sets   = 0;  
        gold_spent      = 0;
		
		num_bundles = nnum_bundles;
		for(int i = 0; i <= nnum_bundles-1; ++i){
			bundles_bought.push_back(false);
		}
    }  
	
	//~State(){
	//	bundles_bought.clear();
	//}
	
// Operations  
    void buy_equipment(Bundle b){  
        // update set of bundles bought  
        // assert bundle not already bought  
        assert(bundles_bought[b.id] == false);  
        bundles_bought[b.id] = true;  
      
        // update inventory  
        state[b.eq.merchant][b.eq.type] += b.quantity;  
      
        // update gold spent  
        gold_spent += b.price;  
      
        // update number of full armor sets  
        int full_set0 = min(state[0][A], min(state[0][B], state[0][C]));  
        int full_set1 = min(state[1][A], min(state[1][B], state[1][C]));  
        int full_set2 = min(state[2][A], min(state[2][B], state[2][C]));  
      
        num_full_sets = full_set0 + full_set1 + full_set2;  
    }  
      
    // print state  
    void print(){  
        // data members  
        cout << "\tnum_full_sets = " << num_full_sets << endl;  
        cout << "\tgold_spent = " << gold_spent << "\n" << endl;  
          
        // print array  
        cout << "\t    A  B  C" << endl;  
        cout << "\t0 : [" << state[0][A] << " " << state[0][B] << " " << state[0][C] << "]" << endl;  
        cout << "\t1 : [" << state[1][A] << " " << state[1][B] << " " << state[1][C] << "]" << endl;  
        cout << "\t2 : [" << state[2][A] << " " << state[2][B] << " " << state[2][C] << "]\n\n" << endl;  
          
        // bundles bought  
        cout << "bundles_bought = [";  
        for(int i = 0; i <= num_bundles-1; ++i){
			cout << bundles_bought[i];
		}
        cout << "]" << endl;  
    }  
};  
  
  
State buy_equipment(State s, Bundle b){  
    State t = s;  
      
    // update set of bundles bought  
    // assert bundle not already bought  
    assert(t.bundles_bought[b.id] == false);  
    t.bundles_bought[b.id] = true;  
      
    // update inventory  
    t.state[b.eq.merchant][b.eq.type] += b.quantity;  
      
    // update gold spent  
    t.gold_spent += b.price;  
      
    // update number of full armor sets  
    int full_set0 = min(t.state[0][A], min(t.state[0][B], t.state[0][C]));  
    int full_set1 = min(t.state[1][A], min(t.state[1][B], t.state[1][C]));  
    int full_set2 = min(t.state[2][A], min(t.state[2][B], t.state[2][C]));  
      
    t.num_full_sets = full_set0 + full_set1 + full_set2;  
      
    return t;  
}  
  
bool feasible(State s, Bundle b){  
    return s.bundles_bought[b.id] == false;  
}  
  
  
struct state_comp{  
    bool operator()(const State& s1, const State& s2) const{  
		//return s1.num_full_sets > s2.num_full_sets;  
		// sort by price
		return s1.gold_spent < s2.gold_spent;
	}     
};  
  
  
/******************** 
 * Branch and Bound * 
 ********************/  
  
typedef priority_queue<State, vector<State>, state_comp> priority_Q;  
  
class BranchBound{  
    vector<Bundle> bundles;  
    int num_bundles;  
    int N;                  // required number of armor sets  
      
    int cheapest_feasible_solution;  
    priority_Q Q;  
    State solution;  
public:  
// Constructor - Destructor  
    BranchBound(vector<Bundle> bbundles, int NN) : 
		bundles(bbundles),
		num_bundles(bundles.size()),
		solution(num_bundles)
    {  
       /* bundles     = bbundles;  
        num_bundles = bundles.size();  
        */
		
	   sort(bundles.begin(), bundles.end(), bcomp);
		N           = NN;  
          
        cheapest_feasible_solution = inf;  
          
        // Initialize priority queue  
        State s(num_bundles);  
        for(int i = 0; i <= num_bundles-1; i++){  
            Q.push(buy_equipment(s, bundles[i]));  
        }  
    }  
      
// accessors  
    int get_cheapest_sol(){  
        return cheapest_feasible_solution;  
    }  
      
// Operations  
      
    // Check if there is a feasible solution  
    // If there is a feasible solution this method  
    // also initializes the cheapest feasible solution.  
   /* bool check_if_feasible_solution(){  
          
        // Buy all equipment  
        State s(num_bundles);  
        for(int i = 0; i <= num_bundles-1; ++i){  
            s.buy_equipment(bundles[i]);  
        }  
          
        // Check if there is a feasible solution  
        bool there_is_feasible = s.num_full_sets >= N;  
        if(there_is_feasible){  
            cheapest_feasible_solution = s.gold_spent;  
            solution = s;  
        }  
          
        return there_is_feasible;  
    }  
	*/
   
	// Greedy
	bool greedy(){
		State s(num_bundles);  
        for(int i = 0; i <= num_bundles-1; ++i){  
            s.buy_equipment(bundles[i]);
			
			if(s.num_full_sets >= N){
				solution					= s;
				cheapest_feasible_solution	= s.gold_spent;
				return true;
			}
        }
        
        return false;
	}
      
    // Branch and Bound Step  
    /*void BB_step(){  
        assert(!Q.empty());  
          
        State current = Q.top(); Q.pop();  
          
        if(current.num_full_sets >= N){  
            if(current.gold_spent < cheapest_feasible_solution){  
                cheapest_feasible_solution  = current.gold_spent;  
                solution                    = current;  
            }
        }  
        else if(current.gold_spent <= cheapest_feasible_solution){  
            for(int i = 0; i <= num_bundles-1; ++i){  
                if(feasible(current, bundles[i])){  
                    State new_state = buy_equipment(current, bundles[i]);  
                    Q.push(new_state);  
                }  
            }  
        }  
    }*/
	
	void BB_step(){  
        assert(!Q.empty());  
          
        State current = Q.top(); Q.pop();  
          
        if(current.num_full_sets >= N){  
            if(current.gold_spent < cheapest_feasible_solution){  
                cheapest_feasible_solution  = current.gold_spent;  
                solution                    = current;  
            }
            return;
        }
        
        
        if(current.gold_spent <= cheapest_feasible_solution){  
            for(int i = 0; i <= num_bundles-1; ++i){  
                if(feasible(current, bundles[i])){  
                    State new_state = buy_equipment(current, bundles[i]);  
                    Q.push(new_state);
					current.bundles_bought[bundles[i].id] = true;
					Q.push(current);
					return;
                }  
            }
        }  
    }
      
    // Branch and Bound Algorithm  
    int BB_method(){
        while(!Q.empty()){  
            BB_step();  
        }  
          
        return cheapest_feasible_solution;  
    }  
  
    // print  
    void print(){  
        // data members  
        cout << "num_bundles = "              	<< num_bundles                  << endl;  
        cout << "N = "                          << N                          	<< endl;  
        cout << "cheapest_feasible_solution = " << cheapest_feasible_solution   << endl;  
          
        // print bundles  
        cout << "bundles = [" << endl;  
        for(int i = 0; i <= num_bundles-1; ++i){  
            bundles[i].print();  
        }  
        cout << "]" << endl;  
          
        // print solution  
        cout << "solution = " << endl;  
        solution.print();  
          
        // print queue  
        // priority queue doesn't allow iteration  
    }  
};  
  
  
/*********************** 
 * Auxiliary Functions * 
 ***********************/  
  
vector<Bundle> read_numbers(int N){  
      
    vector<Bundle> bundles;  
      
    for(int i = 1; i <= N; ++i){  
        Bundle b; b.id = i-1;  
        cin >> b.eq.merchant; b.eq.merchant -= 1;  
          
        char char_type; cin >> char_type;  
        switch(char_type){  
            case 'A': b.eq.type = A; break;  
            case 'B': b.eq.type = B; break;  
            case 'C': b.eq.type = C; break;  
        }  
        cin >> b.quantity;  
        cin >> b.price;  
          
        bundles.push_back(b);  
    }  
      
    return bundles;  
}  
  
  
int main(void){  
      
    int N;  cin >> N;  
    int M;  cin >> M;  
      
    BranchBound bb(read_numbers(M), N);  
    
	// There is at least one feasible solution
	if(bb.greedy() == true){
		bb.BB_method();  
		cout << bb.get_cheapest_sol() << endl;
	}
	// There is not a feasible solution
	else{
		cout << -1 << endl;
	}
      
    return 0;  
}  
