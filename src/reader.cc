#include "demo/reader.h"

shad::EdgeIndex<size_t, size_t>::ObjectID GraphReader(std::ifstream &GFS) {
  std::string line;
  unsigned long EdgeNumber, VertexNumber;

  std::getline(GFS, line);

  std::istringstream headlineStream(line);

  headlineStream >> VertexNumber >> EdgeNumber;
  EdgeNumber <<= 1;

  auto eiGraph = shad::EdgeIndex<size_t, size_t>::Create(VertexNumber);
  shad::rt::Handle handle;

  for (size_t i = 0L; i < VertexNumber; i++) {
    size_t destination;

    std::getline(GFS, line);
    std::istringstream lineStream(line);
    std::vector<size_t> edges;
    while (!lineStream.eof()) {
      lineStream >> destination;
      destination--;
      if (destination >= i) continue;
      edges.push_back(destination);
    }
    eiGraph->AsyncInsertEdgeList(handle, i, edges.data(), edges.size());
  }

  shad::rt::waitForCompletion(handle);
  return eiGraph->GetGlobalID();
}

size_t Sum(shad::OnePerLocality<std::atomic<size_t>> &obj) {
  // This performs a reduction into a single counter.
  shad::rt::Handle handle;

  std::vector<size_t> reducer(shad::rt::numLocalities());

  for (auto &locality : shad::rt::allLocalities()) {
    shad::rt::asyncExecuteAtWithRet(
        handle, locality,
        [](shad::rt::Handle &, const shad::OnePerLocality<std::atomic<size_t>>::ObjectID & oid, size_t *value) {
          auto counter = shad::OnePerLocality<std::atomic<size_t>>::GetPtr(oid);
          *value = (*counter)->load();
        },
        obj.GetGlobalID(), &reducer[static_cast<uint32_t>(locality)]);
  }
  shad::rt::waitForCompletion(handle);

  size_t counter = 0;
  for (size_t i : reducer) counter += i;

  return counter;
}
