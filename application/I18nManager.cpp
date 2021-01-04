#include "I18nManager.h"
#include <fstream>
#include <iostream>
#include <spirit_po/spirit_po.hpp>

using namespace Hix::Application;

constexpr auto DEFAULT_PO = "po/default.po";


std::string htr()
{
	return std::string();
}

Hix::Application::I18nManager::I18nManager()
{
	setlocale(LC_ALL, "");

	std::ifstream ifs(DEFAULT_PO);
	std::string po_file{ std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>() };
	_cat{ spirit_po::default_catalog::from_range(po_file) };
}

