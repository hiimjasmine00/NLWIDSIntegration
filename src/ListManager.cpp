#include "ListManager.hpp"
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

bool ListManager::fetchedRatings = false;
bool ListManager::erroredRatings = false;
std::vector<NLWRating> ListManager::ratings;

void ListManager::init() {
    if (fetchedRatings) return;

    spawn(
        web::WebRequest().get("https://nlw.oat.zone/list?type=all"),
        [](web::WebResponse res) {
            ratings.clear();

            fetchedRatings = true;

            if (!res.ok()) {
                erroredRatings = true;
                return log::error("Failed to fetch NLW ratings: {}", res.string().unwrapOrDefault());
            }

            auto json = res.json();
            if (json.isErr()) {
                erroredRatings = true;
                return log::error("Failed to parse NLW ratings JSON: {}", json.unwrapErr());
            }

            auto data = std::move(json).unwrap().asArray();
            if (!data.isOk()) {
                erroredRatings = true;
                return log::error("NLW ratings JSON is not an array");
            }

            for (auto& level : data.unwrap()) {
                auto& entry = ratings.emplace_back();
                entry.sheetIndex = level["sheetIndex"].asInt().unwrapOrDefault();
                auto type = level["type"].asString().unwrapOrDefault();
                if (type == "platformer") entry.type = NLWRatingType::Platformer;
                else if (type == "pending") entry.type = NLWRatingType::Pending;
                else entry.type = NLWRatingType::Regular;
                entry.tier = level["tier"].asString().unwrapOrDefault();
                entry.id = level["id"].asInt().unwrapOr(-1);
                entry.name = level["name"].asString().unwrapOr("?");
                entry.creator = level["creator"].asString().unwrapOr("?");
                entry.skillset = level["skillset"].asString().unwrapOrDefault();
                entry.enjoyment = level["enjoyment"].asDouble().unwrapOr(-1.0f);
                entry.description = level["description"].asString().unwrapOrDefault();
                if (level["broken"].isString()) {
                    entry.broken = level["broken"].asString().unwrap();
                }
            }

            log::info("Successfully fetched {} NLW ratings", ratings.size());
        }
    );
}

NLWRating* ListManager::getRating(GJGameLevel* level) {
    auto id = level->m_levelID.value();

    for (auto& rating : ratings) {
        if (id == rating.id) return &rating;
    }

    return nullptr;
}

ccColor3B ListManager::getTierColor(std::string_view tier) {
    if (tier == "Fuck") return { 0, 0, 0 };
    if (tier == "Beginner") return { 58, 134, 228 };
    if (tier == "Easy") return { 0, 255, 254 };
    if (tier == "Medium") return { 0, 255, 55 };
    if (tier == "Hard") return { 255, 255, 63 };
    if (tier == "Very Hard") return { 255, 153, 43 };
    if (tier == "Insane") return { 255, 3, 28 };
    if (tier == "Extreme") return { 255, 12,251 };
    if (tier == "Remorseless") return { 157, 10, 250 };
    if (tier == "Relentless") return { 178, 135, 232 };
    if (tier == "Terrifying") return { 241, 158, 234 };
    if (tier == "Catastrophic") return { 234, 102, 97 };
    if (tier == "Inexorable") return { 255, 193, 131 };
    if (tier == "Super Fucking Terrifying") return { 0, 0, 0 };

    if (tier == "Low End") return { 0, 192, 237 };
    if (tier == "Low-Mid Range") return { 0, 255, 135 };
    if (tier == "Mid Range") return { 255, 204, 52 };
    if (tier == "Mid-High Range") return { 255, 5, 128 };
    if (tier == "High End") return { 167, 93, 242 };
    if (tier == "Unknown") return { 255, 255, 255 };
    if (tier == "New Rates") return { 255, 255, 255 };
    if (tier == "Potential Extremes") return { 235, 235, 235 };

    return { 255, 255, 255 };
}

ccColor3B ListManager::getEnjoymentColor(float enjoyment) {
    if (enjoyment > 90.0f) return { 0, 255, 254 };
    if (enjoyment > 75.0f) return { 179, 215, 170 };
    if (enjoyment > 60.0f) return { 255, 229, 159 };
    if (enjoyment > 40.0f) return { 254, 203, 160 };
    if (enjoyment > 0.0f) return { 240, 153, 154 };
    return { 255, 255, 255 };
}

std::string ListManager::getRatingLink(NLWRating* rating) {
    auto sheetID = 0;
    if (rating->type == NLWRatingType::Platformer) sheetID = 339121001;
    if (rating->type == NLWRatingType::Pending) sheetID = 1134134033;

    auto rowID = rating->sheetIndex + 1;

    return fmt::format(
        "https://docs.google.com/spreadsheets/d/1YxUE2kkvhT2E6AjnkvTf-o8iu_shSLbuFkEFcZOvieA/edit#gid={}&range={}:{}",
        sheetID, rowID, rowID
    );
}

GJSearchObject* ListManager::getSearchObject(std::string_view tier) {
    StringBuffer download;
    auto first = true;
    for (auto& rating : ratings) {
        if (rating.tier != tier) continue;

        if (!first) download.append(',');
        download.append("{}", rating.id);
        first = false;
    }

    return GJSearchObject::create(SearchType::Type19, download.str());
}
