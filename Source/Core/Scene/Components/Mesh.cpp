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

        if (pModel) {
            bufferData = Mesh::meshes[meshPath];

            if (!bufferData) {
                bufferData             = MakeShared<MeshBufferData>();
                Mesh::meshes[meshPath] = bufferData;

                auto pMesh = pModel->mMeshes[0];

                bufferData->vertices.Reserve(pMesh->mNumVertices);

                for (std::size_t i{}; i != pMesh->mNumVertices; ++i) {
                    bufferData->vertices.EmplaceBack(
                        Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
                        Vector3D{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z },
                        Vector2D{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y });
                    bufferData->vertices2.EmplaceBack(Vector3D{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z });
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
