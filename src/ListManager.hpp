#include "NLWRating.hpp"
#include <Geode/utils/web.hpp>

class ListManager {
    static std::vector<NLWRating> ratings;
    static void parseResponse(const matjson::Value& data);
    static geode::async::TaskHolder<geode::utils::web::WebResponse> fetchListListener;
public:
    static bool fetchedRatings;
    static bool erroredRatings;

    static void init();
    static void throwError(std::string_view message);
    static NLWRating* getRating(GJGameLevel* level);
    static cocos2d::ccColor3B getTierColor(std::string_view tier);
    static cocos2d::ccColor3B getEnjoymentColor(float enjoyment);
    static std::string getRatingLink(const NLWRating& rating);
    static GJSearchObject* getSearchObject(std::string_view tier);
};
