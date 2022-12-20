fn printf :: string -> ... -> i64

fn add :: i64 -> i64 -> i64
add x y = x + y

fn gcd :: i64 -> i64 -> i64
gcd x y = if x == y 
            then x 
            else 
                if x > y 
                then gcd @ x - y y
                else gcd @ x y - x

fn fib :: i64 -> i64 
fib x = if x == 0
        then 0
        else if x == 1
        then 1
        else (fib @ x - 1) + (fib @ x - 2)

fn main :: i64
main = 
    let add_result = add @ 1 2
        gcd_result = gcd @ 24 15
        fib_result = fib @ 10
    in  printf @ "1 + 2 = %d, gcd @ 24 15 = %d, fib @ 10 = %d" add_result gcd_result fib_result
