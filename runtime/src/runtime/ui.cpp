#include "ui.hpp"

#include "shared/debug/debug_manager.hpp"
#include "runtime/world.hpp"
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/cwind.hpp"
#include "shared/scene/scene.hpp"
#include "microprofile/microprofile.h"

#include <Components/BsCCamera.h>
#include <GUI/BsCGUIWidget.h>
#include <GUI/BsGUIButton.h>
#include <GUI/BsGUIInputBox.h>
#include <GUI/BsGUILabel.h>
#include <GUI/BsGUILayoutX.h>
#include <GUI/BsGUILayoutY.h>
#include <GUI/BsGUIPanel.h>
#include <GUI/BsGUISlider.h>
#include <GUI/BsGUITexture.h>
#include <GUI/BsGUIToggle.h>
#include <Image/BsSpriteTexture.h>
#include <Importer/BsImporter.h>
#include <Scene/BsSceneManager.h>
#include <regex>

namespace wind {

struct NetDebugInfo::Data {
  bs::GUILabel *hCQL{};
  bs::GUILabel *hCQR{};
  bs::GUILabel *hBSCE{};
  bs::GUILabel *hping{};
  bs::GUILabel *houtBytes{};
  bs::GUILabel *houtPackets{};
  bs::GUILabel *hinBytes{};
  bs::GUILabel *hinPackets{};
  bs::GUILabel *hqueueTime{};

  bs::String CQL{};
  bs::String CQR{};
  bs::String BSCE{};
  bs::String ping{};
  bs::String outBytes{};
  bs::String outPackets{};
  bs::String inBytes{};
  bs::String inPackets{};
  bs::String queueTime{};
};

NetDebugInfo::NetDebugInfo()
    : m_data(new Data()) {}

NetDebugInfo::~NetDebugInfo() {
  delete m_data;
}

void NetDebugInfo::setup(bs::GUILayoutY *layout) {
  m_data->hCQL = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->CQL));
  m_data->hCQR = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->CQR));
  m_data->hBSCE = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->BSCE));
  m_data->hping = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->ping));
  m_data->houtBytes = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->outBytes));
  m_data->houtPackets = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->outPackets));
  m_data->hinBytes = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->inBytes));
  m_data->hinPackets = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->inPackets));
  m_data->hqueueTime = layout->addNewElement<bs::GUILabel>(bs::HString(m_data->queueTime));
}

void NetDebugInfo::update(const Client &client) {
  MICROPROFILE_SCOPEI("netdebuginfo", "update", MP_GREEN3);
  auto status = client.GetConnectionStatus();
  if (!status) {
    return;
  }

  m_data->CQL = fmt::format("CQL {:.2f}", status->m_flConnectionQualityLocal);
  m_data->CQR = fmt::format("CQR {:.2f}", status->m_flConnectionQualityRemote);
  m_data->BSCE =
      fmt::format("b/s cap estimation {}", status->m_nSendRateBytesPerSecond);
  m_data->ping = fmt::format("ping {}", status->m_nPing);
  m_data->outBytes = fmt::format("out b/s {:.2f}", status->m_flOutBytesPerSec);
  m_data->outPackets = fmt::format("out p/s {:.2f}", status->m_flOutPacketsPerSec);
  m_data->inBytes = fmt::format("in  b/s {}", status->m_flInBytesPerSec);
  m_data->inPackets = fmt::format("in  p/s {}", status->m_flInPacketsPerSec);
  m_data->queueTime = fmt::format("q time {}us", status->m_usecQueueTime);

  m_data->hCQL->setContent(bs::HString(m_data->CQL));
  m_data->hCQR->setContent(bs::HString(m_data->CQR));
  m_data->hBSCE->setContent(bs::HString(m_data->BSCE));
  m_data->hping->setContent(bs::HString(m_data->ping));
  m_data->houtBytes->setContent(bs::HString(m_data->outBytes));
  m_data->houtPackets->setContent(bs::HString(m_data->outPackets));
  m_data->hinBytes->setContent(bs::HString(m_data->inBytes));
  m_data->hinPackets->setContent(bs::HString(m_data->inPackets));
  m_data->hqueueTime->setContent(bs::HString(m_data->queueTime));
}

// ============================================================ //

constexpr u32 kAimDiameter = 8;

