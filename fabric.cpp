

#include "fabric.h"
#include <map>
#include <functional>

static std::map<std::string, std::function<std::shared_ptr<BaseFilter>()>> constructors{
    {"-crop", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<Crop>(); }},
    {"-gs", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<Grayscale>(); }},
    {"-neg", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<Negative>(); }},
    {"-sharp", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<Sharpening>(); }},
    {"-edge", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<EdgeDetection>(); }},
    {"-blur", []() -> std::shared_ptr<BaseFilter> { return std::make_shared<GaussianBlur>(); }},
};

std::shared_ptr<BaseFilter> filter_fabric(std::string filter_arg) {

    if (constructors.find(filter_arg) == constructors.end()) {
        throw std::invalid_argument("Unknown filter: " + filter_arg);
    }

    return constructors[filter_arg]();
}
