#pragma once

#include <Tempest/Texture2d>
#include <Tempest/Matrix4x4>
#include <Tempest/Uniforms>
#include <Tempest/UniformBuffer>
#include <Tempest/AlignedArray>

#include <vector>
#include <list>

#include "graphics/submesh/staticmesh.h"
#include "graphics/submesh/animmesh.h"
#include "protomesh.h"
#include "objectsbucket.h"
#include "ubochain.h"

class RendererStorage;

class StaticObjects final {
  private:
    using Item = AbstractObjectsBucket::Item;

  public:
    StaticObjects(const RendererStorage& storage);

    class Mesh final {
      public:
        Mesh()=default;
        Mesh(const ProtoMesh* mesh,std::unique_ptr<Item[]>&& sub,size_t subCount):sub(std::move(sub)),subCount(subCount),ani(mesh){}

        void setObjMatrix(const Tempest::Matrix4x4& mt);

      private:
        std::unique_ptr<Item[]> sub;
        size_t                  subCount=0;
        const ProtoMesh*        ani=nullptr;

        void setObjMatrix(const ProtoMesh &ani, const Tempest::Matrix4x4& mt, size_t parent);
      };

    Mesh get(const StaticMesh& mesh);
    Mesh get(const ProtoMesh&  mesh);

    void updateUbo(uint32_t imgId);
    void commitUbo(uint32_t imgId);

    void draw     (Tempest::CommandBuffer &cmd, uint32_t imgId);

    bool needToUpdateCommands() const;
    void setAsUpdated();

    void setModelView(const Tempest::Matrix4x4& m);

  private:
    struct UboGlobal final {
      std::array<float,3>           lightDir={{0,0,1}};
      float                         padding=0;
      Tempest::Matrix4x4            modelView;
      };

    struct UboSt final {
      Tempest::Matrix4x4 obj;
      };

    struct UboDn final {
      Tempest::Matrix4x4 obj;
      };

    const RendererStorage&          storage;

    std::list<ObjectsBucket<UboSt>> chunksSt;
    std::list<ObjectsBucket<UboDn>> chunksDn;

    UboChain<UboGlobal>             uboGlobalPf;
    UboGlobal                       uboGlobal;

    bool                            nToUpdate=true; //invalidate cmd buffers

    ObjectsBucket<UboSt>&           getBucketSt(const Tempest::Texture2d* mat);
    ObjectsBucket<UboDn>&           getBucketDn(const Tempest::Texture2d* mat);

    Item                            implGet(const StaticMesh& mesh,
                                            const Tempest::Texture2d* mat,
                                            const Tempest::IndexBuffer<uint32_t> &ibo);
    Item                            implGet(const AnimMesh& mesh,
                                            const Tempest::Texture2d* mat,
                                            const Tempest::IndexBuffer<uint32_t> &ibo);
  };
