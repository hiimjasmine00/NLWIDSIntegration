#include "NLWRating.h"

NLWRating::NLWRating(const matjson::Value& levelData) {
    sheetIndex = levelData["sheetIndex"].asInt().unwrapOrDefault();
    auto levelType = levelData["type"].asString().unwrapOrDefault();
    if (levelType == "platformer") type = NLWRatingType::Platformer;
    else if (levelType == "pending") type = NLWRatingType::Pending;
    else type = NLWRatingType::Regular;
    tier = levelData["tier"].asString().unwrapOrDefault();
    id = levelData["id"].asInt().unwrapOr(-1);
    name = levelData["name"].asString().unwrapOr("?");
    creator = levelData["creator"].asString().unwrapOr("?");
    skillset = levelData["skillset"].asString().unwrapOrDefault();
    enjoyment = levelData["enjoyment"].asDouble().unwrapOr(-1.0f);
    description = levelData["description"].asString().unwrapOrDefault();
    if (levelData["broken"].isString()) {
        broken = levelData["broken"].asString().unwrap();
    }
}

std::string NLWRating::format() {
    return type == NLWRatingType::Regular || type == NLWRatingType::Platformer ? tier : type == NLWRatingType::Pending ? "Pending" : "";
}
