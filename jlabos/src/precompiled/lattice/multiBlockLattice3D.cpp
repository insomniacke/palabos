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
 * A 3D multiblock lattice -- template instantiation.
 */

#ifdef COMPILE_3D

#include "latticeBoltzmann/nearestNeighborLattices3D.h"
#include "latticeBoltzmann/nearestNeighborLattices3D.hh"
#include "multiBlock/multiBlockLattice3D.h"
#include "multiBlock/multiBlockLattice3D.hh"
#include "coProcessors/coProcessor3D.h"
#include "coProcessors/coProcessor3D.hh"

namespace plb {

    template class MultiBlockLattice3D<FLOAT_T, descriptors::DESCRIPTOR_3D>;

    template double getStoredAverageDensity<FLOAT_T, descriptors::DESCRIPTOR_3D>
        (MultiBlockLattice3D<FLOAT_T,descriptors::DESCRIPTOR_3D> const& blockLattice);
    template double getStoredAverageEnergy<FLOAT_T, descriptors::DESCRIPTOR_3D>
        (MultiBlockLattice3D<FLOAT_T,descriptors::DESCRIPTOR_3D> const& blockLattice);
    template double getStoredMaxVelocity<FLOAT_T, descriptors::DESCRIPTOR_3D>
        (MultiBlockLattice3D<FLOAT_T,descriptors::DESCRIPTOR_3D> const& blockLattice);

}

#endif  // COMPILE_3D
