#pragma once

#include "IDataManager.h"
#include "IStorage.h"
#include "json/json.h"
#include "serialization/ISerializer.h"

namespace io_interface {

class DataManager : public IDataManager {
public:
    explicit DataManager(std::unique_ptr<IStorage> storage, std::unique_ptr<serialization::ISerializer> serializer)
        : m_storage(std::move(storage))
        , m_serializer(std::move(serializer)) {}
    bool load(std::string_view src, core::Data& dst) override;
    bool save(std::string_view file, std::string_view path) override;

private:
    std::unique_ptr<IStorage> m_storage;
    std::unique_ptr<serialization::ISerializer> m_serializer;
    Json::Value m_root;
};

};  // namespace io_interface
