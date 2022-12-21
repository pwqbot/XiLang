# XiLang
XiLang is a pure functional language inspired by Haskell. 
It has the following features:

- Pure functional: Everything is non-mutable and function must not have side-effect.

- Parser combinator frontend: the parser is built in parser combinator style, 
which allows for super expressive and efficient 
definition of the grammar of the language.

see how it parse a `if ... then ... else ...` expr
```cpp
inline const auto Xi_if = token(s_if) > Xi_expr >> [](Xi_Expr cond)
{
    return token(s_then) > Xi_expr >> [cond](Xi_Expr then)
    {
        return token(s_else) > Xi_expr >> [cond, then](Xi_Expr els)
        {
            return unit(Xi_Expr{
                Xi_If{
                    .cond = cond,
                    .then = then,
                    .els  = els,
                },
            });
        };
    };
};
```

- Type system: Xi has a static type system that checks the type correctness of the 
program. It supports basic types such as i64, real, ...
as well as compound types such as arr[T] and user defined type.
```haskell
fn add :: i64 -> i64 -> i64
add x y = x + y

// this will cause compile time error
add @ true 1 
```

- Type inference: The compiler can infer the types of variables and 
expressions based on their values and the context in which they are used, you
only need to write type for function signature.

- LLVM backend: The compiler uses LLVM as its code generation backend. 
This allows the compiler to generate efficient and optimized 
machine code for a wide range of platforms.


## Getting Started

To build the compiler, you will need the following dependencies:

- A C++20 compiler (such as gcc12)
- CMake
- vcpkg

build with cmake

``` bash
mkdir build
cd build
cmake .. -DCMAKE_TOOL_CHAIN_FILE=...
cmake --build .
```

you can use 
```
./app/compiler/compiler <input_file> -o <output_file>

```
The compiler will generate an LLVM IR file <output_file>.ll and an object file <output_file>.o, 
and use Clang to link the object file to generate the executable <output_file>.


## example

Here is a simple example program in Xi:
```haskell
fn printf :: string -> ... -> i64

fn isLunarYear :: i64 -> string
isLunarYear year = 
    let is_lunar_year = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
    in  if is_lunar_year
        then "true"
        else "false"

fn main :: i64
main = 
    let lunar_year_2000 = isLunarYear @ 2000
        lunar_year_2001 = isLunarYear @ 2001
    in  printf @ "2000 is lunar year? %s 2001 is lunar year? %s" lunar_year_2000 lunar_year_2001
```
For more examples of programs written in Xi, see the demo/ directory.

## grammar

``` haskell
Program = { Stmt } .

Stmt = Decl | Func .

Decl = "fn" Iden "::" ParamsType "->" Type .

Func = "fn" Iden "::" ParamsType "->" Type "=" Expr .

Expr = Or .

Or = And { "|" And } .

And = Comparison { "&" Comparison } .

Comparison = Addition { ( "==" | "!=" | "<" | ">" | "<=" | ">=" ) Addition } .

Addition = Multiplication { ( "+" | "-" ) Multiplication } .

Multiplication = Unary { ( "*" | "/" | "%" ) Unary } .

Unary = ( "-" | "!" | "&" ) Unary | Call .

Call = Primary { "(" [ Expr { "," Expr } ] ")" } .

Primary = Iden | Integer | Real | String | Boolean | "(" Expr ")" | Array | ArrayIndex | Let | If .

Iden = alphabetic { alphabetic | digit } .

Integer = digit { digit } .

Real = digit { digit } "." digit { digit } .

String = "\"" { any } "\"" .

Boolean = "true" | "false" .

Array = "[" [ Expr { "," Expr } ] "]" .

ArrayIndex = Iden "[" Expr "]" .

Let = "let" Iden "=" Expr "in" Expr .

If = "if" Expr "then" Expr "else" Expr .

ParamsType = Type { "->" Type } .

Type = "i64" | "real" | "bool" | "string" | "arr" "[" Type "]" | Iden .

```

## reference

<https://www.youtube.com/watch?v=QwaoOYkoqB0>
