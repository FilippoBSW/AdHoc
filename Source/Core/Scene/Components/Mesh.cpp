// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#include "Mesh.hpp"
#include <Math/Math.hpp>
#include <Utility.hpp>
#include <Vulkan/Context.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace adh {
    // FIXME: Assimp fails with big .obj files
    void Mesh::Load(const std::string& meshPath) {
        Assimp::Importer imp;

        auto pModel = imp.ReadFile(
            meshPath.data(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        ADH_THROW(pModel, "Assimp failed to load model!");

        if (pModel) {
            if (vertexBuffer) {
                vertexBuffer.Destroy();
                vertex.Clear();
            }

            if (indexBuffer) {
                indexBuffer.Destroy();
                index.Clear();
            }

            auto pMesh = pModel->mMeshes[0];

            vertex.Reserve(pMesh->mNumVertices);

            for (std::size_t i{}; i != pMesh->mNumVertices; ++i) {
                vertex.EmplaceBack(
                    Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
                    Vector3D{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z });
            }

            index.Reserve(pMesh->mNumFaces * 3);
            for (std::size_t i{}; i != pMesh->mNumFaces; ++i) {
                const auto& face = pMesh->mFaces[i];
                index.EmplaceBack(face.mIndices[0]);
                index.EmplaceBack(face.mIndices[1]);
                index.EmplaceBack(face.mIndices[2]);
            }

            vertexBuffer.Create(vertex);
            indexBuffer.Create(index);

            auto p = meshPath.find_last_of("/");

            meshName     = meshPath.substr(p + 1);
            meshFilePath = meshPath;
        }
    }

    void Mesh::Load2(const char* fileName) {
        Load(vk::Context::Get()->GetDataDirectory() + "Assets/Models/" + fileName);
    }
} // namespace adh
