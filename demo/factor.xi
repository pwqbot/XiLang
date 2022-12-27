fn printf :: string -> ... -> i64

fn main :: i64
main = {
    int n;
    while(n > 0)
    {
        int ans;
        ans = 1;
        n = 1;
        while(n-- > 0)
        {
            ans = ans * n;
        };
        printf @ "factorial: %d " ans;
    };
    return 0;
}
