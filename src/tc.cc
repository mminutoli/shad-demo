#include <string>
#include <fstream>
#include <iostream>

#include "demo/reader.h"
#include "shad/util/measure.h"
#include "shad/data_structures/one_per_locality.h"

// This is one for each Locality.
// static std::atomic<size_t> TriangleCounter(0);

size_t TriangleCount(shad::EdgeIndex<size_t, size_t>::ObjectID &eid) {
  using ELObjectID = shad::EdgeIndex<size_t, size_t>::ObjectID;
  using SrcT = shad::EdgeIndex<size_t, size_t>::SrcType;
  using DestT = shad::EdgeIndex<size_t, size_t>::DestType;
  using CounterID = shad::OnePerLocality<std::atomic<size_t>>::ObjectID;

  auto TriangleCounter = shad::OnePerLocality<std::atomic<size_t>>::Create(0ul);

  // Triangle counting loops:
  // 1 - For each vertex in the graph i
  shad::rt::Handle handle;

  auto elPtr = shad::EdgeIndex<size_t, size_t>::GetPtr(eid);
  auto cid = TriangleCounter->GetGlobalID();

  elPtr->AsyncForEachEdge(
      handle,
      [](shad::rt::Handle &handle, const SrcT &i, const DestT &j,
         ELObjectID &eid, CounterID &cid) {
        auto GraphPtr = shad::EdgeIndex<size_t, size_t>::GetPtr(eid);
        // 3 - Visit all the neighbors k of j such that k < j
        GraphPtr->AsyncForEachNeighbor(
            handle, j,
            [](shad::rt::Handle &handle, const SrcT &j, const DestT &k,
               ELObjectID &eid, CounterID &cid, const SrcT &v) {
              auto GraphPtr = shad::EdgeIndex<size_t, size_t>::GetPtr(eid);

              // 4 - Visit all the neighbors w of k and if w == i
              //     increment the counter.
              GraphPtr->AsyncForEachNeighbor(
                  handle, v,
                  [](shad::rt::Handle &handle, const SrcT &i, const DestT &w,
                     const SrcT &k, CounterID &cid) {
                    if (w != k) return;
                    auto counter = shad::OnePerLocality<std::atomic<size_t>>::GetPtr(cid);
                    (*counter)->operator++();
                  },
                  k, cid);
            },
            eid, cid, i);
      },
      eid, cid);
  shad::rt::waitForCompletion(handle);


  return Sum(*TriangleCounter);
}

namespace shad {

int main(int argc, char **argv) {
  if (argc != 2) return -1;

  shad::EdgeIndex<size_t, size_t>::ObjectID OID(-1);
  auto loadingTime = shad::measure<std::chrono::seconds>::duration([&]() {
    // The GraphReader expects an input file in METIS dump format
    std::ifstream inputFile;
    inputFile.open(argv[1], std::ifstream::in);
    OID = GraphReader(inputFile);
  });

  std::cout << "Graph loaded in " << loadingTime.count()
            << " seconds\nLet's find some triangles..." << std::endl;
  auto eiPtr = shad::EdgeIndex<size_t, size_t>::GetPtr(OID);
  std::cout << "NumVertices: " << eiPtr->Size()
            << " Num Edges: " << eiPtr->NumEdges() << std::endl;
  size_t TC = 0;
  auto duration = shad::measure<std::chrono::seconds>::duration(
      [&]() { TC = TriangleCount(OID); });

  std::cout << "I Found : " << TC << " unique triangles in " << duration.count()
            << " seconds" << std::endl;
  shad::EdgeIndex<size_t, size_t>::Destroy(OID);
  return 0;
}

}  // namespace shad
