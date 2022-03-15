/**********************************************************
 * 2nd Programming Exercise (part 1 of 2)
 * Course: Algorithms and Complexity
 * Winter - 2021
 * 
 * Author: Merkouris Papamichail
 * email: m.papamichail@di.uoa.gr
 * Institute: IGP Algorithms, Logic and Discrete Mathematics
 * NKUA, NTUA
 **********************************************************/ 


#include <iostream>
#include <vector>
#include <limits>
#include <cassert>
#include <algorithm>

using namespace std;

/*************
 * Constants *
 *************/

const int inf = numeric_limits<int>::max();



/**********************************
 * Longest Increasing Subsequence *
 **********************************/

struct L_entry{
	int index_start;	// the start of the interval
	int index_end;		// the end of the interval
	int LSI_length;		// the length of the LSI in the interval
	int value_first;	// the value of the first element in the LSI
	int value_last;		// the value of the last element in the LSI
	
	// functions
	void print(){
		cout << "\t index_start = "	<< index_start	<< endl;
		cout << "\t index_end = "	<< index_end	<< endl;
		cout << "\t LSI_length = "	<< LSI_length	<< endl;
		cout << "\t value_first = "	<< value_first	<< endl;
		cout << "\t value_last ="	<< value_last	<< endl;
	}
};


bool compatible_entries(L_entry entry_l, L_entry entry_r, int K){
	assert(K >= 0);
	
	return entry_l.value_last < K + entry_r.value_first;
}


bool compatible_entries(L_entry entry_l, L_entry entry_m, L_entry entry_r, int K){
	
	int x = entry_l.value_last - entry_m.value_first + 1;
	
	return ( entry_l.value_last < entry_m.value_first &&
			entry_m.value_last < entry_r.value_first ) ||
			( (0 <= x && x <= K) && (entry_m.value_last + x < entry_r.value_first) );
}


class LSI{
	vector<int> sequence;	// the initial number sequence
	int size_seq;			// the size of the sequence
	
	L_entry** L;			// a 2-D array, L[i][j] contains the LSI
							// of the sub interval [i..j]
public:
// Constructor - Destructor
	// Constructor
	LSI(vector<int> ssequence){
		sequence = ssequence;
		size_seq = sequence.size();
		
		// alloc the 2-D array L
		L = new L_entry*[size_seq];
		for(int i = 0; i <= size_seq-1; ++i){
			L[i] = new L_entry[size_seq];
		}
		
		// initialize L
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = 0; j <= size_seq-1; ++j){
				L[i][j].index_start = 0;
				L[i][j].index_end	= 0;
				L[i][j].LSI_length	= 0;
				L[i][j].value_first	= 0;
				L[i][j].value_last	= 0;
			}
		}
	}
	
	// Destructor
	~LSI(){
		sequence.clear();
		
		for(int i = 0; i <= size_seq-1; ++i){
			delete[] L[i];
		}
		delete[] L;
	}
	
	
	// Accessors
	L_entry get_entry(int i, int j){
		assert(i <= j && i >= 0 && j >= 0 && size_seq-1 >= i && size_seq-1 >= j);
		
		return L[i][j];
	}
	
	// The Algorithm
	L_entry longest_increasing_subsequence(int i, int j){
		assert(i <= j && i >= 0 && j >= 0 && size_seq-1 >= i && size_seq-1 >= j);
		
		// initialize
		vector<int> subseq_lsi;
		vector<int> subseq_ind;
		for(int k = 0; k <= j - i; ++k){
			subseq_lsi.push_back(-1);
			subseq_ind.push_back(0);
		}
		
		// compute
		for(int k = 0; k <= j - i; ++k){
			
			// look for a compatible subseq to increase
			for(int l = 0; l <= k-1; ++l){
				if(sequence[i + l] < sequence[i + k]){
					subseq_lsi[k] = subseq_lsi[l] + 1;
					subseq_ind[k] = l;
				}
			}
			
			// compatible subseq not found
			// we start a new subseq
			if(subseq_lsi[k] == -1){
				subseq_lsi[k] = 1;
				subseq_ind[k] = k;
			}
		}
		
		// debug
		/*cout << "subseq_lsi = [";
		for(int k = 0; k <= j - i; ++k){
			cout << subseq_lsi[k] << ", ";
		}
		cout << "]" << endl;
		*/
		
		// reconstruct the solution
		
		// initialize entry
		L_entry entry;
		entry.index_start	= i;
		entry.index_end		= j;
		
		// find the end of the subseq end
		int pos = 0;
		int max = -inf;
		for(int k = 0; k <= j - i; ++k){
			if(subseq_lsi[k] > max){
				max = subseq_lsi[k];
				pos = k;
			}
		}
		
		// update entry
		entry.LSI_length = max;
		entry.value_last = sequence[i + pos];
		
		// find the start of the subseq
		int p = pos;
		while(subseq_lsi[p] > 1){
			p = subseq_ind[p];
		}
		
		// update entry
		entry.value_first	= sequence[i + p];
		
		//cout << "DEBUG" << endl;
		//entry.print();
		
		// return entry
		return entry;
	}
	
	void fill_array(){
		
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = i; j <= size_seq-1; ++j){
				L[i][j] = longest_increasing_subsequence(i, j);
			}
		}
	
	}
	
	// print
	void print(){
		// print sequence
		cout << "Initial Sequence: ";
		cout << "[";
		for(int i = 0; i <= size_seq-1; ++i){
			cout << sequence[i] << ", ";
		}
		cout << "]\n" << endl;
		
		
		// print L matrix
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = i; j <= size_seq-1; ++j){
				cout << "L[" << i << ", " << j << "] =" << endl;
				L[i][j].print();
			}
		}
	}
};



