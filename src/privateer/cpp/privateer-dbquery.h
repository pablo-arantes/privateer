// Library for the YSBL program Privateer (PRogramatic Identification of Various Anomalies Toothsome Entities Experience in Refinement)
// Licence: LGPL - Please check Licence.txt for details.
//
// 2013-
// York Structural Biology Laboratory
// The University of York



#ifndef DBQUERY_H_INCLUDED
#define DBQUERY_H_INCLUDED

#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <clipper/clipper.h>
#include <clipper/clipper-mmdb.h>
#include <clipper/clipper-minimol.h>
#include "clipper-glyco.h"
#include "clipper-glyco_data.h"
#include "privateer-composition.h"
#include "privateer-lib.h"
#include "privateer-parallelism.h"
#include <clipper/minimol/minimol_utils.h>


#ifdef _WIN32
    #undef snprintf
#endif
#include <nlohmann/json.hpp>



void output_dbquery(nlohmann::json& jsonObject, clipper::String glycanWURCS, clipper::MGlycan& currentGlycan, bool closest_match_disable, std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>>& finalGlycanPermutationContainer, bool glucose_only, bool debug_output, int sleepTimer, privateer::thread_pool& pool, bool useParallelism);
void push_data_to_final_permutation_container(nlohmann::json &jsonObject, clipper::MGlycan &currentGlycan, std::vector<std::pair<clipper::MGlycan, std::vector<int>>>& alternativeGlycans, std::vector<std::pair<std::pair<clipper::MGlycan, std::vector<int>>,float>>& finalGlycanPermutationContainer);
void print_output_from_database(nlohmann::json& jsonObject, int valueLocation, clipper::MGlycan &currentGlycan);



#endif