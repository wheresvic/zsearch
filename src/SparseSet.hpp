#ifndef SPARSESET_H
#define SPARSESET_H
#include <vector>
// Idea from 
// Using Uninitialized Memory for Fun and Profit
// http://research.swtch.com/sparse

// What is cool about this Set is that like a hashset all operation are constant time
// but iterating over element of the set is O(n) and clearing the set is O(1)
// so you can reuse the same instance and just call clear() between runs

// mainly useful for very small (cache line) and sparse sets of data.
class SparseSet {
private:
	// counts the number of elements in dense
	unsigned int n;

	// packed list of the elements in the set, stored in order of insertion. 
	vector<unsigned int> dense;
	
	// maps integers to their indices in dense
	vector<unsigned int> sparse;
public:
	typedef unsigned int* iterator;
	typedef const unsigned int* const_iterator;
	
	SparseSet(unsigned int initialsize = 1453689){
		n = 0;	
		dense.resize(initialsize);
		sparse.resize(initialsize);
	}
	
	~SparseSet(){
		
	}
	
	size_t size(){
		return sparse.size();
	}
	
	/**
	 * check whether i is in the set
	 * you verify that the two arrays point at each other for that element
	 * this should be constant time.
	 */
	bool ismember(unsigned int i){
	  //  if ((i+1) > sparse.size() ){
	  //  	return false;
	  //  }
		//If sparse[i] >= n then i is definetly not in the set and it doesn't matter what sparse[i] is set to.
		//otherwise sparse can have any arbitrary values so we need to verify that  dense[sparse[i]] == i
		return sparse[i] < n && dense[sparse[i]] == i;
	}
	
	/**
	 * Adding a member to the set requires updating both of these arrays:
	 * this should be constant time.
	 */
	void insert(unsigned int i){
		if (!ismember(i)){
		    insert_new(i);
		}
	}
	
	
	/**
	 * Adding a member to the set requires updating both of these arrays:
	 * this should be constant time.
	 */
	void insert_new(unsigned int i){
	   // if ((i+1) > dense.size()){
	   // 	dense.resize(i+1);
	   // }
	   // if ((i+1) > sparse.size()){
	   // 	sparse.resize(i+1);
	   // }
		dense[n] = i;
		sparse[i] = n;
		n++;
	}
	

	
	void remove(unsigned int i){
	   // if(!ismember(i)){
	   // 	return;
	   // } 
	   // unsigned int j = dense[n-1];
	   // dense[sparse[i]] = j;
	   // sparse[j] = sparse[i];
	   // n = n-1;
	}
	
	/**
	 * empty the set .. 
	 * this should be constant time.
	 */
	void clear(){
		// Zeroing n effectively clears dense
		// because the code only ever accesses entries in dense with indices less than n
		// and there's no need to clear sparse[]
		n = 0;
	}
	
	iterator  begin() { return dense.data(); }
    iterator  end() { return dense.data() + n; }
	const_iterator  begin() const { return dense.data(); }
    const_iterator  end()   const { return dense.data() + n; }
};
#endif // SPARSESET_H