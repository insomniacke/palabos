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

/* Main author: Dimitrios Kontaxakis */

#ifndef TRIANGLE_SET_HH
#define TRIANGLE_SET_HH

#include "triangleSet.h"
#include "core/util.h"
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cmath>

namespace plb {

template<typename T>
TriangleSet<T>::TriangleSet(Precision precision_)
    : minEdgeLength(std::numeric_limits<T>::max()),
      maxEdgeLength(std::numeric_limits<T>::min())
{
    PLB_ASSERT(precision_ == FLT || precision_ == DBL || precision_ == LDBL);
    precision = precision_;

    boundingCuboid.lowerLeftCorner  = Array<T,3>((T) 0.0, (T) 0.0, (T) 0.0);
    boundingCuboid.upperRightCorner = Array<T,3>((T) 0.0, (T) 0.0, (T) 0.0);
}

template<typename T>
TriangleSet<T>::TriangleSet(std::vector<Triangle> const& triangles_, Precision precision_)
    : triangles(triangles_),
      minEdgeLength(std::numeric_limits<T>::max()),
      maxEdgeLength(std::numeric_limits<T>::min())
{
    PLB_ASSERT(precision_ == FLT || precision_ == DBL || precision_ == LDBL);
    precision = precision_;

    computeMinMaxEdges();
    computeBoundingCuboid();
}

template<typename T>
TriangleSet<T>::TriangleSet(std::string fname, Precision precision_, SurfaceGeometryFileFormat fformat)
    : minEdgeLength(std::numeric_limits<T>::max()),
      maxEdgeLength(std::numeric_limits<T>::min())
{
    PLB_ASSERT(precision_ == FLT || precision_ == DBL || precision_ == LDBL);
    PLB_ASSERT(fformat == STL);
    precision = precision_;

    switch (fformat) {
    case STL: default:
        readSTL(fname);
        break;
    }

    // TODO: Check if the next call is actually needed, since the min and max edges are
    //       computed in the readAsciiSTL and readBinarySTL functions as well.
    computeMinMaxEdges();

    computeBoundingCuboid();
}

template<typename T>
std::vector<typename TriangleSet<T>::Triangle> const&
    TriangleSet<T>::getTriangles() const
{
    return triangles;
}

template<typename T>
void TriangleSet<T>::setPrecision(Precision precision_)
{
    PLB_ASSERT(precision_ == FLT || precision_ == DBL || precision_ == LDBL);
    precision = precision_;
}

template<typename T>
void TriangleSet<T>::readSTL(std::string fname)
{
    char buf[256];
    FILE *fp = fopen(fname.c_str(), "r");
    PLB_ASSERT(fp != NULL); // The input file cannot be read.

    char *sp = fgets(buf, 256, fp);
    PLB_ASSERT(sp != NULL); // The input file cannot be read.
    rewind(fp);

    if (strstr(buf, "solid") != NULL) {
        readAsciiSTL(fp);
    }
    else {
        readBinarySTL(fp);
    }

    fclose(fp);
}

template<typename T>
void TriangleSet<T>::readAsciiSTL(FILE* fp) {
    char buf[256];
    char *cp, *sp;

    sp = fgets(buf, 256, fp);
    PLB_ASSERT(sp != NULL); // The input file is badly structured.

    char fmt[32];
    bool failed = false;
    if (sizeof(T) == sizeof(float))
        strcpy(fmt, "%f%f%f");
    else if (sizeof(T) == sizeof(double))
        strcpy(fmt, "%lf%lf%lf");
    else if (sizeof(T) == sizeof(long double))
        strcpy(fmt, "%Lf%Lf%Lf");
    else
        failed = true;

    PLB_ASSERT(!failed); // The input file cannot be read.

    cp = strstr(buf, "solid");
    PLB_ASSERT(cp != NULL); // The input file is badly structured.

    while (cp != NULL && !failed) {
        if (fgets(buf, 256, fp) == NULL) {
            failed = true;
        }

        do {
            if ((cp = strstr(buf, "facet normal")) == NULL) {
                failed = true;
            }
            cp += 12;
            Array<T,3> n;
            if (sscanf(cp, fmt, &n[0], &n[1], &n[2]) != 3) {
                failed = true;
            }

            if (fgets(buf, 256, fp) == NULL || strstr(buf, "outer loop") == NULL)
            {
                failed = true;
            }

            Triangle triangle;
            T nextMin, nextMax;
            for (int i = 0; i < 3; i++) {
                if ( fgets(buf, 256, fp) == NULL ||
                     (cp = strstr(buf, "vertex")) == NULL )
                {
                    failed = true;
                }
                cp += 6;
                triangle[i][0] = T();
                triangle[i][1] = T();
                triangle[i][2] = T();
                if (sscanf( cp, fmt,
                            &triangle[i][0],
                            &triangle[i][1],
                            &triangle[i][2] ) != 3)
                {
                    failed = true;
                }
            }

            if (fgets(buf, 256, fp) == NULL || strstr(buf, "endloop") == NULL) {
                failed = true;
            }

            if (fgets(buf, 256, fp) == NULL || strstr(buf, "endfacet") == NULL) {
                failed = true;
            }

            if (checkNoAbort(triangle, n)) {
                triangles.push_back(triangle);

                computeMinMaxEdge(triangles.size()-1, nextMin, nextMax);
                minEdgeLength = std::min(minEdgeLength, nextMin);
                maxEdgeLength = std::max(maxEdgeLength, nextMax);
            }

            if (fgets(buf, 256, fp) == NULL) {
                failed = true;
            }
            cp = strstr(buf, "endsolid");
        }
        while (cp == NULL && !failed);

        if (fgets(buf, 256, fp) == NULL)
            break;

        cp = strstr(buf, "solid");
    }
    PLB_ASSERT(!failed); // The input file is badly structured.
}

template<typename T>
void TriangleSet<T>::readBinarySTL(FILE* fp)
{
    char buf[256];
    unsigned int nt;
    float array[3];
    unsigned short abc;
    bool failed = false;

    int count = 0;
    while (fread(buf, sizeof(char), 80, fp) == 80 &&
           fread(&nt, sizeof(unsigned int), 1, fp) == 1 && !failed) {
        count++;
        T nextMin, nextMax;
        for (unsigned it = 0; it < nt && !failed; it++) {
            if (fread(array, sizeof(float), 3, fp) != 3) {
                failed = true;
            }
            Array<T,3> n;
            n[0] = array[0];
            n[1] = array[1];
            n[2] = array[2];

            Triangle triangle;
            for (int i = 0; i < 3 && !failed; i++) {
                if (fread(array, sizeof(float), 3, fp) != 3) {
                    failed = true;
                }
                triangle[i][0] = T();
                triangle[i][1] = T();
                triangle[i][2] = T();
                triangle[i][0] = array[0];
                triangle[i][1] = array[1];
                triangle[i][2] = array[2];
            }

            if (fread(&abc, sizeof(unsigned short), 1, fp) != 1) {
                failed = true;
            }

            if (checkNoAbort(triangle, n)) {
                triangles.push_back(triangle);

                computeMinMaxEdge(triangles.size()-1, nextMin, nextMax);
                minEdgeLength = std::min(minEdgeLength, nextMin);
                maxEdgeLength = std::max(maxEdgeLength, nextMax);
            }
        }
    }

    if (count == 0) 
        failed = true;

    PLB_ASSERT(!failed); // The input file is badly structured.
}

/// Make some optional checks and fix triangle orientation.
template<typename T>
void TriangleSet<T>::check(Triangle& triangle, Array<T,3> const& n)
{
    T eps = getEpsilon<T>(precision);

    Array<T,3> v01 = triangle[1] - triangle[0];
    Array<T,3> v02 = triangle[2] - triangle[0];
    Array<T,3> v12 = triangle[2] - triangle[1];

    T norm01 = sqrt(VectorTemplateImpl<T,3>::normSqr(v01));
    T norm02 = sqrt(VectorTemplateImpl<T,3>::normSqr(v02));
    T norm12 = sqrt(VectorTemplateImpl<T,3>::normSqr(v12));

    if (util::fpequal(norm01, (T) 0.0, eps) || util::fpequal(norm02, (T) 0.0, eps) ||
        util::fpequal(norm12, (T) 0.0, eps))
    {
        PLB_ASSERT(false); // One of the triangles is degenerate.
    }

    Array<T,3> cn;
    crossProduct(v01, v02, cn);

    T norm_cn = sqrt(VectorTemplateImpl<T,3>::normSqr(cn));

    if (util::fpequal(norm_cn, (T) 0.0, eps))
    {
        PLB_ASSERT(false); // One of the triangles has zero area.
    }

    T dot = VectorTemplateImpl<T,3>::scalarProduct(cn,n);

    if (dot < (T) 0.0) {
        std::swap(triangle[1],triangle[2]);
    }
}

/// Make some optional checks and fix triangle orientation.
template<typename T>
bool TriangleSet<T>::checkNoAbort(Triangle& triangle, Array<T,3> const& n)
{
    T eps = getEpsilon<T>(precision);

    Array<T,3> v01 = triangle[1] - triangle[0];
    Array<T,3> v02 = triangle[2] - triangle[0];
    Array<T,3> v12 = triangle[2] - triangle[1];

    T norm01 = sqrt(VectorTemplateImpl<T,3>::normSqr(v01));
    T norm02 = sqrt(VectorTemplateImpl<T,3>::normSqr(v02));
    T norm12 = sqrt(VectorTemplateImpl<T,3>::normSqr(v12));

    if (util::fpequal(norm01, (T) 0.0, eps) || util::fpequal(norm02, (T) 0.0, eps) ||
        util::fpequal(norm12, (T) 0.0, eps))
    {
        return false; // One of the triangles is degenerate.
    }

    Array<T,3> cn;
    crossProduct(v01, v02, cn);

    T norm_cn = sqrt(VectorTemplateImpl<T,3>::normSqr(cn));

    if (util::fpequal(norm_cn, (T) 0.0, eps))
    {
        return false; // One of the triangles has zero area.
    }

    T dot = VectorTemplateImpl<T,3>::scalarProduct(cn,n);

    if (dot < (T) 0.0) {
        std::swap(triangle[1],triangle[2]);
    }
    return true;
}

template<typename T>
void TriangleSet<T>::translate(Array<T,3> const& vector)
{
    T eps = std::numeric_limits<T>::epsilon();
    if (util::fpequal((T) sqrt(VectorTemplateImpl<T,3>::normSqr(vector)), (T) 0.0, eps))
        return;

    plint size = triangles.size();
    if (size == 0)
        return;

    for (plint i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            triangles[i][j] += vector;
        }
    }

