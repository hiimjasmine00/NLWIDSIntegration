#include "ListManager.hpp"
#include "NLWInfoPopupLayer.hpp"
#include <Geode/binding/GJDifficultySprite.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(NLWInfoLayer, LevelInfoLayer) {
    struct Fields {
        NLWRating* m_rating = nullptr;
        CCMenu* m_menu = nullptr;
        bool m_addedTier = false;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        m_fields->m_menu = CCMenu::create();
        m_fields->m_menu->setID("tier_menu"_spr);
        addChild(m_fields->m_menu, 10);

        updateDifficultyFace();

        return true;
    }

    void updateDifficultyFace() {
        if (!m_fields->m_menu) return;

        auto rating = ListManager::getRating(m_level);
        if (!rating) return;

        m_fields->m_rating = rating;

        m_fields->m_menu->setPosition(m_difficultySprite->getPosition() + CCPoint { 0.0f, 35.0f });

        if (m_fields->m_addedTier) return;

        const char* tierStr;
        switch (rating->type) {
            case NLWRatingType::Regular:
            case NLWRatingType::Platformer:
                tierStr = rating->tier.c_str();
                break;
            case NLWRatingType::Pending:
                tierStr = "Pending";
                break;
            default:
                tierStr = "";
        }
        auto label = CCLabelBMFont::create(tierStr, "bigFont.fnt");
        label->setScale(0.5f);
        label->setColor(rating->type == NLWRatingType::Pending ? ccColor3B { 255, 255, 255 } : ListManager::getTierColor(rating->tier));
        auto tier = CCMenuItemSpriteExtra::create(label, this, menu_selector(NLWInfoLayer::openNLWInfoPane));
        tier->setID("tier_label"_spr);
        m_fields->m_menu->addChild(tier);

        m_fields->m_addedTier = true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        updateDifficultyFace();
    }

    void openNLWInfoPane(CCObject* sender) {
        NLWInfoPopupLayer::create(m_fields->m_rating)->show();
    }
};
