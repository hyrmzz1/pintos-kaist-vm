#define F (1 << 14) /* fixed point 1 */
#define INT_MAX ((1 << 31) - 1)
#define INT_MIN (-(1 << 31))
/* x, y <-- fixed_point */
/* n <-- integer */

int int_to_fp(int n);         
int fp_to_int_round(int x);   
int fp_to_int(int x);         
int fp_add(int x, int y);    
int add_complex(int x, int n);  
int fp_sub(int x, int y);     
int sub_complex(int x, int n);  
int fp_mult(int x, int y);    
int mult_complex(int x, int y); 
int fp_div(int x, int y);    
int div_complex(int x, int n);  


/* int --> fp */
int int_to_fp(int n) 
{
    return n * F;
}

/* fp --> int */
int fp_to_int(int x) 
{
    return x / F;
}

/* fp to int 반올림 */
int fp_to_int_round(int x) 
{
    if (x >= 0)
        return (x + F / 2) / F;
    else
        return (x - F / 2) / F;
}

/* fp + fp */
int fp_add(int x, int y) 
{
    return x + y;
}

/* fp + int */
int add_complex(int x, int n) 
{
    return x + n * F;
}

/* fp - fp  */
int fp_sub(int x, int y) 
{
    return x - y;
}

/* fp - int */
int sub_complex(int x, int n) 
{
    return x - n * F;
}

/* fp * fp */
int fp_mult(int x, int y) 
{
    return ((int64_t)x) * y / F;
}

/* fp * int  */
int mult_complex(int x, int n) 
{
    return x * n;
}

/* fp / fp */
int fp_div(int x, int y) 
{
    return ((int64_t)x) * F / y;
}

/* fp / int */
int div_complex(int x, int n) 
{
    return x / n;
}