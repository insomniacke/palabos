/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2013 FlowKit Sarl
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
 * 2D specialization of latticeTemplates functions.
 */

#ifndef LATTICE_TEMPLATES_2D_H
#define LATTICE_TEMPLATES_2D_H

#include "core/globalDefs.h"
#include "latticeBoltzmann/nearestNeighborLattices2D.h"

namespace plb {

// Efficient specialization for D2Q9 lattice
template<typename T>
struct latticeTemplates<T, descriptors::D2Q9Descriptor > {

static void swapAndStreamCell (
        Cell<T,descriptors::D2Q9Descriptor> **grid,
        plint iX, plint iY, plint nX, plint nY, plint iPop, T& fTmp )
{
    fTmp                 = grid[iX][iY][iPop];
    grid[iX][iY][iPop]   = grid[iX][iY][iPop+4];
    grid[iX][iY][iPop+4] = grid[nX][nY][iPop];
    grid[nX][nY][iPop]   = fTmp;
}

static void swapAndStream2D (
        Cell<T,descriptors::D2Q9Descriptor> **grid, plint iX, plint iY )
{
    T fTmp;
    swapAndStreamCell(grid, iX, iY, iX-1, iY+1, 1, fTmp);
    swapAndStreamCell(grid, iX, iY, iX-1, iY,   2, fTmp);
    swapAndStreamCell(grid, iX, iY, iX-1, iY-1, 3, fTmp);
    swapAndStreamCell(grid, iX, iY, iX,   iY-1, 4, fTmp);
}

};

template<typename T>
struct latticeTemplates<T, descriptors::ForcedD2Q9Descriptor> {

static void swapAndStreamCell (
        Cell<T,descriptors::ForcedD2Q9Descriptor> **grid,
        plint iX, plint iY, plint nX, plint nY, plint iPop, T& fTmp )
{
    fTmp                 = grid[iX][iY][iPop];
    grid[iX][iY][iPop]   = grid[iX][iY][iPop+4];
    grid[iX][iY][iPop+4] = grid[nX][nY][iPop];
    grid[nX][nY][iPop]   = fTmp;
}

static void swapAndStream2D (
        Cell<T,descriptors::ForcedD2Q9Descriptor> **grid, plint iX, plint iY )
{
    T fTmp;
    swapAndStreamCell(grid, iX, iY, iX-1, iY+1, 1, fTmp);
    swapAndStreamCell(grid, iX, iY, iX-1, iY,   2, fTmp);
    swapAndStreamCell(grid, iX, iY, iX-1, iY-1, 3, fTmp);
    swapAndStreamCell(grid, iX, iY, iX,   iY-1, 4, fTmp);
}

};

}  // namespace plb

#endif
