// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
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
#include "Math/source/Numbers.hpp"
#include <Event/Event.hpp>
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

        ADH_THROW(pModel, imp.GetErrorString());

        // FIXME: Blender temp fbx import fix
        auto pos       = meshPath.find_last_of('.');
        auto buffer    = meshPath.substr(pos + 1, meshPath.size());
        bool rotateFbx = false;
        if (buffer == "fbx") {
            rotateFbx = true;
        }

        if (pModel) {
            bufferData = Mesh::meshes[meshPath];

            if (!bufferData) {
                bufferData             = MakeShared<MeshBufferData>();
                Mesh::meshes[meshPath] = bufferData;

                auto pMesh = pModel->mMeshes[0];

                bufferData->vertices.Reserve(pMesh->mNumVertices);

                for (std::size_t i{}; i != pMesh->mNumVertices; ++i) {
                    if (rotateFbx) {
                        bufferData->vertices.EmplaceBack(
                            Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }.Rotate(Vector3D{ ToRadians(90.0f), 0.0f, 0.0f }),
                            Vector3D{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }.Rotate(Vector3D{ ToRadians(90.0f), 0.0f, 0.0f }),
                            Vector2D{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y });
                        bufferData->vertices2.EmplaceBack(Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }.Rotate(Vector3D{ ToRadians(90.0f), 0.0f, 0.0f }));
                    } else{
                        bufferData->vertices.EmplaceBack(
                            Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
                            Vector3D{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z },
                            Vector2D{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y });
                        bufferData->vertices2.EmplaceBack(Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z });
                    }
                }

                bufferData->indices.Reserve(pMesh->mNumFaces * 3);
                for (std::size_t i{}; i != pMesh->mNumFaces; ++i) {
                    const auto& face = pMesh->mFaces[i];
                    bufferData->indices.EmplaceBack(face.mIndices[0]);
                    bufferData->indices.EmplaceBack(face.mIndices[1]);
                    bufferData->indices.EmplaceBack(face.mIndices[2]);
                }
                bufferData->vertex.Create(bufferData->vertices);

                bufferData->index.Create(bufferData->indices);

                auto p = meshPath.find_last_of("/");

                bufferData->meshName     = meshPath.substr(p + 1);
                bufferData->meshFilePath = meshPath;
            }
        } else {
            std::string s{ "[" + meshPath + "] " + "Model does not exit!" };
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, s.data());
        }
    }

    void Mesh::Load2(const char* fileName) {
        Load(vk::Context::Get()->GetDataDirectory() + "Assets/Models/" + fileName);
    }
} // namespace adh
