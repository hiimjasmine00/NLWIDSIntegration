#include "ListManager.h"
#include "NLWInfoPopupLayer.h"
#include <Geode/binding/GJDifficultySprite.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(NLWInfoLayer, LevelInfoLayer) {
    struct Fields {
        NLWRating* m_rating;
        CCMenu* m_menu = nullptr;
        GJDifficultySprite* m_sprite = nullptr;
        bool m_addedTier = false;
    };

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1)) return false;

        auto menu = CCMenu::create();
        menu->setID("tier_menu"_spr);

        addChild(menu);
        m_fields->m_menu = menu;

        updateDifficultyFace();

        return true;
    }

    void updateDifficultyFace() {
        if (!ListManager::fetchedRatings || ListManager::erroredRatings) return;
        if (m_level->m_stars != 10) return;
        if (m_level->m_demonDifficulty != 6) return;

        if (!m_fields->m_menu) return;

        auto ratingOpt = ListManager::getRating(m_level);
        if (!ratingOpt) {
            log::info("demon rating for {} not found", m_level->m_levelName);
            return;
        }
        m_fields->m_rating = ratingOpt;

        if (!m_difficultySprite) {
            log::warn("demon icon not found?? ok");
            return;
        }

        if (Mod::get()->getSettingValue<bool>("use-demon-face")) {
            m_fields->m_menu->removeAllChildren();
            m_fields->m_menu->setPosition(m_difficultySprite->getPosition());

            auto sprite = GJDifficultySprite::create(m_level->m_demonDifficulty + 4, GJDifficultyName::Long);
            auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(NLWInfoLayer::openNLWInfoPane));
            m_fields->m_menu->addChild(button);

            m_difficultySprite->setVisible(false);
        }
        else {
            if (m_fields->m_addedTier) return;
            m_fields->m_menu->setPosition(m_difficultySprite->getPosition() + CCPoint { 0.0f, 35.0f });

            auto label = CCLabelBMFont::create(m_fields->m_rating->format().c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setColor(m_fields->m_rating->type == NLWRatingType::Pending
                ? ccColor3B { 255, 255, 255 } : ListManager::getTierColor(m_fields->m_rating->tier));
            auto tier = CCMenuItemSpriteExtra::create(label, this, menu_selector(NLWInfoLayer::openNLWInfoPane));
            m_fields->m_menu->addChild(tier);
        }

        m_fields->m_addedTier = true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        updateDifficultyFace();
    }

    void openNLWInfoPane(CCObject* sender) {
        NLWInfoPopupLayer::create(m_level, m_fields->m_rating)->show();
    }
};
