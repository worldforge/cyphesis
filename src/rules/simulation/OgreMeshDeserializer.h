/*
 Copyright (C); 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option); any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_OGREMESHDESERIALIZER_H
#define CYPHESIS_OGREMESHDESERIALIZER_H


#include <iostream>
#include <fstream>
#include <wfmath/wfmath.h>
#include <vector>
#include <LinearMath/btScalar.h>

/**
 * Deserializas OGRE3D meshes and extracts geometry only.
 * 
 * It only handles meshes in the 1.100 format (1.10+).
 */
class OgreMeshDeserializer
{
    public:
        explicit OgreMeshDeserializer(std::ifstream& stream);;

        void deserialize();

        std::vector<btScalar> m_vertices;
        std::vector<unsigned int> m_indices;
        WFMath::AxisBox<3> m_bounds;
        float m_radius;

    protected:


        struct VertexElement
        {
            unsigned short source, offset, index, vType, vSemantic;
        };

        std::ifstream& m_stream;
        bool m_flipEndian;
        std::uint32_t mCurrentstreamLen;

        void determineEndianness(std::istream& stream);;

        void readFileHeader(std::istream& stream);;

        unsigned short readChunk(std::istream& stream);;

        void readBools(std::istream& stream, bool* pDest, size_t count);

        void readFloats(std::istream& stream, float* pDest, size_t count);

        void readShorts(std::istream& stream, unsigned short* pDest, size_t count);

        void readInts(std::istream& stream, std::uint32_t* pDest, size_t count);

        std::string readString(std::istream& stream);

        void flipFromLittleEndian(void* pData, size_t size, size_t count);

        size_t calcChunkHeaderSize();

        void backpedalChunkHeader(std::istream& stream);

        void skipChunk(std::istream& stream);

        void readMesh();

        void readSubMesh();

        void readGeometry();

        std::vector<OgreMeshDeserializer::VertexElement> readGeometryVertexDeclaration();

        OgreMeshDeserializer::VertexElement readGeometryVertexElement();

        std::vector<char> readGeometryVertexBuffer(unsigned int vertexCount);

        void readBoundsInfo();


};


#endif //CYPHESIS_OGREMESHDESERIALIZER_H
