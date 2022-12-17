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
fn pow :: i64 -> i64 -> i64
pow x y = if y <= 0 then 1 else x * pow(x y - 1)

set point = ((x i64) (y i64))
```

## reference

<https://www.youtube.com/watch?v=QwaoOYkoqB0>
