fn printf :: string -> ... -> i64

set point = ((x i64) (y i64))

set line = ((a point) (b point))

fn lines :: arr[line]
lines = [line @(point @0 0) (point @ 1 1),
         line @(point @1 1) (point @ 2 2),
         line @(point @3 3) (point @ 4 3)]

fn crossProduct :: point -> point -> i64
crossProduct p1 p2 = p1.x * p2.y - p1.y * p2.x

fn isIntersec :: line -> line -> buer
isIntersec l1 l2 = let a1 = l1.a
                       b1 = l1.b
                       a2 = l2.a
                       b2 = l2.b
                   in (a1.x - b1.x) * (a2.y - b2.y) - (a1.y - b1.y) * (a2.x - b2.x) != 0

fn toI64 :: buer -> i64
toI64 b = if b then 1 else 0

fn calIntersecPoint_ :: line -> arr[line] -> i64 -> i64
calIntersecPoint_ l lines x = let toAdd = toI64 @isIntersec@ l lines[x - 1] 
                              in if x == 0
                              then 0
                              else (calIntersecPoint_ @ l lines x - 1 ) + toAdd


fn calIntersecPoint :: arr[line] -> i64 -> i64
calIntersecPoint lines_ length = if length == 1
                                 then 0
                                 else (calIntersecPoint  @lines_ length - 1) + 
                                      (calIntersecPoint_ @lines_[length - 1] lines_ length - 1)

fn main :: i64
main = printf @ "has %d intersecPoints" calIntersecPoint @ (lines @) 3
