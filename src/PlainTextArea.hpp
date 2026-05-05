#include <Geode/binding/FLAlertLayerProtocol.hpp>
#include <Geode/ui/TextRenderer.hpp>
#include <Geode/utils/cocos.hpp>

class PlainTextArea : public cocos2d::CCLayer, public cocos2d::CCLabelProtocol, public FLAlertLayerProtocol {
protected:
    std::string m_text;
    cocos2d::CCSize m_size;
    cocos2d::extension::CCScale9Sprite* m_bgSprite = nullptr;
    cocos2d::CCMenu* m_content = nullptr;
    CCScrollLayerExt* m_scrollLayer = nullptr;
    geode::Ref<geode::TextRenderer> m_renderer;

    bool init(std::string str, const cocos2d::CCSize& size);
public:
    static PlainTextArea* create(std::string str, const cocos2d::CCSize& size);

    void updateLabel();

    void setString(char const* text) override;
    char const* getString() override;

    CCScrollLayerExt* getScrollLayer() const;
};
