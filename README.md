# XiLang

## compile

use vcpkg as package manager
`https://vcpkg.io/en/index.html`

build with cmake

``` bash
mkdir build
cd build
cmake .. -DCMAKE_TOOL_CHAIN_FILE=...
cmake --build .
```

## syntax

```haskell
xi sb = ?x y -> x + y
xi sbb = sb(1 2)

```

## reference

<https://www.youtube.com/watch?v=QwaoOYkoqB0>
