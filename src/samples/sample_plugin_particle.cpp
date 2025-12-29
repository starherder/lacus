#include "sample_plugin_particle.h"

#include "engine/application.h"

#include "particle/ParticleManager.h"

namespace samples 
{
    ImFormParticles::ImFormParticles()
    {
        init();
    }

    void ImFormParticles::draw()
    {
        ImGui::Begin("particles");

        ImGui::TextColored({ 0, 0.5, 0, 1 }, "select particle:");

        static bool emitting = false;
        ImGui::SetNextWindowSize(ImVec2(-FLT_MIN, 200.0f));
        if (ImGui::ListBox("##particle_listbox", &_select_index, _particles.data(), _particles.size(), 4))
        {
            emitting = true;
            auto& sel_particle = _particles[_select_index];
            on_select_particle.emit(sel_particle);
        }

        if (!emitting) {
            if (ImGui::Button("start")) {
                emitting = true;
                on_particle_start.emit();
            }
        } else {
            if (ImGui::Button("stop")) {
                emitting = false;
                on_particle_stop.emit();
            }
        }

        ImGui::End();
    }

    void ImFormParticles::init()
    {
        auto& cfgs = particle::ParticleManager::inst().GetAllParticleConfigs();
        for (auto& [name, file] : cfgs) 
        {
            _particles.push_back(name.c_str());
        }
    }

    void SamplePluginParticle::onInit() 
    {
        particle::ParticleManager::inst().init(application());
        bool res = particle::ParticleManager::inst().LoadParticles("particles/particles.xml");
        if (!res) {
            spdlog::error("load particles failed.");
            return;
        }
    }

    void SamplePluginParticle::onInstall() 
    {  
    }

    void SamplePluginParticle::onUninstall() 
    {
    }

    void SamplePluginParticle::onEnable()
    {
        auto form = imgui::ImFormManager::inst().showForm<ImFormParticles>("particles");
        form->on_select_particle.connect(this, &SamplePluginParticle::onParticleSelect);
        form->on_particle_start.connect(this, &SamplePluginParticle::onParticleStart);
        form->on_particle_stop.connect(this, &SamplePluginParticle::onParticleStop);
    }

    void SamplePluginParticle::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("particles");
    }

    void SamplePluginParticle::onUpdate() 
    {
        if (_particle) 
        {
            _particle->Update(application()->fpsChecker().deltaSeconds());
        }
    }

    void SamplePluginParticle::onDraw() 
    {
        if (_particle) 
        {
            _particle->Draw();
        }
    }

    void SamplePluginParticle::onClose() 
    {
    }

    void SamplePluginParticle::onParticleSelect(const char* particle)
    {
        _particle = particle::ParticleManager::inst().CreateParticle(particle);
        if (!_particle)
        {
            spdlog::error("create particle ({}) failed.", particle);
            return;
        }

        _particle->SetPos({ 500, 500 });
        _particle->Start();
    }
    
    void SamplePluginParticle::onParticleStart()
    {
        if (_particle)
        {
            _particle->Start();
        }
    }
    
    void SamplePluginParticle::onParticleStop()
    {
        if (_particle)
        {
            _particle->Stop();
        }
    }
}