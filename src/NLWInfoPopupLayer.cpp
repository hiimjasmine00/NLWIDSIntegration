#include "NLWInfoPopupLayer.hpp"
#include "ListManager.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/TextArea.hpp>

using namespace geode::prelude;

bool NLWInfoPopupLayer::init(NLWRating* rating) {
    if (!Popup::init({ 420.0f, 190.0f })) return false;

    m_rating = rating;

    auto name = CCLabelBMFont::create(rating->name.c_str(), "bigFont.fnt");
    name->setPosition({ 15.0f, 165.0f });
    name->setScale(0.75f);
    name->setAnchorPoint({ 0.0f, 0.5f });
    name->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    name->limitLabelWidth(170.0f, 0.75f, 0.1f);
    m_mainLayer->addChild(name);

    auto creator = CCLabelBMFont::create(fmt::format("by {}", rating->creator).c_str(), "bigFont.fnt");
    creator->setPosition({ 15.0f, 139.0f });
    creator->setScale(0.75f);
    creator->setAnchorPoint({ 0.0f, 0.5f });
    creator->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    creator->limitLabelWidth(195.0f, 0.75f, 0.1f);
    m_mainLayer->addChild(creator);

    auto tierMenu = CCMenu::create();
    tierMenu->setPosition({ 310.0f, 165.0f });
    m_mainLayer->addChild(tierMenu);

    auto tier = CCLabelBMFont::create(
        (rating->type == NLWRatingType::Pending ? rating->tier : fmt::format("{} Tier", rating->tier)).c_str(), "bigFont.fnt"
    );
    tier->setScale(0.75f);
    tier->setColor(ListManager::getTierColor(rating->tier));
    tier->limitLabelWidth(180.0f, 0.75f, 0.1f);
    auto tierMenuItem = CCMenuItemSpriteExtra::create(tier, this, menu_selector(NLWInfoPopupLayer::openTierLevels));
    tierMenu->addChild(tierMenuItem);

    auto skillset = CCLabelBMFont::create(rating->skillset.c_str(), "bigFont.fnt");
    skillset->setPosition({ 310.0f, 139.0f });
    skillset->setScale(0.5f);
    skillset->limitLabelWidth(180.0f, 0.5f, 0.1f);
    m_mainLayer->addChild(skillset);

    auto background = CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    background->setPosition({ 210.0f, 80.0f });
    background->setContentSize({ 400.0f, 70.0f });
    background->setColor({ 0, 0, 0 });
    background->setOpacity(75);
    m_mainLayer->addChild(background);

    auto description = SimpleTextArea::create(rating->description, "chatFont.fnt", 1.0f, 380.0f);
    description->setPosition({ 210.0f, 80.0f });
    m_mainLayer->addChild(description);

    auto openBtnSpr = ButtonSprite::create("Sheet", "goldFont.fnt", "GJ_button_01.png", 0.8f);
    openBtnSpr->setScale(0.8f);

    auto openBtn = CCMenuItemSpriteExtra::create(openBtnSpr, this, menu_selector(NLWInfoPopupLayer::onOpen));
    openBtn->setPosition({ 210.0f, 25.0f });
    m_buttonMenu->addChild(openBtn);

    auto enjSprite = CCSprite::create("NLW_button_white.png"_spr);
    enjSprite->setColor(ListManager::getEnjoymentColor(rating->enjoyment));
    enjSprite->setPosition(name->getPosition() + CCPoint { 10.0f, 0.0f });
    enjSprite->setScale(0.5f);
    m_mainLayer->addChild(enjSprite);

    auto enj = CCLabelBMFont::create(fmt::format("{:.0f}", rating->enjoyment).c_str(), "bigFont.fnt");
    enj->setPosition(enjSprite->getContentSize() / 2.0f);
    enj->setScale(0.83f);
    enjSprite->addChild(enj);

    if (rating->enjoyment == -1.0f) {
        enjSprite->setVisible(false);
        enj->setVisible(false);
    }
    else name->setPosition(name->getPosition() + CCPoint { 25.0f, 0.0f });

    auto broken = CCLabelBMFont::create(fmt::format("Broken in 2.2: {}", rating->broken.value_or("unknown")).c_str(), "goldFont.fnt");
    broken->setPosition({ 15.0f, 25.0f });
    broken->setAnchorPoint({ 0.0f, 0.5f });
    broken->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    broken->limitLabelWidth(150.0f, 0.5f, 0.1f);
    m_mainLayer->addChild(broken);

    return true;
}

void NLWInfoPopupLayer::onOpen(CCObject* sender) {
    if (!m_rating) return;

    auto insane = m_rating->insane;

    int sheetID;
    switch (m_rating->type) {
        case NLWRatingType::Platformer:
            sheetID = insane ? 506524049 : 339121001;
            break;
        case NLWRatingType::Pending:
            sheetID = 1134134033;
            break;
        default:
            sheetID = insane ? 1309758655 : 0;
            break;
    }

    auto rowID = m_rating->sheetIndex + 1;
    web::openLinkInBrowser(fmt::format(
        "https://docs.google.com/spreadsheets/d/{}/edit#gid={}&range={}:{}",
        insane ? "15ehtAIpCR8s04qIb8zij9sTpUdGJbmAE_LDcfVA3tcU" : "1YxUE2kkvhT2E6AjnkvTf-o8iu_shSLbuFkEFcZOvieA",
        sheetID, rowID, rowID
    ));
}

void NLWInfoPopupLayer::openTierLevels(CCObject* sender) {
    if (!m_rating) return;

    StringBuffer download;
    auto first = true;
    auto& tier = m_rating->tier;
    auto insane = m_rating->insane;
    for (auto& rating : ListManager::ratings) {
        if (rating.tier != tier || rating.insane != insane) continue;

        if (!first) download.append(',');
        download.append("{}", rating.id);
        first = false;
    }

    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5f,
        LevelBrowserLayer::scene(GJSearchObject::create(SearchType::Type19, download.str()))));
}

NLWInfoPopupLayer* NLWInfoPopupLayer::create(NLWRating* rating) {
    auto ret = new NLWInfoPopupLayer();
    if (ret->init(rating)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