    boundingCuboid.lowerLeftCorner  += vector;
    boundingCuboid.upperRightCorner += vector;
}

template<typename T>
void TriangleSet<T>::scale(T alpha)
{
    T eps = std::numeric_limits<T>::epsilon();
    if (util::fpequal(alpha, (T) 1.0, eps))
        return;

    plint size = triangles.size();
    if (size == 0)
        return;

    for (plint i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            triangles[i][j] *= alpha;
        }
    }
    minEdgeLength *= alpha;
    maxEdgeLength *= alpha;

    boundingCuboid.lowerLeftCorner  *= alpha;
    boundingCuboid.upperRightCorner *= alpha;
}

template<typename T>
void TriangleSet<T>::rotateAtOrigin(Array<T,3> const& normedAxis, T theta) {
    plint size = (plint)triangles.size();
    for (plint iTriangle = 0; iTriangle < size; iTriangle++) {
        for (int iVertex = 0; iVertex < 3; iVertex++) {
            triangles[iTriangle][iVertex] =
                plb::rotateAtOrigin (
                    triangles[iTriangle][iVertex], normedAxis, theta );
        }
    }

    computeBoundingCuboid();
}

template<typename T>
void TriangleSet<T>::rotate(T phi, T theta, T psi)
{   
    T eps = std::numeric_limits<T>::epsilon();
    T pi = acos(-1.0);

    PLB_ASSERT((theta > (T) 0.0 || util::fpequal(theta, (T) 0.0, eps)) &&
               (theta < pi  || util::fpequal(theta, pi, eps)));

    plint size = triangles.size();
    if (size == 0)
        return;

    T a[3][3];
    a[0][0] =  (T) 1.0;
    a[0][1] =  (T) 0.0;
    a[0][2] =  (T) 0.0;
    a[1][0] =  (T) 0.0;
    a[1][1] =  cos(theta);
    a[1][2] = -sin(theta);
    a[2][0] =  (T) 0.0;
    a[2][1] =  sin(theta);
    a[2][2] =  cos(theta);

    T b[3][3];
    b[0][0] =  cos(phi);
    b[0][1] = -sin(phi);
    b[0][2] =  (T) 0.0;
    b[1][0] =  sin(phi);
    b[1][1] =  cos(phi);
    b[1][2] =  (T) 0.0;
    b[2][0] =  (T) 0.0;
    b[2][1] =  (T) 0.0;
    b[2][2] =  (T) 1.0;

    T c[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            c[i][j] = (T) 0.0;
            for (int k = 0; k < 3; k++) {
                c[i][j] += a[i][k]*b[k][j];
            }
        }
    }

    b[0][0] =  cos(psi);
    b[0][1] = -sin(psi);
    b[0][2] =  (T) 0.0;
    b[1][0] =  sin(psi);
    b[1][1] =  cos(psi);
    b[1][2] =  (T) 0.0;
    b[2][0] =  (T) 0.0;
    b[2][1] =  (T) 0.0;
    b[2][2] =  (T) 1.0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            a[i][j] = (T) 0.0;
            for (int k = 0; k < 3; k++) {
                a[i][j] += b[i][k]*c[k][j];
            }
        }
    }

    for (plint iTriangle = 0; iTriangle < size; iTriangle++) {
        for (int iVertex = 0; iVertex < 3; iVertex++) {
            Array<T,3> x = triangles[iTriangle][iVertex];
            for (int i = 0; i < 3; i++) {
                triangles[iTriangle][iVertex][i] = (T) 0.0;
                for (int j = 0; j < 3; j++) {
                    triangles[iTriangle][iVertex][i] += a[i][j]*x[j];
                }
            }
        }
    }

    computeBoundingCuboid();
}

