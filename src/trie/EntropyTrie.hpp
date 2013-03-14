#include "bit_vector.hpp"
#include "trie.hpp"
#include <vector>
#include <string>
class EntropyTrie
{
private:
	typedef cindex::bit_vector<> vector_type;
	vector_type bitvector_;
	cindex::trie<> trie_;
	std::size_t key_len_;
	std::vector<const uint8_t*> pending_keys_;
	std::size_t pending_key_count_;
	std::size_t n_;
public:
	/**
	 * key_len = length of each key in number of bytes
	 */
	EntropyTrie(std::size_t key_len): key_len_(key_len),pending_key_count_(0){
		
	}
	
	bool insert(const uint8_t* key){
		uint8_t* key_c = new uint8_t[key_len_];
		memcpy(key_c, key, key_len_);
		pending_keys_.push_back(key_c);
		pending_key_count_++;
		return true;
	}
	
	bool insert(std::string str){
		const uint8_t* key_c = reinterpret_cast<const uint8_t*>(str.c_str());
		return insert(key_c);
	}
	
	void flush(){
		trie_.encode(
				bitvector_,
				pending_keys_, key_len_,
				0,
				pending_key_count_
			);
		for (std::size_t i = 0; i < pending_key_count_; i++)
			delete [] pending_keys_[i];
		pending_keys_.clear();
		n_ = pending_key_count_;
		pending_key_count_ = 0;
		bitvector_.compact();
	}
	
	std::size_t locate(const uint8_t* key){
		std::size_t iter = 0;
		std::size_t key_index = trie_.locate(bitvector_,
			iter,key,key_len_,0 ,n_);
		return key_index;
	}
	
	std::size_t locate(std::string str){
		const uint8_t* key_c  = reinterpret_cast<const uint8_t*>(str.c_str());
		return locate(key_c);
	}
	
	virtual ~EntropyTrie(){
		for (std::size_t i = 0; i < pending_key_count_; i++)
			delete [] pending_keys_[i];
	}

};