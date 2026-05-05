#include "ListManager.hpp"
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

bool ListManager::fetchedRatings;
bool ListManager::erroredRatings;
std::vector<NLWRating> ListManager::ratings;
TaskHolder<web::WebResponse> ListManager::fetchListListener;

void ListManager::parseResponse(const matjson::Value& data) {
    if (!data.isArray()) {
        log::error("got unexpected data: {}", data.dump());
        ListManager::throwError("expected root element to be an array! check logs");
        return;
    }

    auto arr = data.asArray().unwrap();

    for (auto& level : arr) {
        ListManager::ratings.emplace_back(level);
    }
}

std::string getPlatformName() {
    #ifdef GEODE_IS_WINDOWS
    if (HMODULE hntdll = GetModuleHandle("ntdll.dll")) {
        using WineVersionFunc = char const* (CDECL *)(void);
        static WineVersionFunc getWineVersion = reinterpret_cast<WineVersionFunc>(GetProcAddress(hntdll, "wine_get_version"));
        if (getWineVersion) return fmt::format(GEODE_PLATFORM_NAME " (Wine/{})", getWineVersion());
    }
    #endif
    return GEODE_PLATFORM_NAME;
}

std::string getUserAgent() {
    return fmt::format(GEODE_MOD_ID "/{}; GeometryDash/" GEODE_STR(GEODE_GD_VERSION) " (GeodeSDK/{}); {}",
        Mod::get()->getVersion().toNonVString(true),
        Loader::get()->getVersion().toNonVString(true),
        getPlatformName()
    );
}

void ListManager::init() {
    if (!ListManager::fetchedRatings) {
        web::WebRequest req = web::WebRequest();

        ListManager::fetchListListener.spawn(
            web::WebRequest().userAgent(getUserAgent()).get("https://nlw.oat.zone/list?type=all"),
            [](web::WebResponse res) {
                auto json = res.json();
                if (res.ok() && json.isOk()) {
                    ListManager::fetchedRatings = true;
                    ListManager::erroredRatings = false;
                    ListManager::parseResponse(json.unwrap());
                    log::info("successfully fetched {} levels", ListManager::ratings.size());
                }
                else ListManager::throwError(fmt::format("{}: {}", res.code(), res.string().unwrapOr("No data returned")));
            }
        );
    }
}

void ListManager::throwError(std::string_view message) {
    ListManager::fetchedRatings = true;
    ListManager::erroredRatings = true;
    FLAlertLayer::create("Error", fmt::format(
        "{}\n\n"
        "<cr>Could not fetch NLW data.</c>\n"
        "The API could be down, or this is could be a temporary network failure. Restart your game to try again!",
        message
    ), "OK")->show();
    log::error("error fetching ratings: {}", message);
}

NLWRating* ListManager::getRating(GJGameLevel* level) {
    auto id = level->m_levelID.value();

    for (auto& rating : ListManager::ratings) {
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

std::string ListManager::getRatingLink(const NLWRating& rating) {
    auto sheetID = 0;
    if (rating.type == NLWRatingType::Platformer) sheetID = 339121001;
    if (rating.type == NLWRatingType::Pending) sheetID = 1134134033;

    auto rowID = rating.sheetIndex + 1;

    return fmt::format(
        "https://docs.google.com/spreadsheets/d/1YxUE2kkvhT2E6AjnkvTf-o8iu_shSLbuFkEFcZOvieA/edit#gid={}&range={}:{}", sheetID, rowID, rowID);
}

GJSearchObject* ListManager::getSearchObject(std::string_view tier) {
    StringBuffer download;
    auto first = true;
    for (auto rating : ListManager::ratings) {
        if (rating.tier != tier) continue;

        if (!first) download.append(',');
        download.append("{}", rating.id);
        first = false;
    }

    download.append("&gameVersion=22");
    return GJSearchObject::create(SearchType::Type19, download.str());
}
