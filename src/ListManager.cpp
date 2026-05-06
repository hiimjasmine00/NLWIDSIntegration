#include "ListManager.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/loader/GameEvent.hpp>

using namespace geode::prelude;

std::vector<NLWRating> ListManager::ratings;

void loadList(std::string url, bool insane) {
    spawn(
        web::WebRequest().get(std::move(url)),
        [insane](web::WebResponse res) {
            auto listName = insane ? "IDS" : "NLW";

            if (!res.ok()) return log::error("Failed to fetch {} ratings: {}", listName, res.string().unwrapOrDefault());

            auto json = res.json();
            if (json.isErr()) return log::error("Failed to parse {} ratings JSON: {}", listName, json.unwrapErr());

            auto data = std::move(json).unwrap().asArray();
            if (!data.isOk()) return log::error("{} ratings JSON is not an array", listName);

            for (auto& level : data.unwrap()) {
                auto& entry = ListManager::ratings.emplace_back();
                if (auto sheetIndex = level["sheetIndex"].asInt()) {
                    entry.sheetIndex = sheetIndex.unwrap();
                }
                if (auto type = level["type"].asString()) {
                    auto typeStr = std::move(type).unwrap();
                    if (typeStr == "platformer") entry.type = NLWRatingType::Platformer;
                    else if (typeStr == "pending") entry.type = NLWRatingType::Pending;
                    else entry.type = NLWRatingType::Regular;
                }
                if (auto tier = level["tier"].asString()) {
                    entry.tier = tier.unwrap();
                }
                if (auto id = level["id"].asInt()) {
                    entry.id = id.unwrap();
                }
                if (auto name = level["name"].asString()) {
                    entry.name = name.unwrap();
                }
                if (auto creator = level["creator"].asString()) {
                    entry.creator = creator.unwrap();
                }
                if (auto skillset = level["skillset"].asString()) {
                    entry.skillset = skillset.unwrap();
                }
                if (auto enjoyment = level["enjoyment"].asDouble()) {
                    entry.enjoyment = enjoyment.unwrap();
                }
                if (auto description = level["description"].asString()) {
                    entry.description = description.unwrap();
                }
                if (auto broken = level["broken"].asString()) {
                    entry.broken = broken.unwrap();
                }
                entry.insane = insane;
            }

            log::info("Successfully fetched {} ratings", listName);
        }
    );
}

$on_game(Loaded) {
    loadList("https://nlw.oat.zone/list?type=all", false);
    loadList("https://nlw.oat.zone/ids?type=all", true);
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
