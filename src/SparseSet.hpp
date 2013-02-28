#ifndef SPARSESET_H
#define SPARSESET_H
#include <vector>
#include <algorithm>
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
		return n;
	}
	
	bool empty () {
	    return n == 0;
    }

    /**
	 * Look at the last added element without removing it.
	 * Running time O(1).
	 */
	unsigned int peek () {
		return dense[n-1];
	}
	
    void growBuffers(size_t minSize) {
		size_t newLength = std::max(sparse.size(),minSize) *2;
	    sparse.resize(newLength);
		dense.resize(newLength);
    }

	/**
	 * check whether i is in the set
	 * you verify that the two arrays point at each other for that element
	 * Running time O(1)
	 */
	bool ismember(unsigned int i){
	    if (i >= sparse.size() ){
	    	return false;
	    }
		// If sparse[i] >= n then i is definetly not in the set and
		// it doesn't matter what sparse[i] is set to.
		// otherwise sparse can have any arbitrary values 
		// so we need to verify that  dense[sparse[i]] == i
		const unsigned int v = sparse[i];
		return v < n && dense[v] == i;
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
	    if (i >= sparse.size()){
			growBuffers(i);
	    }
		dense[n] = i;
		sparse[i] = n;
		n++;
	}
	
	/**
	 * Remove an elementfrom the set. Running time O(1).
	 * Warning Using this method moves the last inserted element
	 * into the position of the removed element.
	 * Breaking the insertion order invariant - if you rely on that.
     */		
	void remove(unsigned int i){
	    if(!ismember(i)){
	    	return;
	    } 
	    unsigned int j = dense[n-1];
	    dense[sparse[i]] = j;
	    sparse[j] = sparse[i];
	    n--;
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