template<typename T>
void TriangleSet<T>::merge(std::vector<TriangleSet<T>*> meshes)
{
    PLB_ASSERT(meshes.size() != 0);

    triangles.assign(meshes[0]->getTriangles().begin(), meshes[0]->getTriangles().end());
    minEdgeLength = meshes[0]->getMinEdgeLength();
    maxEdgeLength = meshes[0]->getMaxEdgeLength();
    boundingCuboid = meshes[0]->getBoundingCuboid();
    for (pluint i = 1; i < meshes.size(); i++) {
        triangles.insert(triangles.end(), meshes[i]->getTriangles().begin(), meshes[i]->getTriangles().end());
        minEdgeLength = std::min(minEdgeLength, meshes[i]->getMinEdgeLength());
        maxEdgeLength = std::max(maxEdgeLength, meshes[i]->getMaxEdgeLength());

        Cuboid<T> bcuboid = meshes[i]->getBoundingCuboid();
        for (plint j = 0; j < 3; j++) {
            boundingCuboid.lowerLeftCorner[j]  = std::min(boundingCuboid.lowerLeftCorner[j],
                                                          bcuboid.lowerLeftCorner[j]);
            boundingCuboid.upperRightCorner[j] = std::max(boundingCuboid.upperRightCorner[j],
                                                          bcuboid.upperRightCorner[j]);
        }
    }
}

