# named_init
Allows you to construct structs with named arguments.

### Usage
```
#include<memory>  // unique_ptr
#include<cassert> // assert

#include "named_init.h"

struct Fancy {
	BEGIN_REFLECT(Fancy); // Required
	ENABLE_BRACED_INIT(Fancy); // Allows the use of the syntax Fancy{ ... }

	Fancy(int x1, int y1, int yay1)  // Can use user defined constructors but construct will not call them
		: x(x1)
		, y(y1)
		, yay(std::make_unique<int>(yay1))
		{}

	FIELD(int, x, 100); // Declares a field with type int, named x with default value 100

	int y = 0;
	FIELD(y);  // Field can also be declared out of line

	FIELD(std::unique_ptr<int>, yay); // Declares a field with out a default value

	END_REFLECT(); // Required
};

using x_ = Fancy::x_; // Shorten names for fields, underscore at the end is mandatory
using y_ = Fancy::y_;

int main() {
	auto r1 = Fancy{1, 2, std::make_unique<int>(3)}; // Allows for normal ordered construction 
	assert(r1.x == 1);
	assert(r1.y == 2);
	assert(*r1.yay == 3);

	auto r2 = construct<Fancy>(1,2,std::make_unique<int>(3));  // Or by a function
	assert(r1.x == 1);
	assert(r1.y == 2);
	assert(*r2.yay == 3);


	auto r3 = Fancy{ y_{11}, x_{10} }; // With named arguments in any order
	assert(r3.x == 10);
	assert(r3.x == 11);

	auto r4 = Fancy{1, 2, 3}; // Call user defined constructor
	assert(r4.x == 1);
	assert(r4.y == 2);
	assert(*r4.yay == 3);
}

```

### Installing
As the library is header only, just copy `named_init.h` into your source tree and include it.
Requires C++17.

## Running the tests
The tests rely on meson and catch2, although catch2 is included in the source tree.
To run the tests run the following after cloning the project.
```
cd named_init
meson build
cd build
ninja test
```

