/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Parts of this file is taken from the OGRE3D(http://www.ogre3d.org/) project, which uses a MIT license.

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/



#include "OgreMeshDeserializer.h"

#include <sstream>

const std::uint16_t HEADER_STREAM_ID = 0x1000;
const std::uint16_t OTHER_ENDIAN_HEADER_STREAM_ID = 0x0010;

const char* OGRE_MESH_VERSION = "[MeshSerializer_v1.100]";


/** Definition of the OGRE .mesh file format

    .mesh files are binary files (for read efficiency at runtime) and are arranged into chunks
    of data, very like 3D Studio's format.
    A chunk always consists of:
        unsigned short CHUNK_ID        : one of the following chunk ids identifying the chunk
        unsigned long  LENGTH          : length of the chunk in bytes, including this header
        void*          DATA            : the data, which may contain other sub-chunks (various data types)

    A .mesh file can contain both the definition of the Mesh itself, and optionally the definitions
    of the materials is uses (although these can be omitted, if so the Mesh assumes that at runtime the
    Materials referred to by name in the Mesh are loaded/created from another source)

    A .mesh file only contains a single mesh, which can itself have multiple submeshes.

*/

enum MeshChunkID {
    M_HEADER                = 0x1000,
    // char*          version           : Version number check
        M_MESH                = 0x3000,
    // bool skeletallyAnimated   // important flag which affects h/w buffer policies
    // Optional M_GEOMETRY chunk
        M_SUBMESH             = 0x4000,
    // char* materialName
    // bool useSharedVertices
    // unsigned int indexCount
    // bool indexes32Bit
    // unsigned int* faceVertexIndices (indexCount)
    // OR
    // unsigned short* faceVertexIndices (indexCount)
    // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
        M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
    // unsigned short operationType
        M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
    // Optional bone weights (repeating section)
    // unsigned int vertexIndex;
    // unsigned short boneIndex;
    // float weight;
    // Optional chunk that matches a texture name to an alias
    // a texture alias is sent to the submesh material to use this texture name
    // instead of the one in the texture unit with a matching alias name
        M_SUBMESH_TEXTURE_ALIAS = 0x4200, // Repeating section
    // char* aliasName;
    // char* textureName;

    M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
    // unsigned int vertexCount
        M_GEOMETRY_VERTEX_DECLARATION = 0x5100,
    M_GEOMETRY_VERTEX_ELEMENT = 0x5110, // Repeating section
    // unsigned short source;   // buffer bind source
    // unsigned short type;     // VertexElementType
    // unsigned short semantic; // VertexElementSemantic
    // unsigned short offset;   // start offset in buffer in bytes
    // unsigned short index;    // index of the semantic (for colours and texture coords)
        M_GEOMETRY_VERTEX_BUFFER = 0x5200, // Repeating section
    // unsigned short bindIndex;    // Index to bind this buffer to
    // unsigned short vertexSize;   // Per-vertex size, must agree with declaration at this index
        M_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
    // raw buffer data
        M_MESH_SKELETON_LINK = 0x6000,
    // Optional link to skeleton
    // char* skeletonName           : name of .skeleton to use
        M_MESH_BONE_ASSIGNMENT = 0x7000,
    // Optional bone weights (repeating section)
    // unsigned int vertexIndex;
    // unsigned short boneIndex;
    // float weight;
        M_MESH_LOD_LEVEL = 0x8000,
    // Optional LOD information
    // string strategyName;
    // unsigned short numLevels;
    // bool manual;  (true for manual alternate meshes, false for generated)
        M_MESH_LOD_USAGE = 0x8100,
    // Repeating section, ordered in increasing depth
    // NB LOD 0 (full detail from 0 depth) is omitted
    // LOD value - this is a distance, a pixel count etc, based on strategy
    // float lodValue;
        M_MESH_LOD_MANUAL = 0x8110,
    // Required if M_MESH_LOD section manual = true
    // String manualMeshName;
        M_MESH_LOD_GENERATED = 0x8120,
    // Required if M_MESH_LOD section manual = false
    // Repeating section (1 per submesh)
    // unsigned int indexCount;
    // bool indexes32Bit
    // unsigned short* faceIndexes;  (indexCount)
    // OR
    // unsigned int* faceIndexes;  (indexCount)
        M_MESH_BOUNDS = 0x9000,
    // float minx, miny, minz
    // float maxx, maxy, maxz
    // float radius

    // Added By DrEvil
    // optional chunk that contains a table of submesh indexes and the names of
    // the sub-meshes.
        M_SUBMESH_NAME_TABLE = 0xA000,
    // Subchunks of the name table. Each chunk contains an index & string
        M_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,
    // short index
    // char* name

    // Optional chunk which stores precomputed edge data
        M_EDGE_LISTS = 0xB000,
    // Each LOD has a separate edge list
        M_EDGE_LIST_LOD = 0xB100,
    // unsigned short lodIndex
    // bool isManual            // If manual, no edge data here, loaded from manual mesh
    // bool isClosed
    // unsigned long numTriangles
    // unsigned long numEdgeGroups
    // Triangle* triangleList
    // unsigned long indexSet
    // unsigned long vertexSet
    // unsigned long vertIndex[3]
    // unsigned long sharedVertIndex[3]
    // float normal[4]

    M_EDGE_GROUP = 0xB110,
    // unsigned long vertexSet
    // unsigned long triStart
    // unsigned long triCount
    // unsigned long numEdges
    // Edge* edgeList
    // unsigned long  triIndex[2]
    // unsigned long  vertIndex[2]
    // unsigned long  sharedVertIndex[2]
    // bool degenerate

    // Optional poses section, referred to by pose keyframes
        M_POSES = 0xC000,
    M_POSE = 0xC100,
    // char* name (may be blank)
    // unsigned short target    // 0 for shared geometry,
    // 1+ for submesh index + 1
    // bool includesNormals [1.8+]
        M_POSE_VERTEX = 0xC111,
    // unsigned long vertexIndex
    // float xoffset, yoffset, zoffset
    // float xnormal, ynormal, znormal (optional, 1.8+)
    // Optional vertex animation chunk
        M_ANIMATIONS = 0xD000,
    M_ANIMATION = 0xD100,
    // char* name
    // float length
        M_ANIMATION_BASEINFO = 0xD105,
    // [Optional] base keyframe information (pose animation only)
    // char* baseAnimationName (blank for self)
    // float baseKeyFrameTime

    M_ANIMATION_TRACK = 0xD110,
    // unsigned short type          // 1 == morph, 2 == pose
    // unsigned short target        // 0 for shared geometry,
    // 1+ for submesh index + 1
        M_ANIMATION_MORPH_KEYFRAME = 0xD111,
    // float time
    // bool includesNormals [1.8+]
    // float x,y,z          // repeat by number of vertices in original geometry
        M_ANIMATION_POSE_KEYFRAME = 0xD112,
    // float time
        M_ANIMATION_POSE_REF = 0xD113, // repeat for number of referenced poses
    // unsigned short poseIndex
    // float influence

    // Optional submesh extreme vertex list chink
        M_TABLE_EXTREMES = 0xE000,
    // unsigned short submesh_index;
    // float extremes [n_extremes][3];

    /* Version 1.2 of the .mesh format (deprecated)
    enum MeshChunkID {
        M_HEADER                = 0x1000,
            // char*          version           : Version number check
        M_MESH                = 0x3000,
            // bool skeletallyAnimated   // important flag which affects h/w buffer policies
            // Optional M_GEOMETRY chunk
            M_SUBMESH             = 0x4000,
                // char* materialName
                // bool useSharedVertices
                // unsigned int indexCount
                // bool indexes32Bit
                // unsigned int* faceVertexIndices (indexCount)
                // OR
                // unsigned short* faceVertexIndices (indexCount)
                // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
                M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                    // unsigned short operationType
                M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                    // Optional bone weights (repeating section)
                    // unsigned int vertexIndex;
                    // unsigned short boneIndex;
                    // float weight;
            M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
            */
    // unsigned int vertexCount
    // float* pVertices (x, y, z order x numVertices)
        M_GEOMETRY_NORMALS = 0x5100,    //(Optional)
    // float* pNormals (x, y, z order x numVertices)
        M_GEOMETRY_COLOURS = 0x5200,    //(Optional)
    // unsigned long* pColours (RGBA 8888 format x numVertices)
        M_GEOMETRY_TEXCOORDS = 0x5300    //(Optional, REPEATABLE, each one adds an extra set)
    // unsigned short dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
    // float* pTexCoords  (u [v] [w] order, dimensions x numVertices)
    /*
    M_MESH_SKELETON_LINK = 0x6000,
        // Optional link to skeleton
        // char* skeletonName           : name of .skeleton to use
    M_MESH_BONE_ASSIGNMENT = 0x7000,
        // Optional bone weights (repeating section)
        // unsigned int vertexIndex;
        // unsigned short boneIndex;
        // float weight;
    M_MESH_LOD = 0x8000,
        // Optional LOD information
        // unsigned short numLevels;
        // bool manual;  (true for manual alternate meshes, false for generated)
        M_MESH_LOD_USAGE = 0x8100,
        // Repeating section, ordered in increasing depth
        // NB LOD 0 (full detail from 0 depth) is omitted
        // float fromSquaredDepth;
            M_MESH_LOD_MANUAL = 0x8110,
            // Required if M_MESH_LOD section manual = true
            // String manualMeshName;
            M_MESH_LOD_GENERATED = 0x8120,
            // Required if M_MESH_LOD section manual = false
            // Repeating section (1 per submesh)
            // unsigned int indexCount;
            // bool indexes32Bit
            // unsigned short* faceIndexes;  (indexCount)
            // OR
            // unsigned int* faceIndexes;  (indexCount)
    M_MESH_BOUNDS = 0x9000
        // float minx, miny, minz
        // float maxx, maxy, maxz
        // float radius

    // Added By DrEvil
    // optional chunk that contains a table of submesh indexes and the names of
    // the sub-meshes.
    M_SUBMESH_NAME_TABLE,
        // Subchunks of the name table. Each chunk contains an index & string
        M_SUBMESH_NAME_TABLE_ELEMENT,
            // short index
            // char* name

*/
};
/** @} */
/** @} */


/** Reverses byte order of chunks in buffer, where 'size' is size of one chunk.
*/
static inline void bswapChunks(void * pData, size_t size, size_t count)
{
    for(size_t c = 0; c < count; ++c)
    {
        for(char *p0 = (char*)pData + c * size, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1)
        {
            char swapByte = *p0;
            *p0 = *p1;
            *p1 = swapByte;
        }
    }
}

/// Vertex element semantics, used to identify the meaning of vertex buffer contents
enum VertexElementSemantic
{
    /// Position, 3 reals per vertex
        VES_POSITION = 1,
    /// Blending weights
        VES_BLEND_WEIGHTS = 2,
    /// Blending indices
        VES_BLEND_INDICES = 3,
    /// Normal, 3 reals per vertex
        VES_NORMAL = 4,
    /// Diffuse colours
        VES_DIFFUSE = 5,
    /// Specular colours
        VES_SPECULAR = 6,
    /// Texture coordinates
        VES_TEXTURE_COORDINATES = 7,
    /// Binormal (Y axis if normal is Z)
        VES_BINORMAL = 8,
    /// Tangent (X axis if normal is Z)
        VES_TANGENT = 9,
    /// The  number of VertexElementSemantic elements (note - the first value VES_POSITION is 1)
        VES_COUNT = 9
};

/**
 * Vertex element type, used to identify the base types of the vertex contents
 *
 * @note VET_SHORT1, VET_SHORT3, VET_USHORT1 and VET_USHORT3 should never be used
 * because they aren't supported on any known hardware - they are unaligned as their size
 * is not a multiple of 4 bytes. Therefore drivers usually must add padding on upload.
 */
enum VertexElementType
{
    VET_FLOAT1 = 0,
    VET_FLOAT2 = 1,
    VET_FLOAT3 = 2,
    VET_FLOAT4 = 3,
    /// alias to more specific colour type - use the current rendersystem's colour packing
        VET_COLOUR = 4,
    VET_SHORT1 = 5,  ///< @deprecated (see #VertexElementType note)
    VET_SHORT2 = 6,
    VET_SHORT3 = 7,  ///< @deprecated (see #VertexElementType note)
    VET_SHORT4 = 8,
    VET_UBYTE4 = 9,
    /// D3D style compact colour
        VET_COLOUR_ARGB = 10,
    /// GL style compact colour
        VET_COLOUR_ABGR = 11,

    // the following are not universally supported on all hardware:
        VET_DOUBLE1 = 12,
    VET_DOUBLE2 = 13,
    VET_DOUBLE3 = 14,
    VET_DOUBLE4 = 15,
    VET_USHORT1 = 16,  ///< @deprecated (see #VertexElementType note)
    VET_USHORT2 = 17,
    VET_USHORT3 = 18,  ///< @deprecated (see #VertexElementType note)
    VET_USHORT4 = 19,
    VET_INT1 = 20,
    VET_INT2 = 21,
    VET_INT3 = 22,
    VET_INT4 = 23,
    VET_UINT1 = 24,
    VET_UINT2 = 25,
    VET_UINT3 = 26,
    VET_UINT4 = 27,
    VET_BYTE4 = 28,  /// signed bytes
    VET_BYTE4_NORM = 29,   /// signed bytes (normalized to -1..1)
    VET_UBYTE4_NORM = 30,  /// unsigned bytes (normalized to 0..1)
    VET_SHORT2_NORM = 31,  /// signed shorts (normalized to -1..1)
    VET_SHORT4_NORM = 32,
    VET_USHORT2_NORM = 33, /// unsigned shorts (normalized to 0..1)
    VET_USHORT4_NORM = 34
};


OgreMeshDeserializer::OgreMeshDeserializer(std::ifstream& stream)
    : m_radius(0),
      m_stream(stream),
      m_flipEndian(false),
      mCurrentstreamLen(0)
{

}

void OgreMeshDeserializer::deserialize()
{

    // Determine endianness (must be the first thing we do!)
    determineEndianness(m_stream);

    // Check header
    readFileHeader(m_stream);

    unsigned short streamID = readChunk(m_stream);

    while (!m_stream.eof()) {
        switch (streamID) {
            case M_MESH:
                readMesh();
                break;
            default:
                skipChunk(m_stream);
        }

        streamID = readChunk(m_stream);
    }

}

void OgreMeshDeserializer::readGeometry()
{
    std::vector<std::vector<char>> vertexBuffers;
    std::vector<OgreMeshDeserializer::VertexElement> elements;

    unsigned int vertexCount = 0;
    readInts(m_stream, &vertexCount, 1);
    // Find optional geometry streams
    if (!m_stream.eof()) {

        unsigned short streamID = readChunk(m_stream);
        while (!m_stream.eof() &&
               (streamID == M_GEOMETRY_VERTEX_DECLARATION ||
                streamID == M_GEOMETRY_VERTEX_BUFFER)) {
            switch (streamID) {
                case M_GEOMETRY_VERTEX_DECLARATION:
                    elements = readGeometryVertexDeclaration();
                    break;
                case M_GEOMETRY_VERTEX_BUFFER:
                    vertexBuffers.emplace_back(readGeometryVertexBuffer(vertexCount));
                    break;
                default:
                    skipChunk(m_stream);
            }
            // Get next stream
            if (!m_stream.eof()) {
                streamID = readChunk(m_stream);
            }
        }

        if (!elements.empty()) {
            for (auto& element : elements) {
                if (element.vSemantic == VertexElementSemantic::VES_POSITION) {
                    auto& vertexBuffer = vertexBuffers[element.source];
                    size_t vertexSize = vertexBuffer.size() / vertexCount;

                    for (size_t i = 0; i < vertexCount; ++i) {
                        if (element.vType == VertexElementType::VET_FLOAT3) {
                            char* vertexStart = vertexBuffer.data() + (i * vertexSize);
                            char* positionStart = vertexStart + element.offset;
                            auto positions = reinterpret_cast<float*>(positionStart);
                            m_vertices.push_back(*positions);
                            positions++;
                            m_vertices.push_back(*positions);
                            positions++;
                            m_vertices.push_back(*positions);
                        }
                    }

                }
            }
        }

        if (!m_stream.eof()) {
            // Backpedal back to start of non-submesh stream
            backpedalChunkHeader(m_stream);
        }

    }

}

//---------------------------------------------------------------------
std::vector<OgreMeshDeserializer::VertexElement> OgreMeshDeserializer::readGeometryVertexDeclaration()
{
    std::vector<OgreMeshDeserializer::VertexElement> elements;
    // Find optional geometry streams
    if (!m_stream.eof()) {

        unsigned short streamID = readChunk(m_stream);
        while (!m_stream.eof() &&
               (streamID == M_GEOMETRY_VERTEX_ELEMENT)) {
            switch (streamID) {
                case M_GEOMETRY_VERTEX_ELEMENT:
                    elements.push_back(readGeometryVertexElement());
                    break;
                default:
                    break;
            }
            // Get next m_stream
            if (!m_stream.eof()) {
                streamID = readChunk(m_stream);
            }
        }

        if (!m_stream.eof()) {
            // Backpedal back to start of non-submesh m_stream
            backpedalChunkHeader(m_stream);
        }

    }
    return elements;
}

//---------------------------------------------------------------------
OgreMeshDeserializer::VertexElement OgreMeshDeserializer::readGeometryVertexElement()
{
    VertexElement element{};
    // unsigned short source;   // buffer bind source
    readShorts(m_stream, &element.source, 1);
    // unsigned short type;     // VertexElementType
    readShorts(m_stream, &element.vType, 1);
    // unsigned short semantic; // VertexElementSemantic
    readShorts(m_stream, &element.vSemantic, 1);
    // unsigned short offset;   // start offset in buffer in bytes
    readShorts(m_stream, &element.offset, 1);
    // unsigned short index;    // index of the semantic
    readShorts(m_stream, &element.index, 1);
    return element;
}

//---------------------------------------------------------------------
std::vector<char> OgreMeshDeserializer::readGeometryVertexBuffer(unsigned int vertexCount)
{
    std::vector<char> vertices;
    unsigned short bindIndex, vertexSize;
    // unsigned short bindIndex;    // Index to bind this buffer to
    readShorts(m_stream, &bindIndex, 1);
    // unsigned short vertexSize;   // Per-vertex size, must agree with declaration at this index
    readShorts(m_stream, &vertexSize, 1);

    {
        // Check for vertex data header
        unsigned short headerID;
        headerID = readChunk(m_stream);
        if (headerID != M_GEOMETRY_VERTEX_BUFFER_DATA) {
            throw std::runtime_error("Can't find vertex buffer data area");
        }

        // Create / populate vertex buffer
        vertices.resize(vertexCount * vertexSize);
        m_stream.read(vertices.data(), vertexCount * vertexSize);

    }

    return vertices;
}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readMesh()
{
    //First value is whether it's skeletally animated
    m_stream.seekg(sizeof(bool), std::ios_base::cur);

    // Find all substreams
    if (!m_stream.eof()) {

        unsigned short streamID = readChunk(m_stream);
        while (!m_stream.eof() &&
               (streamID == M_GEOMETRY ||
                streamID == M_SUBMESH ||
                streamID == M_MESH_SKELETON_LINK ||
                streamID == M_MESH_BONE_ASSIGNMENT ||
                streamID == M_MESH_LOD_LEVEL ||
                streamID == M_MESH_BOUNDS ||
                streamID == M_SUBMESH_NAME_TABLE ||
                streamID == M_EDGE_LISTS ||
                streamID == M_POSES ||
                streamID == M_ANIMATIONS ||
                streamID == M_TABLE_EXTREMES)) {
            switch (streamID) {
                case M_GEOMETRY:
                    readGeometry();
                    break;
                case M_SUBMESH:
                    readSubMesh();
                    break;
                case M_MESH_BOUNDS:
                    readBoundsInfo();
                    break;
                default:
                    skipChunk(m_stream);
            }

            if (!m_stream.eof()) {
                streamID = readChunk(m_stream);
            }

        }
        if (!m_stream.eof()) {
            // Backpedal back to start of m_stream
            backpedalChunkHeader(m_stream);
        }

    }

}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readSubMesh()
{

    readString(m_stream); //Read material name first, which we discard.

    bool useSharedVertices;
    readBools(m_stream, &useSharedVertices, 1);

    size_t offset = 0;
    if (!useSharedVertices) {
        offset = m_vertices.size() / 3;
    }

    unsigned int indexCount = 0;
    readInts(m_stream, &indexCount, 1);

    bool idx32bit;
    readBools(m_stream, &idx32bit, 1);
    if (indexCount > 0) {
        if (idx32bit) {

            std::vector<std::uint32_t> indices;
            indices.resize(indexCount);
            readInts(m_stream, indices.data(), indexCount);

            for (auto& index : indices) {
                m_indices.emplace_back(index + offset);
            }

        } else // 16-bit
        {
            std::vector<std::uint16_t> indices;
            indices.resize(indexCount);
            readShorts(m_stream, indices.data(), indexCount);
            for (auto& index : indices) {
                m_indices.emplace_back(index + offset);
            }
        }
    }


    {
        // M_GEOMETRY m_stream (Optional: present only if useSharedVertices = false)
        if (!useSharedVertices) {
            unsigned short streamID = readChunk(m_stream);
            if (streamID != M_GEOMETRY) {
                throw std::runtime_error("Missing geometry data in mesh file");
            }
            readGeometry();
        }


        // Find all bone assignments, submesh operation, and texture aliases (if present)
        if (!m_stream.eof()) {
            unsigned short streamID = readChunk(m_stream);
            while (!m_stream.eof() &&
                   (streamID == M_SUBMESH_BONE_ASSIGNMENT ||
                    streamID == M_SUBMESH_OPERATION ||
                    streamID == M_SUBMESH_TEXTURE_ALIAS)) {
                skipChunk(m_stream);

                if (!m_stream.eof()) {
                    streamID = readChunk(m_stream);
                }

            }
            if (!m_stream.eof()) {
                // Backpedal back to start of m_stream
                backpedalChunkHeader(m_stream);
            }
        }
    }


}

void OgreMeshDeserializer::readBoundsInfo()
{
    float minx, miny, minz, maxx, maxy, maxz;
    readFloats(m_stream, &minx, 1);
    readFloats(m_stream, &miny, 1);
    readFloats(m_stream, &minz, 1);
    readFloats(m_stream, &maxx, 1);
    readFloats(m_stream, &maxy, 1);
    readFloats(m_stream, &maxz, 1);
    WFMath::Point<3> min(minx, miny, minz);
    WFMath::Point<3> max(maxx, maxy, maxz);

    min.setValid(true);
    max.setValid(true);
    m_bounds = WFMath::AxisBox<3>(min, max);
    readFloats(m_stream, &m_radius, 1);
}

void OgreMeshDeserializer::determineEndianness(std::istream& stream)
{
    if (stream.tellg() != 0) {
        throw std::runtime_error(
            "Can only determine the endianness of the input stream if it "
                "is at the start");
    }

    std::uint16_t dest;
    // read header id manually (no conversion)
    stream.read(reinterpret_cast<char*>(&dest), sizeof(std::uint16_t));
    auto actually_read = stream.gcount();
    // skip back
    stream.seekg(0);
    if (actually_read != sizeof(std::uint16_t)) {
        // end of file?
        throw std::runtime_error("Couldn't read 16 bit header value from input stream.");
    }
    if (dest == HEADER_STREAM_ID) {
        m_flipEndian = false;
    } else if (dest == OTHER_ENDIAN_HEADER_STREAM_ID) {
        m_flipEndian = true;
    } else {
        throw std::runtime_error("Header chunk didn't match either endian: Corrupted stream?");
    }
}


//---------------------------------------------------------------------
void OgreMeshDeserializer::readFileHeader(std::istream& stream)
{
    unsigned short headerID;

    // Read header ID
    readShorts(stream, &headerID, 1);

    if (headerID == HEADER_STREAM_ID) {
        // Read version
        std::string ver = readString(stream);
        if (ver != OGRE_MESH_VERSION) {
            throw std::runtime_error("Invalid file: version incompatible, file reports " + ver +
                               " Deserializer is version " + OGRE_MESH_VERSION);
        }
    } else {
        throw std::runtime_error("Invalid file: no header");
    }

}

//---------------------------------------------------------------------
unsigned short OgreMeshDeserializer::readChunk(std::istream& stream)
{
    unsigned short id;
    readShorts(stream, &id, 1);

    readInts(stream, &mCurrentstreamLen, 1);

    return id;
}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readBools(std::istream& stream, bool* pDest, size_t count)
{
    char pTemp[count];
    stream.read(pTemp, 1 * count);
    for (unsigned int i = 0; i < count; i++)
        *(pDest + i) = *(pTemp + i);

    //no flipping on 1-byte datatypes
}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readFloats(std::istream& stream, float* pDest, size_t count)
{
    stream.read(reinterpret_cast<char*>(pDest), sizeof(float) * count);
    flipFromLittleEndian(pDest, sizeof(float), count);
}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readShorts(std::istream& stream, unsigned short* pDest, size_t count)
{
    stream.read(reinterpret_cast<char*>(pDest), sizeof(unsigned short) * count);
    flipFromLittleEndian(pDest, sizeof(unsigned short), count);
}

//---------------------------------------------------------------------
void OgreMeshDeserializer::readInts(std::istream& stream, std::uint32_t* pDest, size_t count)
{
    stream.read(reinterpret_cast<char*>(pDest), sizeof(std::uint32_t) * count);
    flipFromLittleEndian(pDest, sizeof(std::uint32_t), count);
}

//---------------------------------------------------------------------
std::string OgreMeshDeserializer::readString(std::istream& stream)
{
    std::stringbuf sb;
    stream.get(sb);
    stream.seekg(1, std::ios_base::cur); //skip delimiter
    return sb.str();
}

void OgreMeshDeserializer::flipFromLittleEndian(void* pData, size_t size, size_t count)
{
    if (m_flipEndian) {
        bswapChunks(pData, size, count);
    }
}

size_t OgreMeshDeserializer::calcChunkHeaderSize()
{
    return sizeof(std::uint16_t) + sizeof(std::uint32_t);
}

void OgreMeshDeserializer::skipChunk(std::istream& stream)
{
    stream.seekg(mCurrentstreamLen - calcChunkHeaderSize(), std::ios_base::cur);
}

void OgreMeshDeserializer::backpedalChunkHeader(std::istream& stream)
{
    if (!stream.eof()) {
        stream.seekg(-(int) calcChunkHeaderSize(), std::ios_base::cur);
    }

}

