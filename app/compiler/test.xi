fn add :: i64 -> i64 -> i64
add x y = x + y

fn pow :: i64 -> i64 -> i64
pow x y = if y == 0 then 1 else x * pow (x y - 1)

fn printf :: string -> ... -> i64

fn main :: i64
main = printf("pow(2 10) = %d" pow(2 10))
