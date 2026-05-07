#include <Geode/ui/Popup.hpp>

struct NLWRating;

class NLWInfoPopupLayer : public geode::Popup {
protected:
    NLWRating* m_rating;

    bool init(NLWRating* rating);
    void onOpen(cocos2d::CCObject* sender);
    void openTierLevels(cocos2d::CCObject* sender);
public:
    static NLWInfoPopupLayer* create(NLWRating* rating);
};
