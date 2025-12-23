#include "map_layer.h"

namespace game {
    
void LoaderUtils::loadProperties(const json& json, Properties& properties)
{
    if(json.contains("properties")) 
    {
        auto& props = json["properties"];
        for (auto& prop : props) 
        {
            const auto& name = prop.value("name", "");
            const auto& type = prop.value("type", "");

            if(type=="bool") {
                properties[name] = prop.value("value", false);
            }
            else if(type=="int") {
                properties[name] = prop.value("value", 0);
            }
            else if(type=="float") {
                properties[name] = prop.value("value", 0.0f);
            }
            else if(type=="string") {
                properties[name] = prop.value("value", "");
            }
            else if(type=="color") {
                auto strcolor = prop.value("value", "");
                engine::Color color; color.fromHexString(strcolor);
                properties[name] = (uint32_t)color;
            }
            else if(type=="file") {
                const auto& strfile = prop.value("value", "");
                properties[name] = strfile;
            }
            else if(type=="object")
            {
                properties[name] = prop.value("value", 0);
            }
            else {
                spdlog::warn("unknown property type: {}", type);
            }
        }
    }
}

bool MapObject::load(const json& json)
{
    id = json.value("id", 0);
    gid = json.value("gid", 0);
    name = json.value("name", "");
    type = json.value("type", "");
    visible = json.value("visible", true);
    rotation = json.value("rotation", 0);

    pos.x = json.value("x", 0);
    pos.y = json.value("y", 0);

    size.x = json.value("width", 0);
    size.y = json.value("height", 0);

    LoaderUtils::loadProperties(json, properties);

    return true;
}

bool MapLayer::load(const json& json) 
{
    id = json.value("id", 0);
    name = json.value("name", "");

    visible = json.value("visible", true);
    opacity = json.value("opacity", 255);

    pos.x = json.value("x", 0);
    pos.y = json.value("y", 0);

    tint_color.fromHexString(json.value("tintcolor", "#ffffffff"));

    auto stype = json.value("type", "");
    if(stype=="tilelayer") {
        type = MapLayerType::TileLayer;
    }
    else if(stype=="objectgroup") {
        type = MapLayerType::ObjectLayer;
    }
    else if(stype=="imagelayer") {
        type = MapLayerType::ImageLayer;
    }
    else if(stype=="group") {
        type = MapLayerType::GroupLayer;
    }
    else  {
        spdlog::error("unknown layer type: {}", stype);
        return false;
    }   

    LoaderUtils::loadProperties(json, properties);
    return true;
}

bool ImageLayer::load(const json& json) 
{
    if(!MapLayer::load(json)) return false;

    image_file = json.value("image", "");

    offset.x = json.value("offsetx", 0);
    offset.y = json.value("offsety", 0);

    image_size.x = json.value("imagewidth", 0);
    image_size.y = json.value("imageheight", 0);

    repeat_x = json.value("repeatx", 0);
    repeat_y = json.value("repeaty", 0);
    
    return true;
}

bool TileLayer::load(const json& json) 
{
    if(!MapLayer::load(json)) return false;

    width = json.value("width", 0);
    height = json.value("height", 0);

    encodeing = json.value("encoding", "");
    compression = json.value("compression", "");

    if(encodeing=="csv" || encodeing.empty())
    {
        data = json.value("data", std::vector<int>{});
    }
    else 
    {
        spdlog::error("tile layer encoding: {} NOT support yet.", encodeing);
    }

    return true;
}

bool ObjectLayer::load(const json& json) 
{
    if(!MapLayer::load(json)) return false;

    draw_order = json.value("draworder", "");

    if(json.contains("objects"))
    {
        auto& objs = json["objects"];
        for (auto& obj : objs) 
        {
            MapObject map_obj;
            if(map_obj.load(obj))
            {
                objects.insert({map_obj.id, map_obj});
            }
        }
    }

    return true;
}

bool GroupLayer::load(const json& json) 
{
    if(!MapLayer::load(json))
        return false;
    return true;
}

}