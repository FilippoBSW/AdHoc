#pragma once
#include <Std/UniquePtr.hpp>
#include <string>

// TODO: efficient load-save from play
namespace adh {
    class Scene;
    class Serializer {

      public:
        Serializer(Scene* scene);

        void Serialize();

        void SerializeToFile(const char* filePath);

        void Deserialize();

        void DeserializeFromFile(const char* filePath);

      private:
        void Deserialize(void* pNode);

      private:
        std::string m_Data;
        Scene* m_Scene;
    };
} // namespace adh
