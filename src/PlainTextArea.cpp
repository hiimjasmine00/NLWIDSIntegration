#include "PlainTextArea.hpp"
#include <Geode/binding/CCContentLayer.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

class PlainContentLayer : public CCContentLayer {
protected:
    CCMenu* m_content;

public:
    static PlainContentLayer* create(CCMenu* content, float width, float height) {
        auto ret = new PlainContentLayer();
        if (ret->initWithColor({ 0, 255, 0, 0 }, width, height)) {
            ret->m_content = content;
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void setPosition(const CCPoint& pos) override {
        CCLayerColor::setPosition(pos);

        if (m_content) {
            for (auto child : CCArrayExt<CCNode*>(m_content->getChildren())) {
                auto y = getPositionY() + child->getPositionY();
                child->setVisible(m_content->getContentHeight() >= y && y >= -child->getContentHeight());
            }
        }
    }
};

bool PlainTextArea::init(std::string str, const CCSize& requestedSize) {
    if (!CCLayer::init()) return false;

    ignoreAnchorPointForPosition(false);
    setAnchorPoint({ 0.5f, 0.5f });

    auto visible = CCDirector::get()->getWinSize();
    m_size.width = std::min(requestedSize.width, visible.width * 0.9f);
    m_size.height = std::min(requestedSize.height, visible.height * 0.8f);

    m_text = str;
    setContentSize(m_size);

    m_renderer = TextRenderer::create();
    m_renderer->retain();

    m_bgSprite = CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
    m_bgSprite->setColor({ 0, 0, 0 });
    m_bgSprite->setOpacity(75);
    m_bgSprite->setContentSize(m_size);
    m_bgSprite->setAnchorPoint({ 0.5f, 0.5f });
    m_bgSprite->setPosition({ m_size.width / 2.0f, m_size.height / 2.0f });
    addChild(m_bgSprite);

    m_scrollLayer = ScrollLayer::create({ { 0.0f, 0.0f }, m_size }, true);
    m_scrollLayer->setTouchEnabled(true);
    addChild(m_scrollLayer);

    m_content = CCMenu::create();
    m_content->setZOrder(2);
    m_scrollLayer->m_contentLayer->addChild(m_content);

    updateLabel();
    return true;
}

class BreakLine : public CCNode {
protected:
    void draw() override {
        ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ccDrawSolidRect({ 0.0f, 0.0f }, getContentSize(), { 1.0f, 1.0f, 1.0f, 0.2f });
    }
public:
    static BreakLine* create(float width) {
        auto ret = new BreakLine();
        if (ret->init()) {
            ret->setContentSize({ width, 1.0f });
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

void PlainTextArea::updateLabel() {
    m_renderer->begin(m_content, { 0.0f, 0.0f }, m_size);

    m_renderer->pushFont([](int style) {
        return TextRenderer::Label(CCLabelBMFont::create("", "chatFont.fnt"));
    });
    m_renderer->pushScale(1.0f);
    m_renderer->pushVerticalAlign(TextAlignment::End);
    m_renderer->pushHorizontalAlign(TextAlignment::Begin);

    m_renderer->renderString(m_text);
    m_renderer->end();

    auto csz = m_content->getContentSize();
    if (csz.height > m_size.height) {
        m_scrollLayer->m_contentLayer->setContentSize(csz + CCSize { 0.0f, 12.5f });
        m_content->setPositionY(10.0f);
    }
    else {
        m_scrollLayer->m_contentLayer->setContentSize(csz);
        m_content->setPositionY(-2.5f);
    }

    m_scrollLayer->moveToTop();
}

CCScrollLayerExt* PlainTextArea::getScrollLayer() const {
    return m_scrollLayer;
}

void PlainTextArea::setString(char const* text) {
    m_text = text;
    this->updateLabel();
}

char const* PlainTextArea::getString() {
    return m_text.c_str();
}

PlainTextArea* PlainTextArea::create(std::string str, const CCSize& size) {
    auto ret = new PlainTextArea();
    if (ret->init(std::move(str), size)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
