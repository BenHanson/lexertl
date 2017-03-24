lexertl: The Modular Lexical Analyser Generator
=======

lexertl is a header-only library for writing lexer analyzers. With lexertl you can:

- build lexical scanner at runtime
- scan Unicode and ASCII input
- scan from file or memory
- generate C++ code or even write your own code generator

### Construct a Lexer and Tokenize input

```cpp
#include "lexertl/generator.hpp"
#include "lexertl/lookup.hpp"
#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::smatch results(input.begin(), input.end());

    // Read ahead
    lexertl::lookup(sm, results);

    while (results.id != 0)
    {
        std::cout << "Id: " << results.id << ", Token: '" <<
            results.str () << "'\n";
        lexertl::lookup(sm, results);
    }

    return 0;
}
```

The same using lexertl::iterator:

```cpp
#include "lexertl/generator.hpp"
#include "lexertl/iterator.hpp"
#include "lexertl/lookup.hpp"
#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::siterator iter(input.begin(), input.end(), sm);
    lexertl::siterator end;

    for (; iter != end; ++iter)
    {
        std::cout << "Id: " << iter->id << ", Token: '" <<
            iter->str() << "'\n";
    }

    return 0;
}
```

## More examples and documentation

See http://www.benhanson.net/lexertl.html for full documentation and more usage examples.
