#pragma once
#include "../Scene/Mesh.h"

#include <vector>
#include <memory>
class MeshRegistry
{
public:
    static MeshRegistry& instance()
    {
        static MeshRegistry inst;
        return inst;
    }

    MeshRegistry(const MeshRegistry&) = delete;
    MeshRegistry& operator=(const MeshRegistry&) = delete;

    void add(Mesh* m) { allmeshes.push_back(m); }
    template<class Range>
    void append(const Range& r) { allmeshes.insert(allmeshes.end(), std::begin(r), std::end(r)); }

    void clear(){ allmeshes.clear(); }

    std::vector<Mesh*> getotalMeshes() const noexcept { return allmeshes; }

private:
    MeshRegistry() = default;
    ~MeshRegistry() = default;

    std::vector<Mesh*> allmeshes;
};

