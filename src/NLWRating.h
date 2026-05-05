#pragma once
#include <matjson.hpp>

enum class NLWRatingType {
    Regular,
    Platformer,
    Pending,
};

class NLWRating {
public:
    explicit NLWRating(const matjson::Value& levelData);

    NLWRating() = default;

    int sheetIndex;
    NLWRatingType type;
    std::string tier;
    int id;
    std::string name;
    std::string creator;
    std::string skillset;
    float enjoyment;
    std::string description;
    std::optional<std::string> broken;

    std::string format();
};
