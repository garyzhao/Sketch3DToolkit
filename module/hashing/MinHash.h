/*
 * MinHash.h
 * Implementation of the protected/public functions of the class of MinHash, which offers a set of hashing methods.
 */

#ifndef _MIN_HASH_H_
#define _MIN_HASH_H_

#include <iostream>
#include <utility>
#include <array>
#include <vector>
#include <algorithm>
#include <cassert>
#include "Platform.h"

typedef std::pair<uint64_t, uint64_t> uint128_t;

//
// HashInfo_: wrapper structure including hash type and value
// MinHash_: template MinHash class with K S-tuples
//
template<size_t MAX_NUM_SEED_, size_t MAX_NUM_FUNC_, typename uint_t_>
struct HashInfo_
{
	using uint_type = uint_t_;
	using func_type = uint_type(*)(const char*, size_t, uint_type);

	static constexpr size_t MAX_NUM_SEED = MAX_NUM_SEED_;
	static constexpr size_t MAX_NUM_FUNC = MAX_NUM_FUNC_;
	static const uint_type Seeds[MAX_NUM_SEED];
	static const func_type Funcs[MAX_NUM_FUNC];
};

template<int K_, int S_, typename HashInfo_>
struct MinHash_
{
	static_assert(K_ > 0 && S_ > 0, "Invalid K or S value for MinHash_.");
	static_assert(K_ <= HashInfo_::MAX_NUM_SEED, "Not enough pre-defined hash seeds for MinHash_.");
	static_assert(S_ <= HashInfo_::MAX_NUM_FUNC, "Not enough pre-defined hash functions for MinHash_.");

	static constexpr int K = K_;
	static constexpr int S = S_;

	typedef typename HashInfo_::uint_type uint_type;
	typedef std::array<uint_type, S> Tuple;
	typedef std::array<Tuple, K> TupleVector;

	template<typename T>
	static void binarize_(const T* src, T* out, size_t len, double th) {
		std::vector<T> vec(src, src + len);
		std::sort(vec.begin(), vec.end());
		T val = vec[size_t(len * (1 - th))];
		for (int i = 0; i < len; i++)
			out[i] = src[i] < val ? 0 : 1;
	}

	template<typename vec_t, typename len_t>
	static TupleVector Hash(const vec_t* vec, len_t len, double th = 0.2) {

		assert(len > 0);
		assert(th > 0 && th <= 1);

		// binarize the input vector
		vector<vec_t> vec_binary(len);
		binarize_(vec, &vec_binary[0], len, th);
	
		// compute K S-tuple min-hash outputs
		TupleVector out;
		for (int i = 0; i < K; i++) {

			// initialize
			for (int k = 0; k < S; k++)
				out[i][k] = std::numeric_limits<HashInfo_::uint_type>::max();

			// compute S-tuple min-hash value
			for (len_t j = 0; j < len; j++) {
				if (vec_binary[j] != 0) {
					for (int k = 0; k < S; k++) {
						typename HashInfo_::uint_type val = HashInfo_::Funcs[k]((char*)&j, sizeof(j) / sizeof(char), HashInfo_::Seeds[i]);
						if (val < out[i][k])
							out[i][k] = val;
					}
				}
			}
		}
		
		return std::move(out);
	}
};

//
// Helper functions for MinHash tuples.
//
template<typename hash_type>
struct equal_to_tuple_
{
	bool operator()(const typename hash_type::Tuple& t1, const typename hash_type::Tuple& t2) const {
		for (int i = 0; i < hash_type::S; i++)
			if (t1[i] != t2[i]) return false;
		return true;
	}
};

// refer to boost::hash_combine
template<typename T>
void hash_combine(size_t& seed, const T& v) {
	seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename hash_type>
struct hash_tuple_ 
{
	size_t operator()(const typename hash_type::Tuple& t) const {
		size_t val = 0;
		for (int i = 0; i < hash_type::S; i++) 
			hash_combine(val, t[i]);
		return val;
	}
};

template<class Ty_, size_t Size_>
std::ostream& operator<<(std::ostream& out, const std::array<Ty_, Size_>& input)
{
	for (const auto& val : input) out << "|" << val;
	return out;
}

//
// Pre-defined 32bit, 64bit and 128bit HashInfo__ templates and MinHash functions.
//
using HashInfo32__	= HashInfo_<20, 2, uint32_t>;
using HashInfo64__	= HashInfo_<20, 2, uint64_t>;
using HashInfo128__	= HashInfo_<20, 2, uint128_t>;

using MinHash32		= MinHash_<20, 2, HashInfo32__>;
using MinHash64		= MinHash_<20, 2, HashInfo64__>;
using MinHash128	= MinHash_<20, 2, HashInfo128__>;

//
// Helper functions check overflow to safely cast numeric values. 
//
template<typename I, typename O>
O numeric_cast_(const I& src) {
	assert(src <= std::numeric_limits<O>::max() && src >= std::numeric_limits<O>::min());
	return static_cast<O>(src);
}

template<typename O> O int_cast_(const int& src) { return numeric_cast_<int, O>(src); }

#endif  // _MIN_HASH_H_