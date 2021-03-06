/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2012 FlowKit Sarl
 * Route d'Oron 2
 * 1010 Lausanne, Switzerland
 * E-mail contact: contact@flowkit.com
 *
 * The most recent release of Palabos can be downloaded at 
 * <http://www.palabos.org/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
 * Helper functions for domain initialization -- header file.
 */

#ifndef SWIG_MULTI_BLOCK_LATTICE_INFO_3D_H
#define SWIG_MULTI_BLOCK_LATTICE_INFO_3D_H

#include "multiBlock/multiBlockLattice3D.h"
#include "multiBlock/multiBlockInfo3D.h"

namespace plb {

template<typename T, template<typename T> class Descriptor>
class MultiBlockLatticeInfo {
public:
    MultiBlockLatticeInfo(MultiBlockLattice3D<T,Descriptor> const& multiBlock);
    plint getNx() const;
    plint getNy() const;
    plint getNz() const;
    plint getNumBlocks() const;
    Box3D getSmallestBlock() const;
    Box3D getLargestBlock() const;
    plint getNumAllocatedCells() const;
private:
    plint nx, ny, nz, numBlocks, numAllocatedCells;
    Box3D smallest, largest;
};

}  // namespace plb

#endif  // SWIG_MULTI_BLOCK_LATTICE_INFO_3D_H
