/*
 * retrieve_sketch_example.cpp
 */

#include <iostream>
#include "retrieval.h"

using namespace std;

int main(int argc, char** argv)
{
	/*
	if (argc < 4)
	{
		cout << "usage: retrieval_sketch_example <indexer_data_path> <query_image_path> <target_image_path ...>" << endl;

		getchar();
		return EXIT_FAILURE;
	}

	string indexer_data_path(argv[1]);
	string query_image(argv[2]);
	vector<string> target_image_vector;
	for (int i = 3; i < argc; i++)
		target_image_vector.push_back(argv[i]);
	*/
	string indexer_data_path{ "indexerdata.dat" };
	string query_image{ "u0_c46_1.png" };
	vector<string> target_image_vector{ { "u0_c00_0.png", "u0_c54_1.png", "u1_c37_1.png", "u2_c39_1.png", "u6_c46_0.png" } };

	// string query_image{ "41.png" };
	// vector<string> target_image_vector({ "m1129_8.png", "m1129_12.png", "m1133_4.png", "m1133_5.png", "m1133_6.png", "m1205_12.png" });

	Indexer indexer;
	for (int i = 0; i < target_image_vector.size(); i++)
	{
		cout << "[info]: inserting " << target_image_vector[i] << " ..." << endl;
		insert_image(target_image_vector[i], target_image_vector[i], indexer);
	}

	cout << "[info]: retrieving sketches ..."<< endl;
	vector<pair<string, double>> result = retrieve_image(query_image, indexer);
	for (const auto& val : result)
		cout << "\t" << val.first << ": value [" << val.second << "]" << endl;
	
	// save indexer data 
	indexer.write(indexer_data_path);
	cout << "[info]: indexer saved." << endl;

	// create new indexer from existing data
	indexer.read(indexer_data_path);
	cout << "[info]: indexer loaded." << endl;

	cout << "[info]: retrieving sketches ..." << endl;
	vector<pair<string, double>> result_new = retrieve_image(query_image, indexer);
	for (const auto& val : result_new)
		cout << "\t" << val.first << ": value [" << val.second << "]" << endl;

	/*
	cout << "[info]: retrieving meshes ..." << endl;
	result_new = retrieve_mesh(query_image, indexer, [](string imageName) -> string {
		size_t idx = imageName.find_first_of("_");
		assert(idx != string::npos);
		return imageName.substr(0, idx);
	});
	for (const auto& val : result_new)
		cout << "\t" << val.first << ": value [" << val.second << "]" << endl;
	*/

	getchar();
	return EXIT_SUCCESS;
}