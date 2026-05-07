#include <ccTypes.h>

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
    std::string broken;
    NLWRatingType type;
    int enjoyment;
    int id;
    int sheetIndex;
    bool insane;
};

class NLWIntegration {
public:
    static std::vector<NLWRating> ratings;

    static cocos2d::ccColor3B getTierColor(std::string_view tier);
};
