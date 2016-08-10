/*
 * HashIndexer.cpp
 */

#include "HashIndexer.h"

using namespace std;
using namespace cv;

//
// Helper functions for PatchInfo.
//
PatchInfo::PatchInfo(const size_t imIdx_, const int iWinX_, const int iWinY_) : imIdx(imIdx_), iWinX(iWinX_), iWinY(iWinY_) {}
PatchInfo::PatchInfo(const size_t imIdx_, const Point& iWin) : PatchInfo(imIdx_, iWin.x, iWin.y) {}
PatchInfo::PatchInfo() : PatchInfo(0, 0, 0) {}

ostream& operator<<(ostream& out, const PatchInfo& info) 
{
	out << info.imIdx << ": " << "[" << info.iWinX << "," << info.iWinY << "]";
	return out;
}

ostream& operator<<(ostream& out, const vector<PatchInfo>& v) 
{
	out << "{";
	for (const auto& e : v)
		out << "|" << e;
	out << "}";
	return out;
}

bool equal_to_hash_info_::operator()(const PatchInfo& info1, const PatchInfo& info2) const
{
	return (info1.imIdx == info2.imIdx && info1.iWinX == info2.iWinX && info1.iWinY == info2.iWinY);
}

size_t hash_hash_info_::operator()(const PatchInfo& info) const
{
	size_t val = 0;
	hash_combine(val, info.imIdx);
	hash_combine(val, info.iWinX);
	hash_combine(val, info.iWinY);
	return val;
}

//
// binary IO
//
ofstream write_file(const string& filePath)
{
	ofstream fout(filePath, ios::out | ios::binary);
	if (!fout.is_open()) {
		std::cerr << "Error: Open file for output failed. [" << filePath << "]" << endl;
		abort();
	}
	return move(fout);
}

ifstream read_file(const string& filePath)
{
	ifstream fin(filePath, ios::in | ios::binary);
	if (!fin.is_open()) {
		std::cerr << "Error: Open file for input failed. [" << filePath << "]" << endl;
		abort();
	}
	return move(fin);
}

void write(ofstream& fout, const string& str)
{
	size_t len = str.size();
	fout.write((char *)&len, sizeof(size_t));
	fout.write(str.c_str(), len);
}


void read(ifstream& fin, string& str)
{
	size_t len;
	fin.read((char *)&len, sizeof(size_t));
	
	str = string(len, '\0');
	for (size_t i = 0; i < len; i++)
		fin.read(&str[i], sizeof(char));
}

void write(ofstream& fout, const unordered_map<string, double>& imMap)
{
	size_t imNum = imMap.size();
	fout.write((char *)&imNum, sizeof(size_t));
	for (const auto& kv : imMap)
		write(fout, kv.first);
}

void read(ifstream& fin, unordered_map<string, double>& imMap)
{
	size_t imNum;
	fin.read((char *)&imNum, sizeof(size_t));

	imMap.clear();
	imMap.reserve(imNum);
	for (size_t i = 0; i < imNum; i++)
	{
		string imName;
		read(fin, imName);
		imMap.insert(make_pair<string, double>(move(imName), 0));
	}
}

void write(ofstream& fout, const PatchInfo& info)
{
	fout.write((char *)&info.imIdx, sizeof(size_t));
	fout.write((char *)&info.iWinX, sizeof(int));
	fout.write((char *)&info.iWinY, sizeof(int));
}

void read(ifstream& fin, PatchInfo& info)
{
	fin.read((char *)&info.imIdx, sizeof(size_t));
	fin.read((char *)&info.iWinX, sizeof(int));
	fin.read((char *)&info.iWinY, sizeof(int));
}

void write(ofstream& fout, const vector<PatchInfo>& v)
{
	size_t infoNum = v.size();
	fout.write((char *)&infoNum, sizeof(size_t));
	for (const auto& info : v)
		write(fout, info);
}

void read(ifstream& fin, vector<PatchInfo>& v)
{
	size_t infoNum;
	fin.read((char *)&infoNum, sizeof(size_t));

	v.clear();
	v.reserve(infoNum);
	for (size_t i = 0; i < infoNum; i++)
	{
		PatchInfo info;
		read(fin, info);
		v.push_back(info);
	}
}

void write(ofstream& fout, const vector<string>& v)
{
	size_t vecSize = v.size();
	fout.write((char *)&vecSize, sizeof(size_t));
	for (const auto& str : v)
		write(fout, str);
}

void read(ifstream& fin, vector<string>& v)
{
	size_t vecSize;
	fin.read((char *)&vecSize, sizeof(size_t));

	v.clear();
	v.reserve(vecSize);
	for (size_t i = 0; i < vecSize; i++)
	{
		string str;
		read(fin, str);
		v.push_back(str);
	}
}