template<typename T>
void TriangleSet<T>::append(TriangleSet<T> const& mesh)
{
    triangles.insert(triangles.end(), mesh.getTriangles().begin(), mesh.getTriangles().end());
    minEdgeLength = std::min(minEdgeLength, mesh.getMinEdgeLength());
    maxEdgeLength = std::max(maxEdgeLength, mesh.getMaxEdgeLength());

    Cuboid<T> bcuboid = mesh.getBoundingCuboid();
    for (plint j = 0; j < 3; j++) {
        boundingCuboid.lowerLeftCorner[j]  = std::min(boundingCuboid.lowerLeftCorner[j],
                                                      bcuboid.lowerLeftCorner[j]);
        boundingCuboid.upperRightCorner[j] = std::max(boundingCuboid.upperRightCorner[j],
                                                      bcuboid.upperRightCorner[j]);
    }
}

template<typename T>
void TriangleSet<T>::refine()
{
    std::vector<Triangle> newTriangles;

    for (pluint i = 0; i < triangles.size(); ++i) {
        Triangle const& triangle = triangles[i];

        Array<T,3> v00 = triangle[0];
        Array<T,3> v11 = triangle[1];
        Array<T,3> v22 = triangle[2];

        Array<T,3> v01 = 0.5 * (v00 + v11);
        Array<T,3> v12 = 0.5 * (v11 + v22);
        Array<T,3> v20 = 0.5 * (v22 + v00);

        Triangle newTriangle;

        newTriangle[0] = v01;
        newTriangle[1] = v12;
        newTriangle[2] = v20;
        newTriangles.push_back(newTriangle);

        newTriangle[0] = v00;
        newTriangle[1] = v01;
        newTriangle[2] = v20;
        newTriangles.push_back(newTriangle);

        newTriangle[0] = v01;
        newTriangle[1] = v11;
        newTriangle[2] = v12;
        newTriangles.push_back(newTriangle);

        newTriangle[0] = v20;
        newTriangle[1] = v12;
        newTriangle[2] = v22;
        newTriangles.push_back(newTriangle);
    }

    triangles.clear();
    triangles = newTriangles;

    computeMinMaxEdges();
    computeBoundingCuboid();
}

