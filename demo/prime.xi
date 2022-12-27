fn printf :: string -> ... -> i64

fn main :: i64
main = {
    int a;
    a = 2;
    while(a < 101){
        int i;
        int f;
        f = 0;
        i = 2;
        while(i * i <= a){
            int b;
            b = a / i;
            if((b * i) == a){
                f = 1;
            };
            i = i + 1;
        };
        if(f == 0)
            printf @ "prime: %d " a;
        a = a + 1;
    }
    return 0;
}
