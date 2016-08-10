/*
 * HashIndexer.h
 */

#ifndef _HASH_INDEXER_H_
#define _HASH_INDEXER_H_

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "Patch.h"
#include "MinHash.h"

 //
 // PatchInfo
 //
struct PatchInfo
{
	size_t	imIdx	= 0;
	int		iWinX	= 0;
	int		iWinY	= 0;
	double	iW		= 1.0;

	PatchInfo();
	PatchInfo(const size_t imIdx_, const int iWinX_, const int iWinY_);
	PatchInfo(const size_t imIdx_, const cv::Point& iWin);
};

std::ostream& operator<<(std::ostream& out, const PatchInfo& info);
std::ostream& operator<<(std::ostream& out, const std::vector<PatchInfo>& v);

struct equal_to_hash_info_ { bool operator()(const PatchInfo& info1, const PatchInfo& info2) const; };
struct hash_hash_info_ { size_t operator()(const PatchInfo& info) const; };

//
// Binary IO
//
std::ofstream write_file(const std::string& filePath);
std::ifstream read_file(const std::string& filePath);
void write(std::ofstream& fout, const std::string& str);
void read(std::ifstream& fin, std::string& str);
void write(std::ofstream& fout, const std::unordered_map<std::string, double>& imMap);
void read(std::ifstream& fin, std::unordered_map<std::string, double>& imMap);
void write(std::ofstream& fout, const PatchInfo& info);
void read(std::ifstream& fin, PatchInfo& info);
void write(std::ofstream& fout, const std::vector<PatchInfo>& v);
void read(std::ifstream& fin, std::vector<PatchInfo>& v);
void write(std::ofstream& fout, const std::vector<std::string>& v);
void read(std::ifstream& fin, std::vector<std::string>& v);

template<typename val_type>
void write(std::ofstream& fout, const std::vector<val_type>& v)
{
	size_t vecSize = v.size();
	fout.write((char *)&vecSize, sizeof(size_t));
	for (const val_type& val : v)
		fout.write((char *)&val, sizeof(val_type));
}

template<typename val_type>
void read(std::ifstream& fin, std::vector<val_type>& v)
{
	size_t vecSize;
	fin.read((char *)&vecSize, sizeof(size_t));

	v.clear();
	v.reserve(vecSize);
	for (size_t i = 0; i < vecSize; i++)
	{
		val_type val;
		fin.read((char *)&val, sizeof(val_type));
		v.push_back(val);
	}
}

template<typename hash_type>
void writeTuple(std::ofstream& fout, const typename hash_type::Tuple& t)
{
	for (size_t i = 0; i < hash_type::S; i++)
		fout.write((char *)&t[i], sizeof(hash_type::uint_type));
}

template<typename hash_type>
void readTuple(std::ifstream& fin, typename hash_type::Tuple& t)
{
	for (size_t i = 0; i < hash_type::S; i++)
		fin.read((char *)&t[i], sizeof(hash_type::uint_type));
}

//
// HashIndexer class definition
//
template<typename hash_type, typename idx_type = std::string>
class HashIndexer
{
	inline uint16_t winIdx(int iWinX, int iWinY) { return (iWinX << 8) | iWinY; }
	inline uint16_t winIdx(const PatchInfo& info) { return winIdx(info.iWinX, info.iWinY); }

	size_t getIdx(const idx_type& imIdx)
	{
		auto it = _imMap.find(imIdx);
		size_t ind;

		if (it == _imMap.end())
		{
			ind = _imVec.size();
			_imVec.push_back(imIdx);
			_imMap.emplace(imIdx, ind);
		}
		else
			ind = it->second;
		
		return ind;
	}

public:
	typedef typename hash_type::Tuple tuple_type;
	typedef typename hash_type::TupleVector tuple_vector_type;

	void insert_patch(PatchInfo& info, const tuple_type& tuple)
	{
		auto it = _tupleMap.find(tuple);

		if (it == _tupleMap.end())
		{
			// new info_vec
			std::unordered_map<uint16_t, std::vector<PatchInfo>> infoMap;
			std::vector<PatchInfo> infoVec;
			infoVec.push_back(info);

			infoMap.emplace(winIdx(info), move(infoVec));
			_tupleMap.emplace(move(tuple), infoMap);
		}
		else
		{
			uint16_t ind = winIdx(info);
			auto i = it->second.find(ind);
			if (i == it->second.end())
			{
				// new info_vec
				std::vector<PatchInfo> infoVec;
				infoVec.push_back(info);
				it->second.emplace(ind, infoVec);
			}
			else
				i->second.push_back(info);
		}
	}

	void insert(const idx_type& imIdx, PatchInfo& info, const tuple_type& tuple)
	{
		info.imIdx = getIdx(imIdx);
		insert_patch(info, tuple);
	}

	void insert(const idx_type& imIdx, PatchInfo& info, const tuple_vector_type& tuples)
	{
		info.imIdx = getIdx(imIdx);
		for (const tuple_type& t : tuples) insert_patch(info, t);
	}
	