template<typename T>
void TriangleSet<T>::reverseOrientation()
{
    plint size = triangles.size();
    for (plint i = 0; i < size; i++) 
        std::swap(triangles[i][1],triangles[i][2]);
}

template<typename T>
void TriangleSet<T>::writeAsciiSTL(std::string fname) const
{
    if (global::mpi().isMainProcessor()) {
        FILE *fp = fopen(fname.c_str(), "w");
        PLB_ASSERT(fp != NULL);

        plint size = triangles.size();
        fprintf(fp, "solid plb\n");
        for (plint i = 0; i < size; i++) {
            Array<T,3> v0 = triangles[i][0];
            Array<T,3> v1 = triangles[i][1];
            Array<T,3> v2 = triangles[i][2];

            Array<T,3> e01 = v1 - v0;
            Array<T,3> e02 = v2 - v0;

            Array<T,3> n;
            crossProduct(e01, e02, n);
            n /= sqrt(VectorTemplateImpl<T,3>::normSqr(n));
            fprintf(fp, "  facet normal % e % e % e\n", (double) n[0], (double) n[1], (double) n[2]);
            fprintf(fp, "    outer loop\n");
            fprintf(fp, "      vertex % e % e % e\n", (double) v0[0], (double) v0[1], (double) v0[2]);
            fprintf(fp, "      vertex % e % e % e\n", (double) v1[0], (double) v1[1], (double) v1[2]);
            fprintf(fp, "      vertex % e % e % e\n", (double) v2[0], (double) v2[1], (double) v2[2]);
            fprintf(fp, "    endloop\n");
            fprintf(fp, "  endfacet\n");
        }
        fprintf(fp, "endsolid plb\n");
        fclose(fp);
    }
}

template<typename T>
void TriangleSet<T>::writeBinarySTL(std::string fname) const
{
    if (global::mpi().isMainProcessor()) {
        FILE *fp = fopen(fname.c_str(), "wb");
        PLB_ASSERT(fp != NULL);

        unsigned int nt = triangles.size();
        unsigned short abc = 0;
        char buf[80];

        for (int i = 0; i < 80; i++)
            buf[i] = '\0';

        fwrite(buf, sizeof(char), 80, fp);
        fwrite(&nt, sizeof(unsigned int), 1, fp);
        for (unsigned int i = 0; i < nt; i++) {
            Array<T,3> v0 = triangles[i][0];
            Array<T,3> v1 = triangles[i][1];
            Array<T,3> v2 = triangles[i][2];

            Array<T,3> e01 = v1 - v0;
            Array<T,3> e02 = v2 - v0;

            Array<T,3> nrml;
            crossProduct(e01, e02, nrml);
            nrml /= sqrt(VectorTemplateImpl<T,3>::normSqr(nrml));

            float n[3];
            n[0] = nrml[0];
            n[1] = nrml[1];
            n[2] = nrml[2];
            fwrite((void *) n, sizeof(float), 3, fp);
            float v[3];
            v[0] = v0[0];
            v[1] = v0[1];
            v[2] = v0[2];
            fwrite((void *) v, sizeof(float), 3, fp);
            v[0] = v1[0];
            v[1] = v1[1];
            v[2] = v1[2];
            fwrite((void *) v, sizeof(float), 3, fp);
            v[0] = v2[0];
            v[1] = v2[1];
            v[2] = v2[2];
            fwrite((void *) v, sizeof(float), 3, fp);
            fwrite(&abc, sizeof(unsigned short), 1, fp);
        }

        fclose(fp);
    }
}

