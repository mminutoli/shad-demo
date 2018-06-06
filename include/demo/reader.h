#pragma once

#include <atomic>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>

#include "shad/runtime/runtime.h"
#include "shad/extensions/graph_library/edge_index.h"
#include "shad/data_structures/one_per_locality.h"

// The GraphReader expects an input file in METIS dump format
shad::EdgeIndex<size_t, size_t>::ObjectID GraphReader(std::ifstream &GFS);
size_t Sum(shad::OnePerLocality<std::atomic<size_t>> &obj);
