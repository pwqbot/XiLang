fn printf :: string -> ... -> i64

fn main :: i64
main = {
    int a = 2;
    int b = 0;
    while(a > 0) {
        a--;
        b++;
    }
    int i;
    for (i = 0; i < 10; i++) {
        b++;
    }
    if (a < b) {
        a = b;
    }
    printf @ "a = %d b = %d" a b;
    return 1;
}