template<typename T>
int TriangleSet<T>::cutTriangleWithPlane(Plane<T> const& plane, Triangle const& triangle,
        TriangleSet<T>& newTriangleSet) const
{
    T epsilon = getEpsilon<T>(precision);

    int vertexTags[3];

    // Tag the triangle vertices.
    for (int iVertex = 0; iVertex < 3; iVertex++) {
        Array<T,3> tmp = triangle[iVertex] - plane.point;
        T norm_tmp = norm(tmp);
        if (norm_tmp > epsilon) {
            tmp /= norm_tmp;
        } else {
            tmp[0] = tmp[1] = tmp[2] = (T) 0.0;
        }
        T dotp = dot(tmp, plane.normal);
        if (fabs(dotp) <= epsilon) {
            vertexTags[iVertex] = 0;
        } else if (dotp > (T) 0.0 && fabs(dotp) > epsilon) {
            vertexTags[iVertex] = -1;
        } else if (dotp < (T) 0.0 && fabs(dotp) > epsilon) {
            vertexTags[iVertex] = 1;
        } else {
            return -1;
        }
    }

    // All three vertices belong to one side of the cut plane.
    if (vertexTags[0] == 1 && vertexTags[1] == 1 && vertexTags[2] == 1) {
        newTriangleSet.triangles.push_back(triangle);
        return 1;
    } else if (vertexTags[0] == -1 && vertexTags[1] == -1 && vertexTags[2] == -1) {
        return 0;
    }

    // One vertex belongs to one side of the cut plane and the other two vertices
    //   belong to the other side.
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) != 3 ? (i + 1) : 0;
        int k = (j + 1) != 3 ? (j + 1) : 0;

        if (vertexTags[i] == 1 && vertexTags[j] == -1 && vertexTags[k] == -1) {
            Array<T,3> intersection_ij((T) 0.0, (T) 0.0, (T) 0.0), intersection_ik((T) 0.0, (T) 0.0, (T) 0.0);
            int rv = 0;
            rv = lineIntersectionWithPlane<T>(plane, triangle[i], triangle[j], precision, intersection_ij);
            if (rv != 1) {
                return -1;
            }
            rv = lineIntersectionWithPlane<T>(plane, triangle[i], triangle[k], precision, intersection_ik);
            if (rv != 1) {
                return -1;
            }
            Triangle newTriangle(triangle[i], intersection_ij, intersection_ik);
            newTriangleSet.triangles.push_back(newTriangle);
            return 1;
        } else if (vertexTags[i] == -1 && vertexTags[j] == 1 && vertexTags[k] == 1) {
            Array<T,3> intersection_ij((T) 0.0, (T) 0.0, (T) 0.0), intersection_ik((T) 0.0, (T) 0.0, (T) 0.0);
            int rv = 0;
            rv = lineIntersectionWithPlane<T>(plane, triangle[i], triangle[j], precision, intersection_ij);
            if (rv != 1) {
                return -1;
            }
            rv = lineIntersectionWithPlane<T>(plane, triangle[i], triangle[k], precision, intersection_ik);
            if (rv != 1) {
                return -1;
            }
            Triangle newTriangle_0(triangle[k], intersection_ij, triangle[j]);
            Triangle newTriangle_1(triangle[k], intersection_ik, intersection_ij);
            newTriangleSet.triangles.push_back(newTriangle_0);
            newTriangleSet.triangles.push_back(newTriangle_1);
            return 1;
        }
    }

    // Only one vertex belongs to the cut plane.
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) != 3 ? (i + 1) : 0;
        int k = (j + 1) != 3 ? (j + 1) : 0;

        if (vertexTags[i] == 0) {
            if (vertexTags[j] == 1 && vertexTags[k] == 1) {
                newTriangleSet.triangles.push_back(triangle);
                return 1;
            } else if (vertexTags[j] == -1 && vertexTags[k] == -1) {
                return 0;
            } else if (vertexTags[j] == 1 && vertexTags[k] == -1) {
                Array<T,3> intersection((T) 0.0, (T) 0.0, (T) 0.0);
                int rv = 0;
                rv = lineIntersectionWithPlane<T>(plane, triangle[j], triangle[k], precision, intersection);
                if (rv != 1) {
                    return -1;
                }
                Triangle newTriangle(triangle[i], triangle[j], intersection);
                newTriangleSet.triangles.push_back(newTriangle);
                return 1;
            } else if (vertexTags[j] == -1 && vertexTags[k] == 1) {
                Array<T,3> intersection((T) 0.0, (T) 0.0, (T) 0.0);
                int rv = 0;
                rv = lineIntersectionWithPlane<T>(plane, triangle[j], triangle[k], precision, intersection);
                if (rv != 1) {
                    return -1;
                }
                Triangle newTriangle(triangle[i], intersection, triangle[k]);
                newTriangleSet.triangles.push_back(newTriangle);
                return 1;
            }
        }
    }

    // Only two of the three vertices belong to the cut plane.
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) != 3 ? (i + 1) : 0;
        int k = (j + 1) != 3 ? (j + 1) : 0;

        if (vertexTags[i] == 0 && vertexTags[j] == 0) {
            if (vertexTags[k] == 1) {
                newTriangleSet.triangles.push_back(triangle);
                return 1;
            } else if (vertexTags[k] == -1) {
                return 0;
            }
        }
    }

    // All 3 vertices belong to the cut plane.
    if (vertexTags[0] == 0 && vertexTags[1] == 0 && vertexTags[2] == 0) {
        newTriangleSet.triangles.push_back(triangle);
        return 1;
    }

    return -1;
}

