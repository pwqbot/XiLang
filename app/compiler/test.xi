fn add :: i64 -> i64 -> i64
add x y = x + y

fn pow :: i64 -> i64 -> i64
pow x y = if y == 0 then 1 else x * pow (x y - 1)

fn check :: buer -> i64 -> i64 -> i64
check x y z = if x then y else z

fn printf :: string -> ... -> i64

fn test_let :: i64 -> i64
test_let x = let y = x + 1 in y + 1

fn test_array :: arr[i64] -> i64

set point = ((x i64) (y i64))

fn test_set :: point -> i64
test_set p = p.x + p.y

fn main :: i64
main = printf(
    "pow(2 10) = %d, test_let = %d, test_set = %d" 
    pow(check(2 == 3 1 2) 10) 
    test_let(1)
    test_set(point(3 100))
)