void Ui::setup(World *world, bs::HSceneObject camera, u32 width, u32 height) {
  using namespace bs;

  auto cameraComp = camera->getComponent<CCamera>();

  auto gui = SceneObject::create("GUI");
  HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
  GUIPanel *mainPanel = guiComp->getPanel();
  GUILayoutY *layout = mainPanel->addNewElement<GUILayoutY>();
  layout->setSize(300, 700);
  layout->addNewElement<GUILabel>(HString{u8"Press the Escape key to quit"});

  { // Start Server
    GUILayoutX *l = layout->addNewElement<GUILayoutX>();
    l->addNewElement<GUILabel>(HString{u8"port"});
    GUIInputBox *input = l->addNewElement<GUIInputBox>();
    input->setText("4040");
    input->setFilter([](const String &str) {
      return std::regex_match(str, std::regex("-?(\\d+)?"));
    });

    GUIButton *startServerBtn =
        l->addNewElement<GUIButton>(GUIContent{HString{"start server"}});
    startServerBtn->onClick.connect([input, world] {
      if (!world->serverIsActive()) {
        logVerbose("start server on {}", input->getText().c_str());
        world->getServer().StartServer(std::atoi(input->getText().c_str()));
      }
    });
  }

  { // Client network
    GUILayoutX *l = layout->addNewElement<GUILayoutX>();
    l->addNewElement<GUILabel>(HString{u8"ip:port"});
    GUIInputBox *input = l->addNewElement<GUIInputBox>();
    input->setText("127.0.0.1:4040");

    GUIButton *btn =
        l->addNewElement<GUIButton>(GUIContent{HString{"connect"}});
    btn->onClick.connect([input, world] {
      if (!world->getMyPlayer()->isConnected()) {
        logVerbose("connect to {}", input->getText().c_str());
        auto &t = input->getText();
        if (t.find(":") != std::string::npos && t.size() > 8) {
          world->getMyPlayer()->connect(input->getText().c_str());
        }
      }
    });
    GUIButton *btn2 = l->addNewElement<GUIButton>(GUIContent{HString{"d/c"}});
    btn2->onClick.connect([input, world] {
        if (world->getMyPlayer()->isConnected()) {
          world->getMyPlayer()->disconnect();
      }
    });
  }

  { // network info
    m_netDebugInfo.setup(layout);
  }

  { // rotor slider
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"rotorSpeed"});
    text->setWidth(70);
    auto slider = l->addNewElement<GUISliderHorz>();
    slider->onChanged.connect([](f32 percent) {
      auto rotors = gSceneManager().findComponents<CRotor>(true);
      for (auto &rotor : rotors) {
        rotor->setRotation(bs::Vector3{0.0f, percent * percent * 1000, 0.0f});
      }
    });
  }

  { // shootForce slider
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"shootForce"});
    text->setWidth(70);
    m_shootForce = l->addNewElement<GUISliderHorz>();
    m_shootForce->setPercent(0.3f);
    m_shootForce->onChanged.connect(
        [world](f32 percent) {
          world->getMyPlayer()->setShootForce(100.0f * percent);
        });
  }

  // Toggle wind volume arrows
  {
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"Wind Volume Arrows"});
    text->setWidth(105);
    auto toggle = l->addNewElement<GUIToggle>(HString());
    toggle->toggleOn();
    DebugManager::setBool("WVArrows", true);
    toggle->onToggled.connect([](bool value) {
        DebugManager::setBool("WVArrows", value);
      });
  }

  { // aim dot
    HTexture dotTex = gImporter().import<Texture>("res/textures/dot.png");
    HSpriteTexture dotSprite = SpriteTexture::create(dotTex);
    m_aim = GUITexture::create(dotSprite);
    m_aim->setSize(kAimDiameter, kAimDiameter);
    updateAimPosition(width, height);
    mainPanel->addElement(m_aim);
  }
}

void Ui::updateAimPosition(u32 width, u32 height) {
  m_aim->setPosition(width / 2 - kAimDiameter / 2,
                     height / 2 - kAimDiameter / 2);
}

f32 Ui::getShootForce() const { return m_shootForce->getPercent(); }

void Ui::setShootForce(f32 percent) { m_shootForce->setPercent(percent); }

}
