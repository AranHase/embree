// ======================================================================== //
// Copyright 2009-2013 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "geometry/bezier1.h"
#include "builders/primrefalloc.h"

namespace embree
{
  /*! stores bounding information for a set of primitives */
  class PrimInfo
  {
    /*! Compute the number of blocks occupied in one dimension. */
    __forceinline static size_t  blocks(size_t a) { return a; }
    //__forceinline static size_t  blocks(size_t a) { return (a+3) >> 2; }

  public:
    __forceinline PrimInfo () 
      : num(0), geomBounds(empty), centBounds(empty) {}
    
    __forceinline PrimInfo (size_t num, const BBox3fa& geomBounds, const BBox3fa& centBounds) 
      : num(num), geomBounds(geomBounds), centBounds(centBounds) {}

    __forceinline void add(const BBox3fa& geomBounds_, const BBox3fa& centBounds_, size_t num_ = 1) {
      geomBounds.extend(geomBounds_);
      centBounds.extend(centBounds_);
      num += num_;
    }

    /*! returns the number of primitives */
    __forceinline size_t size() const { 
      return num; 
    }

    __forceinline float leafSAH(float primCost) const { 
      return halfArea(geomBounds)*primCost*blocks(num); 
    }

  public:
    size_t num;          //!< number of primitives
    BBox3fa geomBounds;   //!< geometry bounds of primitives
    BBox3fa centBounds;   //!< centroid bounds of primitives
  };

  /*! Performs fallback splits */
  struct FallBackSplit
  {
    typedef PrimRefBlockT<Bezier1> BezierRefBlock;
    typedef atomic_set<BezierRefBlock> BezierRefList;
    
    __forceinline FallBackSplit (const NAABBox3fa& bounds0, size_t num0, const NAABBox3fa& bounds1, size_t num1)
      : bounds0(bounds0), num0(num0), bounds1(bounds1), num1(num1) {}
    
    /*! finds some partitioning */
    static FallBackSplit find(size_t threadIndex, PrimRefBlockAlloc<Bezier1>& alloc, 
			      BezierRefList& prims, BezierRefList& lprims_o, PrimInfo& linfo_o, BezierRefList& rprims_o, PrimInfo& rinfo_o);
    
  public:
    size_t num0, num1;
    NAABBox3fa bounds0, bounds1;
  };
}