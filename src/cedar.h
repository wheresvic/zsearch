// cedar -- C++ implementation of Efficiently-updatable Double ARray trie
//  $Id: cedar.h 1363 2013-02-20 02:39:14Z ynaga $
// Copyright (c) 2009-2012 Naoki Yoshinaga <ynaga@tkl.iis.u-tokyo.ac.jp>
#ifndef CEDAR_H
#define CEDAR_H

#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace cedar {
  // type alias
  typedef unsigned int   uint;
  typedef unsigned char  uchar;
  // not a number
  template <typename T> struct nan { enum { NAN1 = -1, NAN2 = -2 }; };
  template <> struct nan <float>   { enum { NAN1 = 0x7f800001, NAN2 = 0x7f800002 }; };
  static const int MAX_ALLOC_SIZE = 1 << 16; // must be divisible by 256
  // dynamic double array
  template <typename value_type,
            const int     NO_VALUE    = nan <value_type>::NAN1,
            const int     NO_PATH     = nan <value_type>::NAN2,
            const bool    SORTED_KEYS = true,
            const uint8_t MAX_TRIAL   = 8,
            const size_t  NUM_TRACKING_NODES = 0>
  class da {
  public:
    enum error_code { CEDAR_NO_VALUE = NO_VALUE, CEDAR_NO_PATH = NO_PATH };
    typedef value_type result_type;
    struct result_pair_type {
      value_type  value;
      size_t      length;  // prefix length
    };
    struct result_triple_type { // for predict ()
      value_type  value;
      size_t      length;  // suffix length
      size_t      id;      // node_id of value
    };
    struct node {
      union { int base; result_type value; }; // neg to store prev empty index
      int  check;                             // neg to store next empty index
      node () : base (0), check (0) {}
      node (const int base_, const int check_) : base (base_), check (check_) {}
    };
    struct ninfo {   // x1.5 update speed; +.25 % memory (8n -> 10n)
      uchar child;   // first child
      uchar sibling; // right sibling (= 0 if not exist)
      ninfo () : child (0), sibling (0) {}
      ninfo (const uchar child_, const uchar sibling_) : child (child_), sibling (sibling_) {}
    };
    struct block {     // a block w/ 256 element
      uint8_t  ehead;  // offset to first empty item
      uint8_t  trial;  // # trial
      uint16_t num;    // # empty element; 0 - 256
      int      prev;   // prev block; 3 bytes
      int      next;   // next block; 3 bytes
      block () : ehead (0), trial (0), num (256), prev (0), next (0) {}
    };
    da () : tracking_node (), _array (0), _ninfo (0), _block (0), _bheadF (0), _bheadC (0), _bheadO (0), _size (0), _capacity (0), _no_delete (false)
    { _initialize (); }
    ~da () { clear (false); }
    size_t tracking_node[NUM_TRACKING_NODES + 1];
    size_t capacity   () const { return static_cast <size_t> (_capacity); }
    size_t size       () const { return static_cast <size_t> (_size); }
    size_t unit_size  () const { return sizeof (node); }
    size_t total_size () const { return sizeof (node) * _size; }
    size_t nonzero_size () const {
      size_t n = 0;
      for (int to = 1; to < _size; ++to)
        if (_array[to].check >= 0) ++n;
      return n;
    }
    size_t num_keys () const {
      size_t n = 0;
      for (int to = 1; to < _size; ++to)
        if (_array[to].check >= 0 && _array[_array[to].check].base == to) ++n;
      return n;
    }
    // interfance
    template <typename T>
    T exactMatchSearch (const char* key, size_t len = 0, size_t from = 0) const {
      union { int i; result_type x; } b;
      size_t pos = 0;
      b.i = _find (key, from, pos, len);
      if (b.i == CEDAR_NO_PATH) b.i = CEDAR_NO_VALUE;
      T result;
      _set_result (&result, b.x, len);
      return result;
    }
    template <typename T>
    size_t commonPrefixSearch (const char* key, T* result, size_t result_len, size_t len = 0, size_t from = 0) const {
      if (! len) len = std::strlen (key);
      size_t num (0), pos (0);
      while (pos < len) {
        union { int i; result_type x; } b;
        b.i = _find (key, from, pos, pos + 1);
        if (b.i == CEDAR_NO_VALUE) continue;
        if (b.i == CEDAR_NO_PATH)  return num;
        if (num < result_len) _set_result (&result[num], b.x, pos);
        ++num;
      }
      return num;
    }
    // predict key from double array
    template <typename T>
    size_t commonPrefixPredict (const char* prefix, T* result, size_t result_len, size_t len = 0, size_t from = 0) const {
      if (! _capacity) _not_supported ("commonPrefixPredict ()");
      if (! len) len = std::strlen (prefix);
      size_t num (0), pos (0);
      for (; pos < len; ++pos) {
        const int to = _array[from].base ^ static_cast <uchar> (prefix[pos]);
        if (_array[to].check != static_cast <int> (from)) return 0;
        from = static_cast <size_t> (to);
      }
      _predict (from, result, result_len, 0, num);
      return num;
    }
    void suffix (char* key, const size_t len, size_t to) {
      key[len] = '\0';
      size_t pos = len;
      while (pos) {
        const int from = _array[to].check;
        key[--pos] = _array[from].base ^ to;
        to = static_cast <size_t> (from);
      }
    }
    result_type traverse (const char* key, size_t& from, size_t& pos, size_t len = 0) const {
      union { int i; result_type x; } b;
      b.i = _find (key, from, pos, len);
      return b.x;
    }
    result_type& update (const char* key, size_t len = 0, result_type val = result_type (0)) {
      size_t from = 0;
      return update (key, from, len, val);
    }
    result_type& update (const char* key, size_t& from, size_t len, result_type val) {
      if (! len) len = std::strlen (key);
      // follow link
      for (size_t pos = 0;; ++pos) {
        const uchar c    = static_cast <uchar> (pos == len ? 0 : key[pos]);
        const int   base = _array[from].base;
        int         to   = base ^ c;
        if (base < 0 || _array[to].check < 0) {
          to = _pop_enode (base, c, static_cast <int> (from));
          _push_sibling (from, base, to, c);
        } else if (_array[to].check != static_cast <int> (from)) {
          to = _resolve (from, base, c);
        }
        if (pos == len) return _array[to].value += val;
        from = static_cast <size_t> (to);
      }
    }
    // easy-going erase () without compression
    void erase (const char* key, size_t len = 0) {
      size_t from = 0;
      erase (key, len, from);
    }
    void erase (const char* key, size_t len, size_t from) {
      union { int i; result_type x; } b;
      if (! len) len = std::strlen (key);
      size_t pos = 0;
      b.i = _find (key, from, pos, len);
      if (b.x == CEDAR_NO_PATH || b.x == CEDAR_NO_VALUE) return;
      int e = _array[from].base ^ 0;
      bool flag = false; // have sibling
      do {
        const node& n = _array[from];
        if ((flag = _ninfo[n.base ^ _ninfo[from].child].sibling))
          _pop_sibling (from, n.base, (n.base ^ e) & 0xff);
        _push_enode (e);
        e = static_cast <int> (from);
        from = static_cast <size_t> (_array[from].check);
      } while (! flag);
    }
    int build (size_t num, const char** key, const size_t* len = 0, const result_type* val = 0) {
      for (size_t i = 0; i < num; ++i)
        update (key[i], len ? len[i] : 0, val ? val[i] : result_type (i));
      return 0;
    }
    template <typename T>
    void dump (T* result, const size_t result_len) {
      size_t num = 0;
      _predict (0, result, result_len, 0, num);
      if (result_len < num) {
        std::fprintf (stderr, "pass dump () with array of length = %ld", num);
        std::exit (1);
      }
    }
    int save (const char* fn, const char* mode = "wb") const {
      // _test ();
      FILE* writer = std::fopen (fn, mode);
      if (! writer) return -1;
      std::fwrite (_array, sizeof (node), static_cast <size_t> (_size), writer);
      std::fclose (writer);
      return 0;
    }
    int open (const char* fn, const char* mode = "rb",
              const size_t offset = 0, size_t size_ = 0) {
      FILE* reader = std::fopen (fn, mode);
      if (! reader) return -1;
      // get size
      if (! size_) {
        if (std::fseek (reader, 0, SEEK_END) != 0) return -1;
        size_ = static_cast <size_t> (std::ftell (reader)) / sizeof (node);
        if (std::fseek (reader, 0, SEEK_SET) != 0) return -1;
        if (size_ <= offset) return -1;
        size_ -= offset;
      }
      // set array
      if (_array) free (_array);
      _array = reinterpret_cast <node*> (std::malloc (sizeof (node) * size_));
      if (! _array)
        { std::fprintf (stderr, "memory allocation failed."); std::exit (1); }
      if (std::fseek (reader, static_cast <long> (offset), SEEK_SET) != 0)
        return -1;
      if (size_ != std::fread (reinterpret_cast <node*> (_array),
                               sizeof (node), size_, reader)) return -1;
      std::fclose (reader);
      _size = static_cast <int> (size_);
      _capacity = 0;
      return 0;
    }
    void set_array (void* p, size_t size_ = 0) { // TODO: immutable
      clear (false);
      _no_delete = true;
      _array = reinterpret_cast <node*> (p);
      _size  = static_cast <int> (size_);
      _capacity = 0;
    }
    void clear (bool reuse = true) {
      if (_array && ! _no_delete) free (_array); _array = 0;
      if (_ninfo && ! _no_delete) free (_ninfo); _ninfo = 0;
      if (_block && ! _no_delete) free (_block); _block = 0;
      _bheadF = _bheadC = _bheadO = _size = _capacity = 0; // *
      if (reuse) _initialize ();
      _no_delete = false;
    }
  private:
    // currently disabled; implement these if you need
    da (const da&);
    da& operator= (const da&);
    node*   _array;
    ninfo*  _ninfo;
    block*  _block;
    int     _bheadF;  // first block of Full;   0
    int     _bheadC;  // first block of Closed; 0 if no Closed
    int     _bheadO;  // first block of Open;   0 if no Open
    int     _size;
    int     _capacity;
    int     _no_delete;
    //
    static void _not_supported (const char* f) {
      std::fprintf (stderr, "%s is not supported in loaded double-array.\n", f);
      std::exit (1);
    }
    template <typename T>
    static void _realloc_array (T*& array, const int size_n, const int size_p) {
      array = reinterpret_cast <T*> (std::realloc (array, sizeof (T) * static_cast <size_t> (size_n)));
      if (! array)
        { std::fprintf (stderr, "memory reallocation failed."); std::exit (1); }
      static const T T0 = T ();
      for (T* p (array + size_p), * const q (array + size_n); p != q; ++p)
        *p = T0;
    }
    void _initialize () { // initilize the first special block
      _realloc_array (_array, 256, _size);
      _realloc_array (_ninfo, 256, _size);
      _realloc_array (_block, 1,   _size);
      for (int i = 1; i < 256; ++i)
        _array[i] = node (i == 1 ? -255 : - (i - 1), i == 255 ? -1 : - (i + 1));
      _size = _capacity = 256; _block[0].ehead = 1; // bug fix for erase
      std::memset (&tracking_node[0], 0, sizeof (size_t) * (NUM_TRACKING_NODES + 1));
    }
    // find key from double array
    int _find (const char* key, size_t& from, size_t& pos, size_t len) const {
      if (! len) len = std::strlen (key);
      // follow edge to get value of key
      for (; pos < len; ++pos) {
        const int to = _array[from].base ^ static_cast <uchar> (key[pos]);
        if (_array[to].check != static_cast <int> (from)) return CEDAR_NO_PATH;
        from = static_cast <size_t> (to);
      }
      // get value
      const node& n = _array[_array[from].base ^ 0];
      return n.check == static_cast <int> (from) ? n.base : CEDAR_NO_VALUE;
    }
    template <typename T>
    void _predict (const size_t from, T* result, const size_t result_len, size_t pos, size_t &num) const {
      uchar c = _ninfo[from].child;
      if (! c) { // found_key
        const size_t to = _array[from].base ^ 0;
        if (from) {
          union { int i; result_type x; } b;
          b.i = _array[to].base;
          if (num < result_len) _set_result (&result[num], b.x, pos, from);
          ++num;
        }
        c = _ninfo[to].sibling;
      }
      while (c) {
        const size_t to = _array[from].base ^ c;
        _predict (to, result, result_len, pos + 1, num);
        c = _ninfo[to].sibling;
      }
    }
    void _set_result (result_type* x, result_type r, size_t = 0, size_t = 0) const
    { *x = r; }
    void _set_result (result_pair_type* x, result_type r, size_t l, size_t = 0) const
    { x->value = r; x->length = l; }
    void _set_result (result_triple_type* x, result_type r, size_t l, size_t from) const
    { x->value = r; x->length = l; x->id = from; }
    void _pop_block (const int bi, int& head_in, const bool last) {
      if (last) { // last one poped; Closed or Open
        head_in = 0;
      } else {
        const block& b = _block[bi];
        _block[b.prev].next = b.next;
        _block[b.next].prev = b.prev;
        // if (bi == head_in) head_in = b.next; // head_in's next
        head_in = b.next; // load balancing; more space needed
      }
    }
    void _push_block (const int bi, int& head_out, const bool empty) {
      block& b = _block[bi];
      if (empty) { // the destination is empty
        head_out = b.prev = b.next = bi;
      } else {
        // most recently pushed
        int& tail_out = _block[head_out].next;
        b.prev = head_out;
        b.next = tail_out;
        tail_out = _block[tail_out].prev = bi;
        // int& tail_out = _block[head_out].prev;
        // b.prev = tail_out;
        // b.next = head_out;
        // tail_out = _block[tail_out].next = bi;
      }
    }
    int _add_block () {
      const int n = _size + 256;
      if (n > _capacity) { // allocate memory if needed
#ifdef USE_EXACT_FIT
        _capacity += _size > MAX_ALLOC_SIZE ? MAX_ALLOC_SIZE : _size;
#else
        while (n > _capacity) _capacity *= 2;
#endif
        _realloc_array (_array, _capacity, _size);           // copy _array
        _realloc_array (_ninfo, _capacity, _size);           // copy _ninfo
        _realloc_array (_block, _capacity >> 8, _size >> 8); // copy _block
      }
      _array[_size] = node (- (_size + 255),  - (_size + 1));
      for (int i = _size + 1; i < n - 1; ++i)
        _array[i] = node (-(i - 1), -(i + 1));
      _array[_size + 255] = node (- (_size + 254),  -_size);
      _push_block (_size >> 8, _bheadO, ! _bheadO); // append to block Open
      _size += 256;
      return (_size >> 8) - 1;
    }
    // transfer block from one start w/ head_in to one start w/ head_out
    void _transfer_block (const int bi, int& head_in, int& head_out) {
      _pop_block  (bi, head_in, bi == _block[bi].next);
      _push_block (bi, head_out, ! head_out && _block[bi].num);
    }
    // pop empty node from block; never transfer the special block (bi = 0)
    int _pop_enode (int base, const uchar label, const int from) {
      const int bi =
        (base < 0) ?
        (_bheadC ? _bheadC : (_bheadO ? _bheadO : _add_block ())) : base >> 8;
      if (base < 0)
        _array[from].base = base = ((bi << 8) ^ _block[bi].ehead) ^ label;
      const int e = base ^ label;
      node&  n = _array[e];
      block& b = _block[bi];
      if (--b.num == 0) {
        if (bi) _transfer_block (bi, _bheadC, _bheadF); // Closed to Full
      } else { // release empty node from empty ring
        _array[-n.base].check = n.check;
        _array[-n.check].base = n.base;
        if ((e & 0xff) == b.ehead)
          b.ehead = static_cast <uint8_t> (-n.check & 0xff); // set ehead
        if (bi && b.num == 1 && b.trial != MAX_TRIAL) // Open to Closed
          _transfer_block (bi, _bheadO, _bheadC);
      }
      // initialize the released node
      if (label) n.check = from, n.base = -1;
      else       n.check = from, n.value = result_type (0);
      return e;
    }
    // push empty node into empty ring
    void _push_enode (const int e) {
      const int bi = e >> 8;
      block& b = _block[bi];
      node&  n = _array[e];
      if (++b.num == 1) { // Full to Closed
        b.ehead = static_cast <uint8_t> (e & 0xff);
        n = node (-e, -e);
        _transfer_block (bi, _bheadF, _bheadC); // Full to Closed
      } else {
        const int next = (bi << 8) ^ b.ehead;
        const int prev = -_array[next].base;
        n = node (-prev, -next);
        _array[prev].check = _array[next].base = -e;
        if (b.num == 2 || b.trial == MAX_TRIAL)  // Closed to Open
          _transfer_block (bi, _bheadC, _bheadO);
        b.trial = 0;
      }
      _ninfo[e] = ninfo (0, 0); // reset ninfo; no child, no sibling
    }
    // push label to from's child
    void _push_sibling (const size_t from, const int base, const int to, const uchar label) {
      uchar* child = &_ninfo[from].child;
      if (base < 0 || (SORTED_KEYS ? label < *child : *child)) {
        _ninfo[to] = ninfo (0, *child);
        *child = label;
      } else {
        if (SORTED_KEYS)
          while (_ninfo[base ^ *child].sibling &&
                 _ninfo[base ^ *child].sibling < label)
            child = &_ninfo[base ^ *child].sibling;
        _ninfo[to] = ninfo (0, _ninfo[base ^ *child].sibling);
        _ninfo[base ^ *child].sibling = label;
      }
    }
    // pop label from from's child
    void _pop_sibling (const size_t from, const int base, const uchar label) {
      uchar* child = &_ninfo[from].child;
      if (*child == label) {
        *child = _ninfo[base ^ label].sibling;
      } else {
        while (_ninfo[base ^ *child].sibling != label)
          child = &_ninfo[base ^ *child].sibling;
        _ninfo[base ^ *child].sibling = _ninfo[base ^ label].sibling;
      }
    }
    // enumerate (equal to or more than one) child nodes
    int _get_child (uchar* const child, const int base, uchar c, const uint label = 256) {
      int nc = 0;
      if (! c)  { child[nc++] = c; c = _ninfo[base ^ c].sibling; } // 0: terminal
      if (SORTED_KEYS)
        while (c && c < label) { child[nc++] = c; c = _ninfo[base ^ c].sibling; }
      if (label < 256) child[nc++] = static_cast <uchar> (label);
      while (c) { child[nc++] = c; c = _ninfo[base ^ c].sibling; }
      return nc;
    }
    // explore new block to settle down
    int _find_place () {
      if (_bheadC) return (_bheadC << 8) ^ _block[_bheadC].ehead;
      if (_bheadO) return (_bheadO << 8) ^ _block[_bheadO].ehead;
      return (_add_block () << 8);
    }
    int _find_place (const uchar* const firstborn, const uchar* const lastborn) {
      const int nc = static_cast <int> (lastborn - firstborn + 1);
      if (int bi = _bheadO) {
        do { // set candidate block
          block& b = _block[bi];
          if (b.num >= nc) { // explore configuration
            int e = (bi << 8) ^ b.ehead;
            do {
              const int base = e ^ *firstborn;
              for (const uchar* p = firstborn; _array[base ^ *++p].check < 0; )
                if (p == lastborn) return e; // no conflict
            } while (((e = -_array[e].check) & 0xff) != b.ehead);
          }
          const int bnext = b.next;
          if (++b.trial == MAX_TRIAL) _transfer_block (bi, _bheadO, _bheadC);
          bi = bnext;
        } while ( _bheadO && bi != _bheadO); // bug fix; do not touch here
      }
      return (_add_block () << 8);
    }
    // resolve conflict on base_n ^ label_n = base_p ^ label_p
    int _resolve (size_t& from_n, const int base_n, const uchar label_n) {
      // examine siblings of conflicted nodes
      const int to_pn  = base_n ^ label_n;
      const int from_p = _array[to_pn].check;
      const int base_p = _array[from_p].base;
      // siblings of current node
      uchar child_p[256]; // <= 255
      const int nc_p = _get_child (child_p, base_p, _ninfo[from_p].child);
      // siblings of incoming node
      uchar child_n[256]; // <= 255
      const int nc_n =
        (nc_p == 1) ? 256  // always replace single node
        : _get_child (child_n, base_n, _ninfo[from_n].child, label_n);
      // search new address
      const bool immigrate = nc_n <= nc_p; // replace siblings of newly added
      const uchar* const firstborn = immigrate ? &child_n[0] : &child_p[0];
      const uchar* const lastborn  = firstborn + (immigrate ? nc_n : nc_p) - 1;
      const int base =
        (firstborn == lastborn ?
         _find_place () : _find_place (firstborn, lastborn)) ^ *firstborn;
      const int bi = base >> 8;
      if (_block[bi].num > 1 && _block[bi].trial == MAX_TRIAL) // bug fix
        _transfer_block (bi, _bheadC, _bheadO); // Closed to Open
      _block[bi].trial = 0; // reset
      // replace & modify empty list
      const int from  = immigrate ? static_cast <int> (from_n) : from_p;
      const int base_ = immigrate ? base_n : base_p;
      if (immigrate && *firstborn == label_n) _ninfo[from].child = label_n;
      _array[from].base = base;
      for (const uchar* p = firstborn; p <= lastborn; ++p) { // to_ => to
        const int to  = _pop_enode (base, *p, from);
        const int to_ = base_ ^ *p;
        ninfo& i = _ninfo[to];
        i.sibling = (p == lastborn ? static_cast <uchar> (0) : *(p + 1));
        if (immigrate && to_ == to_pn) continue; // skip newcomer
        node& n  = _array[to];
        node& n_ = _array[to_];
        if ((n.base = n_.base) > 0 && *p) { // copy base; bugfix
          uchar c = i.child = _ninfo[to_].child;
          do _array[n.base ^ c].check = to; // adjust grand son's check
          while ((c = _ninfo[n.base ^ c].sibling));
        }
        if (! immigrate && to_ == static_cast <int> (from_n))
          from_n = static_cast <size_t> (to); // bug fix
        if (! immigrate && to_ == to_pn) { // the address is immediately used
          n_.check = static_cast <int> (from_n);
          if (label_n) n_.base = -1; else n_.value = result_type (0);
          _push_sibling (from_n, base_n, to_pn, label_n);
        } else {
          _push_enode (to_);
        }
        if (NUM_TRACKING_NODES) // keep the traversed node updated
          for (size_t j = 0; tracking_node[j] != 0; ++j)
            if (tracking_node[j] == static_cast <size_t> (to_))
              { tracking_node[j] = static_cast <size_t> (to); break; }
      }
      return immigrate ? base ^ label_n : to_pn;
    }
    // test the validity of double array for debug
    void _test (const size_t from = 0) const {
      const int base = _array[from].base;
      uchar c = _ninfo[from].child;
      do {
        assert (_array[base ^ c].check == static_cast <int> (from));
        if (c) _test (static_cast <size_t> (base ^ c));
      } while ((c = _ninfo[base ^ c].sibling));
    }
  };
}
#endif
