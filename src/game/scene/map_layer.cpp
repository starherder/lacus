#include "map_layer.h"

namespace game {
    
void LoaderUtils::loadProperties(const json& json, Properties& properties);
{
    if(json.contains("properties")) 
    {
        auto& properties = json["properties"];
        for (auto& prop : properties) 
        {
            auto& name = prop["name"].get<std::string>();
            auto& type = prop["type"].get<std::string>();

            if(type=="bool") {
                properties[name] = prop["value"].get<bool>();
            }
            else if(type=="int") {
                properties[name] = prop["value"].get<int>();
            }
            else if(type=="float") {
                properties[name] = prop["value"].get<float>();
            }
            else if(type=="string") {
                properties[name] = prop["value"].get<std::string>();
            }
            else if(type=="color") {
                auto& strcolor = prop["value"].get<std::string>();
                auto color = engine::ColorUtils::from_string(strcolor);
                properties[name] = engine::ColorUtils::to_uint32(color);
            }
            else if(type=="file") {
                auto& strfile = prop["value"].get<std::string>();
                properties[name] = strfile;
            }
            else if(type=="object")
            {
                properties[name] = prop["value"].get<int>;
            }
            else {
                spdlog::warn("unknown property type: {}", type);
            }
        }
    }
}

bool MapObject::load(const json& json)
{
    id = json["id"].get<int>();
    gid = json["gid"].get<int>();
    name = json["name"].get<std::string>();
    type = json["type"].get<std::string>();
    visible = json["visible"].get<bool>();
    rotation = json["rotation"].get<int>();

    pos.x = json["x"].get<int>();
    pos.y = json["y"].get<int>();

    size.x = json["width"].get<int>();
    size.y = json["height"].get<int>();

    LoaderUtils::loadProperties(json, _properties);

    return true;
}

bool MapLayer::load(const json& json) 
{
    id = json["id"].get<int>();
    name = json["name"].get<std::string>();
    visible = json["visible"].get<bool>();
    opacity = json["opacity"].get<int>();
    pos.x = json["x"].get<int>();
    pos.y = json["y"].get<int>();

    LoaderUtils::loadProperties(json, _properties);
    return true;
}

bool ImageLayer::load(const json& json) 
{
    if(!MapLayer::load(json) return false;

    image_file = json["image"].get<std::string>();

    offset.x = json["offsetx"].get<int>();
    offset.y = json["offsety"].get<int>();

    image_size.x = json["imagewidth"].get<int>();
    image_size.y = json["imageheight"].get<int>();

    repeat_x = json["repeatx"].get<int>();
    repeat_y = json["repeaty"].get<int>();
    
    return true;
}

bool TileLayer::load(const json& json) 
{
    if(!MapLayer::load(json) return false;

    width = json["width"].get<int>();
    height = json["height"].get<int>();

    encodeing = json["encoding"].get<std::string>();
    compression = json["compression"].get<std::string>();

    if(encodeing=="csv" || encodeing.empty())
    {
        data = json["data"].get<std::vector<int>>();
    }
    else 
    {
        spdlog::error("tile layer encoding: {} NOT support yet.", encodeing);
    }

    return true;
}

bool ObjectLayer::load(const json& json) 
{
    if(!MapLayer::load(json) return false;

    draw_order = json["draworder"].get<std::string>();

    if(json.contains("objects"))
    {
        auto& objects = json["objects"];
        for (auto& obj : objects) 
        {
            MapObject map_obj;
            if(map_obj.load(obj))
            {
                objects.push_back(map_obj);
            }
        }
    }

    return true;
}

bool GroupLayer::load(const json& json) 
{
    if(!MapLayer::load(json) return false;
    return true;
}

}