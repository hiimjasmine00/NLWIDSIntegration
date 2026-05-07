#include "NLWIntegration.hpp"
#include <Geode/loader/GameEvent.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

std::vector<NLWRating> NLWIntegration::ratings;

void loadList(std::string url, bool insane) {
    spawn(
        web::WebRequest().get(std::move(url)),
        [insane](web::WebResponse res) {
            std::string_view listName = insane ? "IDS" : "NLW";

            if (!res.ok()) {
                auto data = res.data();
                return log::error("Failed to fetch {} ratings: {}", listName, std::string(data.begin(), data.end()));
            }

            auto json = res.json();
            if (json.isErr()) return log::error("Failed to parse {} ratings: {}", listName, json.unwrapErr());

            auto data = std::move(json).unwrap().asArray();
            if (!data.isOk()) return log::error("Failed to parse {} ratings: not an array", listName);

            for (auto& level : data.unwrap()) {
                auto type = NLWRatingType::Regular;
                if (auto typeRes = level.get<std::string>("type")) {
                    auto typeStr = std::move(typeRes).unwrap();
                    if (typeStr == "platformer") type = NLWRatingType::Platformer;
                    else if (typeStr == "pending") type = NLWRatingType::Pending;
                }
                NLWIntegration::ratings.emplace_back(
                    level.get<std::string>("tier").unwrapOrDefault(),
                    level.get<std::string>("name").unwrapOr("?"),
                    level.get<std::string>("creator").unwrapOr("?"),
                    level.get<std::string>("skillset").unwrapOrDefault(),
                    level.get<std::string>("description").unwrapOrDefault(),
                    level.get<std::string>("broken").unwrapOr("unknown"),
                    type,
                    level.get<int>("enjoyment").unwrapOr(-1),
                    level.get<int>("id").unwrapOr(-1),
                    level.get<int>("sheetIndex").unwrapOr(0),
                    insane
                );
            }

            log::info("Successfully fetched {} ratings", listName);
        }
    );
}

$on_game(Loaded) {
    loadList("https://nlw.oat.zone/list?type=all", false);
    loadList("https://nlw.oat.zone/ids?type=all", true);
}

ccColor3B NLWIntegration::getTierColor(std::string_view tier) {
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