template<typename T>
int TriangleSet<T>::cutWithPlane(Plane<T> const& plane, TriangleSet<T>& newTriangleSet) const
{
    T epsilon = getEpsilon<T>(precision);

    T norm_normal = norm(plane.normal);
    PLB_ASSERT(norm_normal > epsilon); // The cut plane normal vector cannot have zero magnitude.
    Plane<T> newPlane;
    newPlane.point = plane.point;
    newPlane.normal = plane.normal / norm_normal;

    newTriangleSet.triangles.resize(0);

    newTriangleSet.precision = precision;

    for (pluint iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
        if (cutTriangleWithPlane(newPlane, triangles[iTriangle], newTriangleSet) == -1) {
            return -1;
        }
    }

    if (newTriangleSet.triangles.size() != 0) {
        newTriangleSet.computeMinMaxEdges();
        newTriangleSet.computeBoundingCuboid();
    }

    if (newTriangleSet.triangles.size() == 0 || newTriangleSet.triangles.size() == triangles.size()) {
        return 0;
    }

    return 1;
}

template<typename T>
int TriangleSet<T>::cutWithPlane (
        Plane<T> const& plane, Cuboid<T> const& cuboid, TriangleSet<T>& newTriangleSet ) const
{
    T epsilon = getEpsilon<T>(precision);

    T norm_normal = norm(plane.normal);
    PLB_ASSERT(norm_normal > epsilon); // The cut plane normal vector cannot have zero magnitude.
    Plane<T> newPlane;
    newPlane.point = plane.point;
    newPlane.normal = plane.normal / norm_normal;

    T norm_diagonal = norm(cuboid.upperRightCorner - cuboid.lowerLeftCorner);
    PLB_ASSERT(norm_diagonal > epsilon); // The diagonal of the cuboid cannot have zero length.

    newTriangleSet.triangles.resize(0);

    newTriangleSet.precision = precision;

    for (pluint iTriangle = 0; iTriangle < triangles.size(); iTriangle++) {
        Triangle const& triangle = triangles[iTriangle];

        Array<T,3> vertices[3];
        vertices[0] = triangle[0];
        vertices[1] = triangle[1];
        vertices[2] = triangle[2];

        // Check if the triangle is fully contained in the cuboid.
        int isNotFullyContained = 0;
        for (int iVertex = 0; iVertex < 3; iVertex++) {
            Array<T,3> diff_l;
            diff_l = vertices[iVertex] - cuboid.lowerLeftCorner;

            Array<T,3> diff_u;
            diff_u = vertices[iVertex] - cuboid.upperRightCorner;

            if ((diff_l[0] < (T) 0.0 && fabs(diff_l[0]) > epsilon) ||
                (diff_l[1] < (T) 0.0 && fabs(diff_l[1]) > epsilon) ||
                (diff_l[2] < (T) 0.0 && fabs(diff_l[2]) > epsilon) ||
                (diff_u[0] > (T) 0.0 && fabs(diff_u[0]) > epsilon) ||
                (diff_u[1] > (T) 0.0 && fabs(diff_u[1]) > epsilon) ||
                (diff_u[2] > (T) 0.0 && fabs(diff_u[2]) > epsilon)) {
                isNotFullyContained = 1;
                break;
            }
        }

        if (isNotFullyContained) {
            newTriangleSet.triangles.push_back(triangle);
            continue;
        }

        if (cutTriangleWithPlane(newPlane, triangle, newTriangleSet) == -1)
            return -1;
    }

    if (newTriangleSet.triangles.size() != 0) {
        newTriangleSet.computeMinMaxEdges();
        newTriangleSet.computeBoundingCuboid();
    }

    if (newTriangleSet.triangles.size() == 0 || newTriangleSet.triangles.size() == triangles.size()) {
        return 0;
    }

    return 1;
}

