#include "index.hpp"

Index::Index(int i, int w, bool inc) :
	move{std::bind((inc ? &Index::inc : &Index::dec), this)},
	check{std::bind((inc ? &Index::check_ceiling : &Index::check_floor), this)},
	i{i},
	width{w},
	size{width * width}
{}
int Index::val() const { return i; }


IndexX::IndexX(int i, int width, bool inc) : Index(i, width, inc) {}
void IndexX::dec() { --i; }
void IndexX::inc() { ++i; }
bool IndexX::check_floor() const
{
	return i >= 0 && i % width != width - 1;
}
bool IndexX::check_ceiling() const
{
	return i < size && i % width != 0;
}


IndexY::IndexY(int i, int width, bool inc) : Index(i, width, inc) {}
void IndexY::dec() { i -= width; }
void IndexY::inc() { i += width; }
bool IndexY::check_floor() const
{
	return i >= 0;
}
bool IndexY::check_ceiling() const
{
	return i < size;
}

IndexU::IndexU(int i, int width, bool inc) : Index(i, width, inc) {}
void IndexU::dec() { i = i + width - 1; }
void IndexU::inc() { i = i - width + 1; }
bool IndexU::check_floor() const
{
	return i >= 0 && i % width != 0;
}
bool IndexU::check_ceiling() const
{
	return i < size && i % width != width - 1;
}

IndexD::IndexD(int i, int width, bool inc) : Index(i, width, inc) {}
void IndexD::dec() { i = i - width - 1; }
void IndexD::inc() { i = i + width + 1; }
bool IndexD::check_floor() const
{
	return i >= 0 && i % width != width - 1;
}
bool IndexD::check_ceiling() const
{
	return i < size && i % width != 0;
}