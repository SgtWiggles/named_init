#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <named_init.h>

struct Rect {
	BEGIN_REFLECT(Rect);
	ENABLE_BRACED_INIT(Rect);

	Rect(int sx, int sy, int sz) : x(sx), y(sy), yay(std::make_unique<int>(sz)){};

	FIELD(int, x, 100);
	FIELD(int, y, 0);
	FIELD(std::unique_ptr<int>, yay);

	END_REFLECT();
};

TEST_CASE("Default initialization", "[construct]") {
	auto r = Rect{};
	REQUIRE(r.x == 100);
	REQUIRE(r.y == 0);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Partial construction", "[construct]") {
	auto r = Rect{Rect::x_{10}};
	REQUIRE(r.x == 10);
	REQUIRE(r.y == 0);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Complete construction", "[construct]") {
	auto r = Rect{Rect::x_{10}, Rect::y_{10}};
	REQUIRE(r.x == 10);
	REQUIRE(r.y == 10);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Unordered construction", "[construct]") {
	auto r = Rect{Rect::y_{10}, Rect::x_{10}};
	REQUIRE(r.x == 10);
	REQUIRE(r.y == 10);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Ordered construction all args", "[construct]") {
	auto r = Rect{1, 2};
	REQUIRE(r.x == 1);
	REQUIRE(r.y == 2);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Ordered construction one arg", "[construct]") {
	auto r = Rect{1};
	REQUIRE(r.x == 1);
	REQUIRE(r.y == 0);
	REQUIRE(r.yay == nullptr);
}

TEST_CASE("Move semantics", "[construct]") {
	auto r = Rect{Rect::yay_{std::make_unique<int>(0)}};
	REQUIRE(r.x == 100);
	REQUIRE(r.y == 0);
	REQUIRE(r.yay != nullptr);
}

TEST_CASE("Construct Func", "[construct]") {
	auto r = construct<Rect>(1, 2, std::make_unique<int>(3));
	REQUIRE(r.x == 1);
	REQUIRE(r.y == 2);
	REQUIRE(r.yay != nullptr);
	REQUIRE(*r.yay == 3);
}
