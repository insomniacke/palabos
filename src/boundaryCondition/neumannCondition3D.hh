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
 * Neumann and outflow boundary conditions -- generic implementation.
 */
#ifndef NEUMANN_CONDITION_3D_HH
#define NEUMANN_CONDITION_3D_HH

#include "boundaryCondition/neumannCondition3D.h"
#include "latticeBoltzmann/indexTemplates.h"

namespace plb {

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
void CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    std::vector<plint> const& unknownIndices = indexTemplates::subIndex<Descriptor<T>, direction, -orientation>();
    enum {
        normalX = direction==0 ? orientation : 0,
        normalY = direction==1 ? orientation : 0,
        normalZ = direction==2 ? orientation : 0
    };
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                for (pluint fIndex=0; fIndex<unknownIndices.size(); ++fIndex) {
                    plint iPop = unknownIndices[fIndex];
                    lattice.get(iX,iY,iZ)[iPop] = lattice.get(iX-normalX, iY-normalY, iZ-normalZ)[iPop];
                }
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>*
    CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>::clone() const
{
    return new CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>(*this);
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
void CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::staticVariables;
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
BlockDomain::DomainT CopyUnknownPopulationsFunctional3D<T,Descriptor,direction,orientation>::appliesTo() const {
    return BlockDomain::bulk;
}


template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyAllPopulationsFunctional3D<T,Descriptor,normalX,normalY,normalZ>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                for (plint iPop=0; iPop<Descriptor<T>::q; ++iPop) {
                    lattice.get(iX,iY,iZ)[iPop] = lattice.get(iX-normalX, iY-normalY, iZ-normalZ)[iPop];
                }
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
CopyAllPopulationsFunctional3D<T,Descriptor,normalX,normalY,normalZ>*
    CopyAllPopulationsFunctional3D<T,Descriptor,normalX,normalY,normalZ>::clone() const
{
    return new CopyAllPopulationsFunctional3D<T,Descriptor,normalX,normalY,normalZ>(*this);
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyAllPopulationsFunctional3D<T,Descriptor, normalX,normalY,normalZ>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::staticVariables;
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
BlockDomain::DomainT CopyAllPopulationsFunctional3D<T,Descriptor,normalX,normalY,normalZ>::appliesTo() const {
    return BlockDomain::bulk;
}


template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                Array<T,Descriptor<T>::d> u;
                lattice.get(iX-normalX, iY-normalY, iZ-normalZ).computeVelocity(u);
                lattice.get(iX, iY, iZ).defineVelocity(u);
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
CopyVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>*
    CopyVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::clone() const
{
    return new CopyVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>(*this);
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyVelocityFunctional3D<T,Descriptor, normalX,normalY,normalZ>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::dynamicVariables;
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
BlockDomain::DomainT CopyVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::appliesTo() const {
    return BlockDomain::bulk;
}


template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyTangentialVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                Array<T,Descriptor<T>::d> u;
                lattice.get(iX-normalX, iY-normalY, iZ-normalZ).computeVelocity(u);
                if (normalX!=0) {
                    u[0] = T();
                }
                if (normalY!=0) {
                    u[1] = T();
                }
                if (normalZ!=0) {
                    u[2] = T();
                }
                lattice.get(iX, iY, iZ).defineVelocity(u);
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
CopyTangentialVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>*
    CopyTangentialVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::clone() const
{
    return new CopyTangentialVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>(*this);
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyTangentialVelocityFunctional3D<T,Descriptor, normalX,normalY,normalZ>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::dynamicVariables;
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
BlockDomain::DomainT CopyTangentialVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::appliesTo() const {
    return BlockDomain::bulk;
}



template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyNormalVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                Array<T,Descriptor<T>::d> u;
                lattice.get(iX-normalX, iY-normalY, iZ-normalZ).computeVelocity(u);
                if (normalX==0) {
                    u[0] = T();
                }
                if (normalY==0) {
                    u[1] = T();
                }
                if (normalZ==0) {
                    u[2] = T();
                }
                lattice.get(iX, iY, iZ).defineVelocity(u);
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
CopyNormalVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>*
    CopyNormalVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::clone() const
{
    return new CopyNormalVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>(*this);
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyNormalVelocityFunctional3D<T,Descriptor, normalX,normalY,normalZ>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::dynamicVariables;
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
BlockDomain::DomainT CopyNormalVelocityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::appliesTo() const {
    return BlockDomain::bulk;
}



template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyDensityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                lattice.get(iX, iY, iZ).defineDensity (
                        lattice.get(iX-normalX, iY-normalY, iZ-normalZ).computeDensity() );
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
CopyDensityFunctional3D<T,Descriptor,normalX,normalY,normalZ>*
    CopyDensityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::clone() const
{
    return new CopyDensityFunctional3D<T,Descriptor,normalX,normalY,normalZ>(*this);
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
void CopyDensityFunctional3D<T,Descriptor, normalX,normalY,normalZ>::
         getTypeOfModification(std::vector<modif::ModifT>& modified) const
{
    modified[0] = modif::dynamicVariables;
}

template<typename T, template<typename U> class Descriptor, int normalX, int normalY, int normalZ> 
BlockDomain::DomainT CopyDensityFunctional3D<T,Descriptor,normalX,normalY,normalZ>::appliesTo() const {
    return BlockDomain::bulk;
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
void FluidPressureOutlet3D<T,Descriptor,direction,orientation>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    std::vector<plint> const& unknownIndices = indexTemplates::subIndex<Descriptor<T>, direction, -orientation>();
    enum {
        normalX = direction==0 ? orientation : 0,
        normalY = direction==1 ? orientation : 0,
        normalZ = direction==2 ? orientation : 0
    };

    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                Cell<T,Descriptor>& cell = lattice.get(iX,iY,iZ);
                Cell<T,Descriptor>& neighbor = lattice.get(iX-normalX, iY-normalY, iZ-normalZ);
                for (pluint fIndex=0; fIndex<unknownIndices.size(); ++fIndex) {
                    plint iPop = unknownIndices[fIndex];
                    cell[iPop] = neighbor[iPop];
                }
                T rhoBar;
                Array<T,3> j;
                cell.getDynamics().computeRhoBarJ(cell, rhoBar, j);
                Array<T,Descriptor<T>::q> oldFeq, newFeq;
                T jSqr = normSqr(j);
                cell.getDynamics().computeEquilibria(oldFeq, rhoBar, j, jSqr);
                cell.getDynamics().computeEquilibria(newFeq, T(), j, jSqr);
                for (plint iPop=0; iPop<Descriptor<T>::q; ++iPop) {
                    cell[iPop] += newFeq[iPop]-oldFeq[iPop];
                }
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation>
void CopyUnknown3D<T,Descriptor,direction,orientation>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    std::vector<plint> const& unknownIndices = indexTemplates::subIndex<Descriptor<T>, direction, -orientation>();
    enum {
        normalX = direction==0 ? orientation : 0,
        normalY = direction==1 ? orientation : 0,
        normalZ = direction==2 ? orientation : 0
    };

    for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {

                Cell<T,Descriptor>& cell = lattice.get(iX,iY,iZ);
                Cell<T,Descriptor>& neighbor = lattice.get(iX-normalX, iY-normalY, iZ-normalZ);
                for (pluint fIndex=0; fIndex<unknownIndices.size(); ++fIndex) {
                    plint iPop = unknownIndices[fIndex];
                    cell[iPop] = neighbor[iPop];
                }
            }
        }
    }
}


template<typename T, template<typename U> class Descriptor>
VirtualOutlet<T,Descriptor>::VirtualOutlet(T outsideDensity_, Box3D globalDomain_, int type_)
        : outsideDensity(outsideDensity_),
          globalDomain(globalDomain_),
          type(type_)
{
    PLB_ASSERT(type == 0 || type == 1);
}

template<typename T, template<typename U> class Descriptor>
void VirtualOutlet<T,Descriptor>::processGenericBlocks(Box3D domain, std::vector<AtomicBlock3D*> blocks)
{
    PLB_ASSERT(blocks.size() == 3);
    BlockLattice3D<T,Descriptor> *lattice = dynamic_cast<BlockLattice3D<T,Descriptor>*>(blocks[0]);
    PLB_ASSERT(lattice);
    ScalarField3D<T> *rhoBar = dynamic_cast<ScalarField3D<T>*>(blocks[1]);
    PLB_ASSERT(rhoBar);
    TensorField3D<T,3> *j = dynamic_cast<TensorField3D<T,3>*>(blocks[2]);
    PLB_ASSERT(j);

    Dot3D absOfs = lattice->getLocation();

    Dot3D ofsRB = computeRelativeDisplacement(*lattice, *rhoBar);
    Dot3D ofsJ = computeRelativeDisplacement(*lattice, *j);

    if (type == 0) {
        for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
            for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
                for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                    Cell<T,Descriptor>& cell = lattice->get(iX, iY, iZ);
                    for (plint iPop = 1; iPop < Descriptor<T>::q; ++iPop) {
                        plint prevX = iX - Descriptor<T>::c[iPop][0];
                        plint prevY = iY - Descriptor<T>::c[iPop][1];
                        plint prevZ = iZ - Descriptor<T>::c[iPop][2];
                        
                        if (!contained(prevX + absOfs.x, prevY + absOfs.y, prevZ + absOfs.z, globalDomain)) {
                            plint opp = indexTemplates::opposite<Descriptor<T> >(iPop);
                            T savedPop = lattice->get(prevX, prevY, prevZ)[opp];

                            // Velocity is simply taken from the previous time step.
                            Array<T,3> J = j->get(iX + ofsJ.x, iY + ofsJ.y, iZ +ofsJ.z);
                            T Jsqr = dot<T,3>(J, J);
                            // Density is prescribed as a boundary condition.
                            T outsideRhoBar = Descriptor<T>::rhoBar(outsideDensity);
                            T feq_i = cell.computeEquilibrium(iPop, outsideRhoBar, J, Jsqr);
                            T feq_opp_i = cell.computeEquilibrium(opp, outsideRhoBar, J, Jsqr);
                            cell[iPop] = feq_i + feq_opp_i - savedPop;
                        }
                    }
                }
            }
        }
    } else {
        for (plint iX=domain.x0; iX<=domain.x1; ++iX) {
            for (plint iY=domain.y0; iY<=domain.y1; ++iY) {
                for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) {
                    Cell<T,Descriptor>& cell = lattice->get(iX, iY, iZ);
                    for (plint iPop = 1; iPop < Descriptor<T>::q; ++iPop) {
                        plint prevX = iX - Descriptor<T>::c[iPop][0];
                        plint prevY = iY - Descriptor<T>::c[iPop][1];
                        plint prevZ = iZ - Descriptor<T>::c[iPop][2];
                        
                        if (!contained(prevX + absOfs.x, prevY + absOfs.y, prevZ + absOfs.z, globalDomain)) {
                            plint opp = indexTemplates::opposite<Descriptor<T> >(iPop);
                            T savedPop = lattice->get(prevX, prevY, prevZ)[opp];

                            // Velocity is simply taken from the previous time step.
                            Array<T,3> J = j->get(iX + ofsJ.x, iY + ofsJ.y, iZ +ofsJ.z);
                            T Jsqr = dot<T,3>(J, J);

                            // For the density there are several choices:
                            // First choice: density from the previous time step.
                            //T RhoBar = rhoBar->get(iX + ofsRB.x, iY + ofsRB.y, iZ +ofsRB.z);

                            // Second choice: Laplacian smoothed value from the previous time step.
                            T RhoBar = 0.0;
                            int n = 0;
                            for (plint dx = -1; dx <= 1; dx++) {
                                plint i = iX + dx;
                                for (plint dy = -1; dy <= 1; dy++) {
                                    plint j = iY + dy;
                                    for (plint dz = -1; dz <= 1; dz++) {
                                        plint k = iZ + dz;
                                        if (!(dx == 0 && dy == 0 && dz == 0)) {
                                            if (contained(i + absOfs.x, j + absOfs.y, k + absOfs.z, globalDomain)) {
                                                RhoBar += rhoBar->get(i + ofsRB.x, j + ofsRB.y, k +ofsRB.z);
                                                n++;
                                            }
                                        }
                                    }
                                }
                            }
                            if (n != 0) {
                                RhoBar /= n;
                            } else {
                                RhoBar = Descriptor<T>::rhoBar(outsideDensity);
                            }

                            // Third choice: Mean value of the Laplacian smoothed value and the local
                            //               value from the previous time step. This is believed
                            //               to kill checkerboard modes.
                            RhoBar = 0.5 * (RhoBar + rhoBar->get(iX + ofsRB.x, iY + ofsRB.y, iZ +ofsRB.z));

                            T feq_i = cell.computeEquilibrium(iPop, RhoBar, J, Jsqr);
                            T feq_opp_i = cell.computeEquilibrium(opp, RhoBar, J, Jsqr);
                            cell[iPop] = feq_i + feq_opp_i - savedPop;
                        }
                    }
                }
            }
        }
    }
}

}  // namespace plb

#endif  // NEUMANN_CONDITION_3D_HH
