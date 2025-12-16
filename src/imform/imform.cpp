#include "imform.h"

namespace imgui
{
	ImForm::ImForm()
	{
		_visible = false;
		_pending_destroy = false;
	}

	ImForm::~ImForm()
	{

	}
	
	bool ImForm::getPendingDestroy()
	{
		return _pending_destroy;
	}

	bool ImForm::getVisible()
	{
		return _visible;
	}

	void ImForm::setName(const std::string& name)
	{
		_name = name;
	}

	std::string ImForm::getName()
	{
		return _name;
	}

	void ImForm::show(bool visible)
	{
		_visible = visible;

		if (_visible)
		{
			onShow();
		}
	}

	void ImForm::close()
	{
		_visible = false;
		_pending_destroy = true;
	}

	void ImForm::onShow() 
	{
	}

	void ImForm::drawForm()
	{
		if (_visible)
		{
			draw();
		}
	}
}
