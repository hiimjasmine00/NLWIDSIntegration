#include "../NLWIntegration.hpp"
#include "../classes/NLWInfoPopupLayer.hpp"
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

        auto f = m_fields.self();
        f->m_menu = CCMenu::create();
        f->m_menu->setID("tier-menu"_spr);
        addChild(f->m_menu, 10);

        updateDifficultyFace();

        return true;
    }

    void updateDifficultyFace() {
        auto f = m_fields.self();
        if (!f->m_menu) return;

        auto id = m_level->m_levelID.value();
        auto it = std::ranges::find_if(NLWIntegration::ratings, [id](const NLWRating& rating) { return rating.id == id; });
        if (it == NLWIntegration::ratings.end()) return;

        auto& rating = *it;
        f->m_rating = &rating;

        f->m_menu->setPosition(m_difficultySprite->getPosition() + CCPoint { 0.0f, 35.0f });

        if (f->m_addedTier) return;

        auto type = rating.type;
        auto& tierStr = rating.tier;

        auto label = CCLabelBMFont::create(type == NLWRatingType::Pending ? "Pending" : tierStr.c_str(), "bigFont.fnt");
        label->setScale(0.5f);
        label->setColor(type == NLWRatingType::Pending ? ccColor3B { 255, 255, 255 } : NLWIntegration::getTierColor(tierStr));
        auto tier = CCMenuItemSpriteExtra::create(label, this, menu_selector(NLWInfoLayer::openNLWInfoPane));
        tier->setID("tier-label"_spr);
        f->m_menu->addChild(tier);

        f->m_addedTier = true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();
        updateDifficultyFace();
    }

    void openNLWInfoPane(CCObject* sender) {
        NLWInfoPopupLayer::create(m_fields->m_rating)->show();
    }
};
