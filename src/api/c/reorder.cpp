/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <af/index.h>
#include <af/blas.h>
#include <err_common.hpp>
#include <handle.hpp>
#include <backend.hpp>
#include <ArrayInfo.hpp>
#include <reorder.hpp>

using af::dim4;
using namespace detail;

template<typename T>
static inline af_array reorder(const af_array in, const af::dim4 &rdims)
{
    return getHandle(*reorder<T>(getArray<T>(in), rdims));
}

af_err af_reorder(af_array *out, const af_array in, const af::dim4 &rdims)
{
    try {
        ArrayInfo info = getInfo(in);
        af_dtype type = info.getType();

        DIM_ASSERT(1, info.elements() > 0);

        // Check that dimensions are not repeated
        int allDims[] = {0, 1, 2, 3};
        for(int i = 0; i < 3; i++) {
            DIM_ASSERT(i + 2, rdims[i] == allDims[rdims[i]]);
            allDims[rdims[i]] = -1;
        }

        // If reorder is a (batched) transpose, then call transpose
        if(info.dims()[3] == 1) {
            if(rdims[0] == 1 && rdims[1] == 0 &&
               rdims[2] == 2 && rdims[3] == 3) {
                return af_transpose(out, in, false);
            }
        }

        af_array output;

        switch(type) {
            case f32: output = reorder<float  >(in, rdims);  break;
            case c32: output = reorder<cfloat >(in, rdims);  break;
            case f64: output = reorder<double >(in, rdims);  break;
            case c64: output = reorder<cdouble>(in, rdims);  break;
            case b8:  output = reorder<char   >(in, rdims);  break;
            case s32: output = reorder<int    >(in, rdims);  break;
            case u32: output = reorder<uint   >(in, rdims);  break;
            case u8:  output = reorder<uchar  >(in, rdims);  break;
            default:  TYPE_ERROR(1, type);
        }
        std::swap(*out,output);
    }
    CATCHALL;

    return AF_SUCCESS;
}

af_err af_reorder(af_array *out, const af_array in,
               const unsigned x, const unsigned y,
               const unsigned z, const unsigned w)
{
    af::dim4 rdims(x, y, z, w);
    return af_reorder(out, in, rdims);
}
