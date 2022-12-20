fn add :: i64 -> i64 -> i64
add x y = x + y

fn pow :: i64 -> i64 -> i64
pow x y = if y == 0 then 1 else x * pow (x y - 1)

fn check :: buer -> i64 -> i64 -> i64
check x y z = if x then y else z

fn printf :: string -> ... -> i64

fn test_let :: i64 -> i64
test_let x = let y = x + 1 in y + 1

set point = ((x i64) (y i64))

set point2 = ((p1 point) (p2 point))

fn test_if :: i64 -> i64
test_if x = if 
                if x < 5
                then true
                else false
            then 1000
            else -1000

fn test_set :: point -> i64
test_set p = p.x + p.y

fn test_set2 :: point2 -> i64
test_set2 p = test_set(p.p1) + test_set(p.p2)

fn test_array :: i64 -> i64
test_array x = let a = [1, 2, 3, 4, 5] in a[x]

fn test_set_array :: i64 -> i64
test_set_array x = let a = [point(0 0), point(1 1), 
        point(2 2)] in a[x].y

fn main :: i64
main = printf(
    "pow(2 10) = %d, test_let = %d, test_set = %d, test_set2 = %d, test_if = %d, test_array = %d test_set_array = %d" 
    pow(check(2 == 3 1 2) 10) 
    test_let(1)
    test_set(point(3 100))
    test_set2(point2(point(1 2) point(3 4)))
    test_if(3)
    test_array(3)
    test_set_array(1)
)
