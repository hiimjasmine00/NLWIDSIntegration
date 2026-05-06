#include <Geode/utils/web.hpp>

enum class NLWRatingType {
    Regular,
    Platformer,
    Pending,
};

struct NLWRating {
    std::string tier;
    std::string name = "?";
    std::string creator = "?";
    std::string skillset;
    std::string description;
    std::string broken = "unknown";
    NLWRatingType type = NLWRatingType::Regular;
    float enjoyment = -1.0f;
    int id = -1;
    int sheetIndex = 0;
    bool insane = false;
};

class ListManager {
public:
    static std::vector<NLWRating> ratings;

    static NLWRating* getRating(GJGameLevel* level);
    static cocos2d::ccColor3B getTierColor(std::string_view tier);
    static cocos2d::ccColor3B getEnjoymentColor(float enjoyment);
};
