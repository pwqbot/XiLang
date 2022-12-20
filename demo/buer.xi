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
