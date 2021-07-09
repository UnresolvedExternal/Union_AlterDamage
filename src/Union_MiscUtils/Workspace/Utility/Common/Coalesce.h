#define COA1(a) (a)
#define COA2(a, b) ( (a) ? (a)->b : decltype((a)->b)() )
#define COA3(a, b, c) ( (a) ? COA2((a)->b, c) : decltype((a)->b->c)() )
#define COA4(a, b, c, d) ( (a) ? COA3((a)->b, c, d) : decltype((a)->b->c->d)() )
#define COA5(a, b, c, d, e) ( (a) ? COA4((a)->b, c, d, e) : decltype((a)->b->c->d->e)() )
#define COA6(a, b, c, d, e, f) ( (a) ? COA5((a)->b, c, d, e, f) : decltype((a)->b->c->d->e->f)() 

#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define COA(...) EXPAND(GET_MACRO(__VA_ARGS__, COA6, COA5, COA4, COA3, COA2, COA1))EXPAND((__VA_ARGS__))
