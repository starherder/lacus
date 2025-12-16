#include "imform.h"

namespace imgui
{
	ImGuiForm::ImGuiForm()
	{
		_visible = false;
		_pending_destroy = false;
	}

	ImGuiForm::~ImGuiForm()
	{

	}
	
	bool ImGuiForm::getPendingDestroy()
	{
		return _pending_destroy;
	}

	bool ImGuiForm::getVisible()
	{
		return _visible;
	}

	void ImGuiForm::setName(const std::string& name)
	{
		_name = name;
	}

	std::string ImGuiForm::getName()
	{
		return _name;
	}

	void ImGuiForm::show(bool visible)
	{
		_visible = visible;

		if (_visible)
		{
			onShow();
		}
	}

	void ImGuiForm::close()
	{
		_visible = false;
		_pending_destroy = true;
	}

	void ImGuiForm::onShow() 
	{
	}

	void ImGuiForm::drawForm()
	{
		if (_visible)
		{
			draw();
		}
	}
}
