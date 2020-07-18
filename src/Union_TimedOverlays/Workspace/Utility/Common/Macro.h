#define COA2(a, b) ( (a) ? (a)->b : decltype((a)->b)() )
#define COA3(a, b, c) ( (a) ? COA2((a)->b, c) : decltype((a)->b->c)() )
#define COA4(a, b, c, d) ( (a) ? COA3((a)->b, c, d) : decltype((a)->b->c->d)() )
#define COA5(a, b, c, d, e) ( (a) ? COA4((a)->b, c, d, e) : decltype((a)->b->c->d->e)() )
#define COA6(a, b, c, d, e, f) ( (a) ? COA5((a)->b, c, d, e, f) : decltype((a)->b->c->d->e->f)() 

#define PIF ((float)PI)