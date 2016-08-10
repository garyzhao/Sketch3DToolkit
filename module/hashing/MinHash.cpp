/*
 * MinHash.cpp
 */

#include "MinHash.h"
#include "City.h"
#include "Spooky.h"

//
// 64bit HashInfo_
//
const HashInfo64__::uint_type HashInfo64__::Seeds[HashInfo64__::MAX_NUM_SEED] = { 962, 371, 375, 849, 237, 971, 409, 539, 196, 159, 888, 422, 377, 126, 220, 874, 212, 705, 397, 145 };
const HashInfo64__::func_type HashInfo64__::Funcs[HashInfo64__::MAX_NUM_FUNC] = {
	[](const char* key, size_t length, uint64_t seed) { return CityHash64WithSeed(key, length, seed); },
	[](const char* key, size_t length, uint64_t seed) { return SpookyHash::Hash64((const void*)key, length, seed); }
};