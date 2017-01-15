#pragma once

#include "MeshUtils/RawVector.h"
#include "MeshUtils/MeshUtils.h"

namespace ms {

enum class MessageType
{
    Unknown,
    Get,
    Delete,
    Mesh,
};

struct GetDataCS;
struct DeleteDataCS;
struct MeshDataCS;


class MessageData
{
public:
    virtual ~MessageData();
    virtual uint32_t getSerializeSize() const = 0;
    virtual void serialize(std::ostream& os) const = 0;
    virtual void deserialize(std::istream& is) = 0;
};


// Get request

struct GetFlags
{
    uint32_t get_transform : 1;
    uint32_t get_points : 1;
    uint32_t get_normals : 1;
    uint32_t get_tangents : 1;
    uint32_t get_uv : 1;
    uint32_t get_indices : 1;
    uint32_t get_bones : 1;
    uint32_t swap_handedness : 1;
    uint32_t swap_faces : 1;
    uint32_t apply_transform : 1;
    uint32_t bake_skin : 1;
};

class GetData : public MessageData
{
public:
    GetFlags flags = {0};
    float scale = 1.0f;

    GetData();
    uint32_t getSerializeSize() const;
    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};


// Delete request

class DeleteData : public MessageData
{
using super = MessageData;
public:
    std::string path;

    DeleteData();
    uint32_t getSerializeSize() const;
    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};


// Mesh

struct MeshRefineFlags
{
    uint32_t split : 1;
    uint32_t gen_normals : 1;
    uint32_t gen_tangents : 1;
    uint32_t swap_handedness : 1;
    uint32_t swap_faces : 1;
    uint32_t apply_transform : 1;
};

struct MeshDataFlags
{
    uint32_t has_transform : 1;
    uint32_t has_indices : 1;
    uint32_t has_counts : 1;
    uint32_t has_points : 1;
    uint32_t has_normals : 1;
    uint32_t has_tangents : 1;
    uint32_t has_uv : 1;
};

struct MeshRefineSettings
{
    MeshRefineFlags flags = { 0 };
    float scale = 1.0f;
    int split_unit = 65000;
};

struct Transform
{
    float3   position = float3::zero();
    quatf    rotation = quatf::identity();
    float3   rotation_eularZXY = float3::zero();
    float3   scale = float3::one();
    float4x4 local2world = float4x4::identity();
    float4x4 world2local = float4x4::identity();
};

struct Submesh
{
    IntrusiveArray<float3> points;
    IntrusiveArray<float3> normals;
    IntrusiveArray<float4> tangents;
    IntrusiveArray<float2> uv;
    RawVector<int> indices;
};

struct ClientSpecificData
{
    enum class Type {
        NoData,
        Metasequia,
    };
    Type type;
    struct Metasequoia {
        float smooth_angle = 0.0f;
    } mq;
};

class MeshData : public MessageData
{
using super = MessageData;
public:
    using SubmeshPtr = std::shared_ptr<Submesh>;
    using Submeshes = std::vector<SubmeshPtr>;

    std::string       path;
    MeshDataFlags     flags = {0};
    RawVector<float3> points;
    RawVector<float3> normals;
    RawVector<float4> tangents;
    RawVector<float2> uv;
    RawVector<int>    counts;
    RawVector<int>    indices;

    Transform         transform;
    ClientSpecificData csd;

    Submeshes submeshes;
    int num_submeshes = 0;

    MeshData();
    MeshData(const MeshDataCS& cs);
    void clear();
    uint32_t getSerializeSize() const;
    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    void swap(MeshData& v);
    void refine(const MeshRefineSettings &s);
    void applyTransform(const float4x4& t);
};


struct GetDataCS
{
    GetFlags flags = {};

    GetDataCS(const GetData& v);
};

struct DeleteDataCS
{
    const char *obj_path = nullptr;

    DeleteDataCS(const DeleteData& v);
};

struct MeshDataCS
{
    MeshDataFlags flags = {0};
    MeshData *cpp = nullptr;
    const char *path = nullptr;
    float3 *points = nullptr;
    float3 *normals = nullptr;
    float4 *tangents = nullptr;
    float2 *uv = nullptr;
    int *indices = nullptr;
    int num_points = 0;
    int num_indices = 0;
    int num_submeshes = 0;
    Transform transform;

    MeshDataCS(const MeshData& v);
};

struct SubmeshDataCS
{
    float3 *points = nullptr;
    float3 *normals = nullptr;
    float4 *tangents = nullptr;
    float2 *uv = nullptr;
    int *indices = nullptr;
    int num_points = 0;
    int num_indices = 0;

    SubmeshDataCS();
    SubmeshDataCS(const Submesh& v);
};

} // namespace ms