	std::vector<std::pair<idx_type, double>> retrieve(const std::vector<std::pair<PatchInfo, tuple_vector_type>>& input)
	{
		std::vector<double> imVec; 
		imVec.resize(_imVec.size());

		for (const auto& kv : input)
		{
			for (const tuple_type& t : kv.second)
			{	
				auto it = _tupleMap.find(t);
				if (it != _tupleMap.end()) 
				{
					const PatchInfo& info_(kv.first);

					for (const PatchInfo& info : it->second[winIdx(info_.iWinX - 1,	 info_.iWinY	)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX - 1,	 info_.iWinY - 1)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX - 1,	 info_.iWinY + 1)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX,		 info_.iWinY	)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX,		 info_.iWinY - 1)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX,		 info_.iWinY + 1)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX + 1,	 info_.iWinY	)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX + 1,	 info_.iWinY - 1)])	imVec[info.imIdx] += info_.iW;
					for (const PatchInfo& info : it->second[winIdx(info_.iWinX + 1,	 info_.iWinY + 1)])	imVec[info.imIdx] += info_.iW;
				}
			}
		}

		// sort
		std::vector<std::pair<idx_type, double>> vec;
		vec.reserve(imVec.size());
		for (size_t i = 0; i < imVec.size(); i++)
			vec.push_back(std::pair<idx_type, double>(_imVec[i], imVec[i]));

		std::sort(vec.begin(), vec.end(), [](const std::pair<idx_type, double>& a, const std::pair<idx_type, double>& b) { return b.second < a.second; });
		return vec;
	}

	void write(const std::string& filePath) const
	{
		// std::ofstream fout_ = write_file(flagFileName(filePath));
		std::ofstream fout_ = write_file(filePath);

		size_t tupleNum = _tupleMap.size();
		::write(fout_, _imVec);

		fout_.write((char *)&tupleNum, sizeof(size_t));
		for (const auto& kv : _tupleMap)
			::writeTuple<hash_type>(fout_, kv.first);

		for (const auto& kv : _tupleMap)
		{
			// std::ofstream fout = write_file(tupleFileName(filePath, kv.first));
			// ::write(fout, kv.second);
			// fout.close();
			/*
			::write(fout_, kv.second);
			*/

			size_t infoVecNum = kv.second.size();
			fout_.write((char *)&infoVecNum, sizeof(size_t));
			for (const auto& ikv : kv.second)
			{
				fout_.write((char *)&ikv.first, sizeof(uint16_t));
				::write(fout_, ikv.second);
			}
		}
		fout_.close();
	}

	void read(const std::string& filePath)
	{
		clear();

		// std::ifstream fin_ = read_file(flagFileName(filePath));
		std::ifstream fin_ = read_file(filePath);

		size_t tupleNum;
		::read(fin_, _imVec);
		_imMap.reserve(_imVec.size());
		for (size_t i = 0; i < _imVec.size(); i++)
			_imMap.emplace(_imVec[i], i);

		fin_.read((char *)&tupleNum, sizeof(size_t));
		_tupleMap.reserve(tupleNum);

		std::vector<tuple_type> tv;
		tv.reserve(tupleNum);
		for (size_t i = 0; i < tupleNum; i++)
		{
			tuple_type t;
			::readTuple<hash_type>(fin_, t);
			tv.push_back(t);
		}

		for (const auto& t : tv)
		{
			// std::ifstream fin = read_file(tupleFileName(filePath, t));
			// std::vector<PatchInfo> iv;
			// ::read(fin, iv);
			// _tupleMap.emplace(t, move(iv));
			// fin.close();
			/*
			std::vector<PatchInfo> iv;
			::read(fin_, iv);
			_tupleMap.emplace(t, move(iv));
			*/

			size_t infoVecNum;
			fin_.read((char *)&infoVecNum, sizeof(size_t));

			std::unordered_map<uint16_t, std::vector<PatchInfo>> infoMap;
			infoMap.reserve(infoVecNum);
			for (size_t i = 0; i < infoVecNum; i++)
			{
				uint16_t ind;
				std::vector<PatchInfo> infoVec;
				fin_.read((char *)&ind, sizeof(uint16_t));
				::read(fin_, infoVec);

				infoMap.emplace(ind, infoVec);
			}
			_tupleMap.emplace(t, infoMap);
		}
		fin_.close();
	}

	void clear() 
	{
		_tupleMap.clear();
		_imVec.clear();
		_imMap.clear();
	}

protected:
	std::unordered_map<tuple_type, std::unordered_map<uint16_t, std::vector<PatchInfo>>, hash_tuple_<hash_type>, equal_to_tuple_<hash_type>> _tupleMap;
	std::vector<idx_type> _imVec;
	std::unordered_map<idx_type, size_t> _imMap;

	template<typename unit_type>
	static std::string padding(const unit_type& n, int len = 20) 
	{
		std::stringstream ss;
		ss << setw(len) << setfill('0') << n;
		return ss.str();
	}
	static std::string flagFileName(const std::string& filePath) { return filePath + "/" + "_"; } 
	static std::string tupleFileName(const std::string& filePath, const tuple_type& t)
	{
		std::string str = filePath + "/";
		for (size_t i = 0; i < hash_type::S; i++)
			str += padding(t[i]);
		return str;
	}
};

#endif  // _HASH_INDEXER_H_