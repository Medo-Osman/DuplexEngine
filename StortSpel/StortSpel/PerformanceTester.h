#pragma once
//Hopkok av Stefan Petersson (spr@bth.se), copyright 2003-2019 etc etc bla bla, glhf :-)
#include "3DPCH.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>

#pragma comment(lib, "dxgi.lib")

#include <psapi.h>

#include <string>
#include <iostream>
#include <string>
#include "Timer.h"

class PerformanceTester
{
private:
	Timer m_internalTimer;
	double m_sampleFrequency = 10.f / 60.f; //10 times a second

	float m_frameRate = 0;

	float checkRam()
	{
		HANDLE hProcess = GetCurrentProcess();

		PROCESS_MEMORY_COUNTERS pmc{};
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			//pmc returns page memory used in bytes, split by 1000 to get kb, 1000 again to get mb.
			return float(pmc.PagefileUsage / 1000.0 / 1000.0); //Mb
		}

	}

	float checkVram()
	{
		float vram = 0;

		IDXGIFactory* factory = nullptr;
		HRESULT ret_code = ::CreateDXGIFactory(
			__uuidof(IDXGIFactory),
			reinterpret_cast<void**>(&factory));

		if (!FAILED(ret_code))
		{
			IDXGIAdapter* dxgiAdapter = nullptr;

			if (!FAILED(factory->EnumAdapters(0, &dxgiAdapter)))
			{
				IDXGIAdapter4* dxgiAdapter4 = NULL;
				if (!FAILED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
				{
					DXGI_QUERY_VIDEO_MEMORY_INFO info;

					if (!FAILED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
					{
						vram = float(info.CurrentUsage / 1000.0 / 1000.0); //Mb
					};

					dxgiAdapter4->Release();
				}
				dxgiAdapter->Release();
			}
			factory->Release();
		}

		return vram;
	}

	PerformanceTester() {
		m_internalTimer.start();
	};

	std::vector<float> fps;

public:
	static PerformanceTester& get()
	{
		static PerformanceTester instance;
		return instance;
	}

	PerformanceTester(const PerformanceTester&) = delete;
	void operator=(PerformanceTester const&) = delete;

	void runPerformanceTestPrint()
	{
		float vram = checkVram();
		float ram = checkRam();
		std::cout << "(MB) VRAM: " << vram << "\tRAM: " << ram << std::endl;
	}

	void runPerformanceTestsGui(float dt)
	{
		float vram = checkVram();
		float ram = checkRam();
		
		ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
		ImGui::SetNextWindowSize(ImVec2(250.f, 60.f));

		double time = m_internalTimer.timeElapsed();
		if (time >= m_sampleFrequency)
		{
			m_frameRate = 1.f / dt;
			m_internalTimer.restart();
		}
			
		ImGui::Begin("Counter", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("RAM: %d Mb", (int)ram);
		ImGui::Text("VRAM: %d Mb", (int)vram);
		ImGui::Text("%.0f FPS ", m_frameRate);
		ImGui::End();
	}
};


