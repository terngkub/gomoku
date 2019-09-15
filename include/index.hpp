#pragma once
#include <functional>

class Index
{
public:
    std::function<void()> move;
    std::function<bool()> check;

	Index(int i, int w, bool inc);
	int val() const;
	virtual void inc() = 0;
	virtual void dec() = 0;
	virtual bool check_floor() const = 0;
	virtual bool check_ceiling() const = 0;

protected:
	int i;
    int const width;
    int const size;
};

class IndexX : public Index
{
public:
    IndexX(int i, int width, bool inc);
	void dec() override;
	void inc() override;
	bool check_floor() const override;
	bool check_ceiling() const override;
};

class IndexY : public Index
{
public:
    IndexY(int i, int width, bool inc);
	void dec() override;
	void inc() override;
	bool check_floor() const override;
	bool check_ceiling() const override;
};

class IndexU : public Index
{
public:
    IndexU(int i, int width, bool inc);
	void dec() override;
	void inc() override;
	bool check_floor() const override;
	bool check_ceiling() const override;
};

class IndexD : public Index
{
public:
    IndexD(int i, int width, bool inc);
	void dec() override;
	void inc() override;
	bool check_floor() const override;
	bool check_ceiling() const override;
};