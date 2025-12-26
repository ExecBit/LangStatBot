#pragma once

#include "IDataManager.h"
#include "IStorage.h"
#include "serialization/ISerializer.h"

namespace io_interface {

class DataManager : public IDataManager {
public:
    explicit DataManager(std::unique_ptr<IStorage> storage, std::unique_ptr<serialization::ISerializer> serializer,
                         std::string_view dataPath)
        : dataPath(dataPath)
        , m_storage(std::move(storage))
        , m_serializer(std::move(serializer)) {}
 
    // TODO корректно читать файл при старте
    bool load(std::string_view src, core::Data& dst) override;

    //bool save(const core::Data& src, std::string_view path) override;
    std::string save(const core::Data& src, std::string_view path) override;

    std::string_view dataPath;
private:
    std::unique_ptr<IStorage> m_storage;
    std::unique_ptr<serialization::ISerializer> m_serializer;
};

};  // namespace io_interface