/*******************
 * Gonverment Lies *
 *******************/

class GovLies{
	LSI lsi;
	int size_seq;
	
	int K;
	
	int** L;
	
public:
// Constructor - Destructor
	// Constructor
	GovLies(vector<int> init_seq, int KK) : lsi(init_seq){
		// fill lsi array
		lsi.fill_array();
		
		size_seq 	= init_seq.size();
		K			= KK;
		
		// alloc the 2-D array L
		L = new int*[size_seq];
		for(int i = 0; i <= size_seq-1; ++i){
			L[i] = new int[size_seq];
		}
		
		// initialize L
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = 0; j <= size_seq-1; ++j){
				L[i][j] = 0;
			}
		}
	}
	
	// Destructor
	~GovLies(){
		for(int i = 0; i <= size_seq-1; ++i){
			delete[] L[i];
		}
		delete[] L;
	}
	
	// fill array
	void fill_array(){
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = i; j <= size_seq-1; ++j){
				// initialize
				vector<int> solutions; solutions.push_back(-inf);
				
				// L[i][j] = LSI[0][i-1]
				if(i >= 1){
					solutions.push_back(lsi.get_entry(0, i-1).LSI_length);
				}
				
				// L[i][j] = LSI[j+1][size_seq-1]
				if(j <= size_seq-2){
					solutions.push_back(lsi.get_entry(j+1, size_seq-1).LSI_length);
				}
					
				// if feasible
				// L[i][j] = LSI[0][i-1] + (LSI[j+1][size_seq-1] + x)
				if(	i >= 1 && j <= size_seq-2
					&&
					compatible_entries(
						lsi.get_entry(0, i-1),
						lsi.get_entry(j+1, size_seq-1), 
						K
				)){
					solutions.push_back(
						lsi.get_entry(0, i-1).LSI_length
						+
						lsi.get_entry(j+1, size_seq-1).LSI_length
					);
				}
				
				// if feasible
				// L[i][j] = LSI[0][i-1] + (LSI[i][j] + x)
				if(	i >= 1 && j <= size_seq-1
					&&
					compatible_entries(
						lsi.get_entry(0, i-1),
						lsi.get_entry(i, j), 
						K
				)){
					solutions.push_back(
						lsi.get_entry(0, i-1).LSI_length
						+
						lsi.get_entry(i, j).LSI_length
					);
				}
				
				// if feasible
				// L[i][j] = LSI[i][j] + (LSI[j+1][size_seq-1] + x)
				if(	j <= size_seq-2
					&&
					compatible_entries(
						lsi.get_entry(i, j),
						lsi.get_entry(j+1, size_seq-1), 
						K
				)){
					solutions.push_back(
						lsi.get_entry(i, j).LSI_length
						+
						lsi.get_entry(j+1, size_seq-1).LSI_length
					);
				}
				
				// if feasible
				// L[i][j] = LSI[0][i-1] + (LSI[i][j] + x) + LSI[j+1][size_seq-1]
				if(	i >= 1 && j <= size_seq-2
					&&
					compatible_entries(
						lsi.get_entry(0, i-1),
						lsi.get_entry(i, j),
						lsi.get_entry(j+1, size_seq-1), 
						K
				)){
					solutions.push_back(
						lsi.get_entry(0, i-1).LSI_length
						+
						lsi.get_entry(i, j).LSI_length
						+
						lsi.get_entry(j+1, size_seq-1).LSI_length
					);
				}
				
				L[i][j] = *max_element(solutions.begin(), solutions.end());
			}
		}
	}
	
	// return max LSI
	int max_lsi(){
		
		int max_lsi = -inf;
		for(int i = 0; i <= size_seq-1; ++i){
			for(int j = i + 1; j <= size_seq-1; ++j){
				if(L[i][j] > max_lsi){
					max_lsi = L[i][j];
				}
			}
		}
		
		return max_lsi;
	}
	
	// print
	void lsi_print(){
		lsi.print();
	}
	
	void print(){
		cout << "L = " << endl;
		for(int i = 0; i <= size_seq-1; ++i){
			cout << "[";
			for(int j = 0; j <= size_seq-1; ++j){
				cout << L[i][j] << " ";
			}
			cout << "]" << endl;
		}
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


int main(void){
	
	int N;	cin >> N;
	int K;	cin >> K;
	
	GovLies gl(read_numbers(N), K);
	
	gl.fill_array();
	//gl.lsi_print();
	//gl.print();
	cout << gl.max_lsi() << endl;
	
	return 0;
}