template<typename T>
void TriangleSet<T>::computeMinMaxEdges() {
    T nextMin, nextMax;
    for (pluint i=0; i<triangles.size(); ++i) {
        computeMinMaxEdge(i, nextMin, nextMax);
        minEdgeLength = std::min(minEdgeLength, nextMin);
        maxEdgeLength = std::max(maxEdgeLength, nextMax);
    }
}

template<typename T>
void TriangleSet<T>::computeMinMaxEdge(pluint iTriangle, T& minEdge, T& maxEdge) const {
    PLB_ASSERT( iTriangle<triangles.size() );
    Triangle const& triangle = triangles[iTriangle];
    T edge1 = norm(triangle[1]-triangle[0]);
    T edge2 = norm(triangle[2]-triangle[1]);
    T edge3 = norm(triangle[0]-triangle[2]);
    minEdge = std::min(edge1, std::min(edge2, edge3));
    maxEdge = std::max(edge1, std::max(edge2, edge3));
}

template<typename T>
void TriangleSet<T>::computeBoundingCuboid() {
    T xMin, yMin, zMin;
    T xMax, yMax, zMax;

    xMin = yMin = zMin =  std::numeric_limits<T>::max();
    xMax = yMax = zMax = -std::numeric_limits<T>::max();
    for (pluint i=0; i<triangles.size(); ++i) {
        Triangle const& triangle = triangles[i];

        xMin = std::min(xMin, std::min(triangle[0][0], std::min(triangle[1][0], triangle[2][0])));
        yMin = std::min(yMin, std::min(triangle[0][1], std::min(triangle[1][1], triangle[2][1])));
        zMin = std::min(zMin, std::min(triangle[0][2], std::min(triangle[1][2], triangle[2][2])));

        xMax = std::max(xMax, std::max(triangle[0][0], std::max(triangle[1][0], triangle[2][0])));
        yMax = std::max(yMax, std::max(triangle[0][1], std::max(triangle[1][1], triangle[2][1])));
        zMax = std::max(zMax, std::max(triangle[0][2], std::max(triangle[1][2], triangle[2][2])));
    }
    boundingCuboid.lowerLeftCorner  = Array<T,3>(xMin, yMin, zMin);
    boundingCuboid.upperRightCorner = Array<T,3>(xMax, yMax, zMax);
}

} // namespace plb

#endif  // TRIANGLE_SET_HH
