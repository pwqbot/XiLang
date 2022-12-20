fn printf :: string -> ... -> i64

set point = ((x i64) (y i64))

fn dis :: point -> point -> i64
dis p1 p2 = let x_dis = p1.x - p2.x 
                y_dis = p1.y - p2.y
            in x_dis * x_dis + y_dis * y_dis

fn main :: i64
main = let p1 = point @1 2
           p2 = point @3 4
       in printf @ "dis (1 2) (3 4) = %d" dis @ p1 p2
