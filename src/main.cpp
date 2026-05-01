#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// --- SETTINGS STORAGE ---
bool g_noclip = false;
bool g_jumpHack = false;
bool g_safeMode = true;
bool g_forcePlatformer = false;

// --- GAMEPLAY HOOKS ---

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (g_forcePlatformer) {
            level->m_isPlatformer = true;
        }
        return PlayLayer::init(level, useReplay, dontSave);
    }

    void destroyPlayer(PlayerObject* p, GameObject* obj) {
        if (g_noclip) return; 
        PlayLayer::destroyPlayer(p, obj);
    }

    void levelComplete() {
        if (g_safeMode && (g_noclip || g_jumpHack || g_forcePlatformer)) {
            FLAlertLayer::create("Safe Mode", "Progress not saved due to active hacks.", "OK")->show();
            return;
        }
        PlayLayer::levelComplete();
    }
};

class $modify(PlayerObject) {
    void pushButton(PlayerButton btn) {
        if (g_jumpHack && btn == PlayerButton::Jump) {
            this->m_isRocket = true; 
            PlayerObject::pushButton(btn);
            this->m_isRocket = false; 
            return;
        }
        PlayerObject::pushButton(btn);
    }
};

// --- GUI IMPLEMENTATION ---

class AethexMenu : public FLAlertLayer {
    CCLayer* m_globalTab;
    CCLayer* m_hacksTab;

public:
    static AethexMenu* create() {
        auto ret = new AethexMenu();
        if (ret && ret->init(300, 240, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool setup() override {
        auto winSize = CCDirector::get()->getWinSize();

        // --- Custom Header: Aethex GUI ---
        auto titleLabel = CCLabelBMFont::create("AETHEX GUI", "goldFont.fnt");
        titleLabel->setScale(0.85f);
        titleLabel->setPosition({150, 215}); 
        m_mainLayer->addChild(titleLabel);

        // --- Tab Switcher ---
        auto tabMenu = CCMenu::create();
        tabMenu->setPosition({winSize.width / 2, winSize.height / 2 + 55});

        auto globalBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Global", "bigFont.fnt", "GJ_button_05.png", 0.5f),
            this, menu_selector(AethexMenu::onShowGlobal)
        );
        auto hacksBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Hacks", "bigFont.fnt", "GJ_button_05.png", 0.5f),
            this, menu_selector(AethexMenu::onShowHacks)
        );

        tabMenu->addChild(globalBtn);
        tabMenu->addChild(hacksBtn);
        tabMenu->alignItemsHorizontallyWithPadding(15);
        m_mainLayer->addChild(tabMenu);

        m_globalTab = CCLayer::create();
        m_hacksTab = CCLayer::create();
        m_mainLayer->addChild(m_globalTab);
        m_mainLayer->addChild(m_hacksTab);

        createGlobalContent();
        createHacksContent();

        onShowGlobal(nullptr);
        return true;
    }

    void createGlobalContent() {
        auto menu = CCMenu::create();
        menu->setPosition({150, 90});

        auto label = CCLabelBMFont::create("Safe Mode", "bigFont.fnt");
        label->setScale(0.5f);
        label->setPosition({0, 30});

        auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(AethexMenu::toggleSafeMode), 0.8f);
        toggle->toggle(g_safeMode);

        menu->addChild(label);
        menu->addChild(toggle);
        m_globalTab->addChild(menu);
    }

    void createHacksContent() {
        auto menu = CCMenu::create();
        menu->setPosition({150, 60});

        // Noclip
        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.45f);
        noclipLabel->setPosition({-75, 70});
        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(AethexMenu::toggleNoclip), 0.7f);
        noclipToggle->setPosition({-75, 40});
        noclipToggle->toggle(g_noclip);

        // Air Jump
        auto jumpLabel = CCLabelBMFont::create("Air Jump", "bigFont.fnt");
        jumpLabel->setScale(0.45f);
        jumpLabel->setPosition({75, 70});
        auto jumpToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(AethexMenu::toggleJump), 0.7f);
        jumpToggle->setPosition({75, 40});
        jumpToggle->toggle(g_jumpHack);

        // Force Platformer
        auto platLabel = CCLabelBMFont::create("Force Plat", "bigFont.fnt");
        platLabel->setScale(0.45f);
        platLabel->setPosition({0, 0});
        auto platToggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(AethexMenu::togglePlatformer), 0.7f);
        platToggle->setPosition({0, -30});
        platToggle->toggle(g_forcePlatformer);

        menu->addChild(noclipLabel);
        menu->addChild(noclipToggle);
        menu->addChild(jumpLabel);
        menu->addChild(jumpToggle);
        menu->addChild(platLabel);
        menu->addChild(platToggle);
        m_hacksTab->addChild(menu);
    }

    void onShowGlobal(CCObject*) { m_globalTab->setVisible(true); m_hacksTab->setVisible(false); }
    void onShowHacks(CCObject*) { m_globalTab->setVisible(false); m_hacksTab->setVisible(true); }

    void toggleSafeMode(CCObject*) { g_safeMode = !g_safeMode; }
    void toggleNoclip(CCObject*) { g_noclip = !g_noclip; }
    void toggleJump(CCObject*) { g_jumpHack = !g_jumpHack; }
    void togglePlatformer(CCObject*) { g_forcePlatformer = !g_forcePlatformer; }
};

// --- MAIN MENU BUTTON ---

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        auto menu = this->getChildByID("bottom-menu");
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        auto btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MenuLayer::openAethexMenu));
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }
    void openAethexMenu(CCObject*) { AethexMenu::create()->show(); }
};
