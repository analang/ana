<?php

declare(strict_types=1);

function fib($n): int
{
  if ($n <= 1)
  {
    return $n;
  }
  
  return fib( $n - 1) + fib( $n - 2);
}

print(fib(9));
