#include "NLWRating.h"
#include <Geode/ui/Popup.hpp>

class NLWInfoPopupLayer : public geode::Popup {
protected:
    GJGameLevel* m_level;
    NLWRating* m_rating;

    bool init(GJGameLevel* level, NLWRating* rating);
    void onOpen(cocos2d::CCObject* sender);
    void openTierLevels(cocos2d::CCObject* sender);
public:
    static NLWInfoPopupLayer* create(GJGameLevel* level, NLWRating* rating);
};
