#include <Geode/utils/web.hpp>

enum class NLWRatingType {
    Regular,
    Platformer,
    Pending,
};

struct NLWRating {
    std::string tier;
    std::string name;
    std::string creator;
    std::string skillset;
    std::string description;
    std::optional<std::string> broken;
    NLWRatingType type;
    float enjoyment;
    int id;
    int sheetIndex;
};

class ListManager {
public:
    static std::vector<NLWRating> ratings;
    static bool fetchedRatings;
    static bool erroredRatings;

    static void init();
    static NLWRating* getRating(GJGameLevel* level);
    static cocos2d::ccColor3B getTierColor(std::string_view tier);
    static cocos2d::ccColor3B getEnjoymentColor(float enjoyment);
    static std::string getRatingLink(NLWRating* rating);
    static GJSearchObject* getSearchObject(std::string_view tier);
};
